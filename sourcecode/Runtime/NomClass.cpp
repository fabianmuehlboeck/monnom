#include "NomClass.h"
#include "NomConstants.h"
#include "NomInstantiationRef.h"
#include <vector>
#include <map>
#include "Context.h"
#include "NomType.h"
#include "NomClassType.h"
#include "NomTypeVar.h"
#include "llvm/ADT/Twine.h"
#include "llvm/ADT/SmallVector.h"
#include "NomJIT.h"
#include "GlobalNameAddressLookupList.h"
#include "NomNameRepository.h"
#include "RTSubtyping.h"
#include "RTOutput.h"
#include "CompileHelpers.h"
#include <iostream>
#include "llvm/Support/raw_os_ostream.h"
#include "llvm/IR/Verifier.h"
#include "RTDescriptor.h"
#include "NomPartialApplication.h"
#include "RTVTable.h"
#include "NomMethodTableEntry.h"
#include "NomCallableVersion.h"
#include "RTInterfaceTableEntry.h"
#include "instructions/CastInstruction.h"
#include "RTInterface.h"
#include "IntClass.h"
#include "FloatClass.h"
#include "CompileEnv.h"
#include "RTCast.h"
#include "NomDynamicType.h"
#include "instructions/CallDispatchBestMethod.h"
#include "NomTypeParameter.h"
#include "ObjectClass.h"
#include "CallingConvConf.h"
#include "IMT.h"
#include "RTCompileConfig.h"
#include "NomInterfaceCallTag.h"
#include "EnsureDynamicMethodInstruction.h"
#include "llvm/ADT/SmallPtrSet.h"
#include "NomLambdaCallTag.h"
#include "Metadata.h"

namespace Nom
{
	namespace Runtime
	{
		using namespace std;
		using namespace llvm;
		NomClassLoaded::NomClassLoaded(const ConstantID name, ConstantID typeArgs, ConstantID superClass, const ConstantID superInterfaces, const NomMemberContext* parent) : NomInterface(NomConstants::GetString(name)->GetText()->ToStdString()), NomInterfaceLoaded(name, typeArgs, superInterfaces, parent), /*NomMemberContextLoaded(parent, typeArgs), NomNamedLoaded(name, typeArgs, parent),*/ superClass(superClass)
		{
			RegisterClass(NomConstants::GetString(name)->GetText(), this);
		}

		NomStaticMethodLoaded* NomClassLoaded::AddStaticMethod(const std::string& name, const std::string& qname, const ConstantID typeArgs, const ConstantID returnType, const ConstantID arguments, const RegIndex regcount)
		{
			NomStaticMethodLoaded* meth = new NomStaticMethodLoaded(name, this, qname, returnType, typeArgs, arguments, regcount);
			NomClass::AddStaticMethod(meth);
			return meth;
		}

		NomConstructorLoaded* NomClassLoaded::AddConstructor(const ConstantID arguments, const RegIndex regcount) {
			auto name = "_Constructor_" + this->GetName()->ToStdString() + "_" + std::to_string(Constructors.size());
			NomConstructorLoaded* constr = new NomConstructorLoaded(this, name, name, arguments, regcount, 0, false);
			NomClass::AddConstructor(constr);
			return constr;
		}

		NomTypedField* NomClassLoaded::AddField(const ConstantID name, const ConstantID type, Visibility visibility, bool isReadOnly, bool isVolatile) {
			NomTypedField* field = new NomTypedField(this, name, type, visibility, isReadOnly, isVolatile);
			NomClass::AddField(field);
			return field;
		}

		NomLambda* NomClassLoaded::AddLambda(const ConstantID lambdaID, int regcount, ConstantID closureTypeParams, ConstantID closureArguments, ConstantID typeParams, ConstantID argTypes, ConstantID returnType) {
			NomLambda* lambda = new NomLambda(lambdaID, nullptr, regcount, closureTypeParams, closureArguments, typeParams, argTypes, returnType);
			NomClass::AddLambda(lambda);
			return lambda;
		}

		NomRecord* NomClassLoaded::AddStruct(const ConstantID structID, ConstantID closureTypeParams, RegIndex regcount, RegIndex endargregcount, ConstantID initializerArgTypes) {
			NomRecord* structure = new NomRecord(structID, nullptr, closureTypeParams, regcount, endargregcount, initializerArgTypes);
			NomClass::AddStruct(structure);
			return structure;
		}

		void NomClass::CompileLLVM(Module* mod) const
		{
			if (compiled)
			{
				return;
			}
			compiled = true;
			std::vector<NomMethod*> methods;
			auto sclass = GetSuperClass();
			if (sclass.HasElem())
			{
				sclass.Elem->CompileLLVM(mod);
			}
			NomBuilder builder;
			for (auto meth = Methods.begin(); meth != Methods.end(); meth++)
			{
				(*meth)->GetLLVMElement(*mod);
				//meth->Compile(builder, mod);
			}
			for (auto meth = StaticMethods.begin(); meth != StaticMethods.end(); meth++)
			{
				(*meth)->GetLLVMElement(*mod);
				//meth->Compile(builder, mod);
			}
			for (auto cons = Constructors.begin(); cons != Constructors.end(); cons++)
			{
				(*cons)->GetLLVMElement(*mod);
			}
		}



		NomValue NomClass::GenerateConstructorCall(NomBuilder& builder, CompileEnv* env, const TypeList typeArgs, llvm::Value* objpointer, llvm::ArrayRef<NomValue> args) const
		{
			auto argssize = args.size();
			NomTypeRef* argTypesBuf = (NomTypeRef*)(nmalloc(sizeof(NomTypeRef) * argssize));
			llvm::Value** argValuesBuf = (llvm::Value**)(nmalloc(sizeof(llvm::Value*) * (argssize + 1 + typeArgs.size())));
			size_t argspos = 0;
			argValuesBuf[argspos] = builder->CreatePointerCast(objpointer, REFTYPE);
			argspos++;
			for (auto& typeArg : typeArgs)
			{
				argValuesBuf[argspos] = typeArg->GenerateRTInstantiation(builder, env);
				argspos++;
			}
			for (size_t i = 0; i < argssize; i++)
			{
				argTypesBuf[i] = args[i].GetNomType();
				argValuesBuf[argspos + i] = args[i];
			}
			auto argTypes = llvm::ArrayRef<NomTypeRef>(argTypesBuf, argssize);
			auto argValues = llvm::ArrayRef<llvm::Value*>(argValuesBuf, argssize + 1 + typeArgs.size());
			NomSubstitutionContextList nscl(typeArgs);
			for (auto constructor : Constructors)
			{
				if (constructor->Satisfies(&NomSubstitutionContextList::EmptyContext(), typeArgs, argTypes)) //TODO: make pessimistic and go to dispatcher later
				{
					auto fun = constructor->GetLLVMFunction(env->Module);
					auto funtype = fun->getFunctionType();
					auto constructorSignature = constructor->Signature(&nscl);
					for (size_t i = 0; i < argssize; i++)
					{
						if (!argTypesBuf[i]->IsSubtype(constructorSignature.ArgumentTypes[i], false))
						{
							argValuesBuf[argspos + i] = CastInstruction::MakeCast(builder, env, args[i], constructorSignature.ArgumentTypes[i]);
						}
						auto paramtype = funtype->getParamType(argspos + i);
						if (paramtype->isIntegerTy() && !argValuesBuf[argspos + i]->getType()->isIntegerTy())
						{
							argValuesBuf[argspos + i] = UnpackInt(builder, argValuesBuf[argspos + i]);
						}
						else if (paramtype == FLOATTYPE && argValuesBuf[argspos + i]->getType() != FLOATTYPE)
						{
							argValuesBuf[argspos + i] = UnpackFloat(builder, argValuesBuf[argspos + i]);
						}
						else if ((!paramtype->isIntegerTy()) && argValuesBuf[argspos + i]->getType()->isIntegerTy())
						{
							argValuesBuf[argspos + i] = PackInt(builder, argValuesBuf[argspos + i]);
						}
						else if (paramtype != FLOATTYPE && argValuesBuf[argspos + i]->getType() == FLOATTYPE)
						{
							argValuesBuf[argspos + i] = PackFloat(builder, argValuesBuf[argspos + i]);
						}
					}
					auto callInst = builder->CreateCall(fun, argValues);
					callInst->setCallingConv(NOMCC);
					//callInst->setTailCallKind(llvm::CallInst::TailCallKind::TCK_Tail);
					return NomValue(callInst, this->GetType(typeArgs), true);
				}
			}
			throw new std::exception(); //TODO : call dispatcher
		}

		NomInstantiationRef<const NomStaticMethod> NomClass::GetStaticMethod(NomStringRef methodName, const TypeList typeArgs, const TypeList argTypes) const
		{
			for (auto meth : StaticMethods)
			{
				if (meth->GetName() == methodName->ToStdString() && meth->Satisfies(&NomSubstitutionContextList::EmptyContext(), typeArgs, argTypes))
				{
					return NomInstantiationRef<const NomStaticMethod>(meth, typeArgs);
				}
			}
			throw methodName;
		}

		NomInstantiationRef<const NomConstructor> NomClass::GetConstructor(const TypeList typeArgs, const TypeList argTypes) const
		{
			for (auto constructor : Constructors)
			{
				if (constructor->Satisfies(&NomSubstitutionContextList::EmptyContext(), typeArgs, argTypes))
				{
					return NomInstantiationRef<const NomConstructor>(constructor, typeArgs);
				}
			}
			throw new std::exception();
		}

		llvm::Constant* NomClass::GetSuperInstances(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage, llvm::GlobalVariable* gvar, llvm::StructType* stetype) const
		{
			auto instantiations = GetInstantiations();
			auto instaCount = instantiations.size();
			auto entryArr = makealloca(Constant*, instaCount);
			auto fieldArr = makealloca(Constant*, instaCount + 1);
			auto typeArr = makealloca(Type*, instaCount + 1);
			auto orderedInstas = makealloca(const NomInterface*, instaCount);
			typeArr[0] = arrtype(SuperInstanceEntryType(), instaCount);
			int pos = GetSuperClassCount();
			auto curSuper = GetSuperClass();
			while (curSuper.HasElem())
			{
				pos--;
				auto instantiation = instantiations[curSuper.Elem];
				size_t instasize = instantiation.size();
				typeArr[pos + 1] = arrtype(TYPETYPE, instasize);
				llvm::Constant** instaArgBuf = makealloca(llvm::Constant*, instasize);
				for (size_t i = 0; i < instasize; i++)
				{
					instaArgBuf[instasize - (i + 1)] = instantiation[i]->GetLLVMElement(mod);
				}
				fieldArr[pos + 1] = ConstantArray::get(arrtype(TYPETYPE, instasize), ArrayRef<Constant*>(instaArgBuf, instasize));
				orderedInstas[pos] = curSuper.Elem;
				curSuper = curSuper.Elem->GetSuperClass();
			}
			pos = GetSuperClassCount();
			for (auto& instantiation : instantiations)
			{
				if (instantiation.first->IsInterface())
				{
					size_t instasize = instantiation.second.size();
					typeArr[pos + 1] = arrtype(TYPETYPE, instasize);
					llvm::Constant** instaArgBuf = makealloca(llvm::Constant*, instasize);
					for (size_t i = 0; i < instasize; i++)
					{
						instaArgBuf[instasize - (i + 1)] = instantiation.second[i]->GetLLVMElement(mod);
					}
					fieldArr[pos + 1] = ConstantArray::get(arrtype(TYPETYPE, instasize), ArrayRef<Constant*>(instaArgBuf, instasize));
					orderedInstas[pos] = instantiation.first;
					pos++;
				}
			}
			if (stetype->isOpaque())
			{
				stetype->setBody(ArrayRef<Type*>(typeArr, instaCount + 1), false);
			}
			while (pos > 0)
			{
				pos--;
				auto instantiation = instantiations[orderedInstas[pos]];
				size_t instasize = instantiation.size();
				entryArr[pos] = ConstantStruct::get(SuperInstanceEntryType(),
					ConstantExpr::getPointerCast(orderedInstas[pos]->GetLLVMElement(mod), RTInterface::GetLLVMType()->getPointerTo()),
					ConstantExpr::getGetElementPtr(gvar->getValueType(), gvar, ArrayRef<Constant*>({ MakeInt32(0), MakeInt32(gvar->getValueType()->getStructNumElements() - 1), MakeInt32(pos + 1), MakeInt32(instasize) })));
			}
			fieldArr[0] = ConstantArray::get(arrtype(SuperInstanceEntryType(), instaCount), ArrayRef<Constant*>(entryArr, instaCount));
			return ConstantStruct::get(stetype, ArrayRef<Constant*>(fieldArr, instaCount + 1));
		}

		llvm::ArrayType* NomClass::GetSuperInstancesType(bool generic) const
		{
			return arrtype(SuperInstanceEntryType(), (generic ? 0 : GetInstantiations().size()));
		}


		int NomClass::GetSuperClassCount() const
		{
			size_t superClassCount = 0;
			auto currentSuper = this->GetSuperClass();
			while (currentSuper.HasElem())
			{
				superClassCount++;
				currentSuper = currentSuper.Elem->GetSuperClass();
			}
			return superClassCount;
		}

		llvm::Constant* NomClass::findLLVMElement(llvm::Module& mod) const {
			llvm::Twine t = "NOM_CD_";
			llvm::SmallVector<char, 64> buf;
			llvm::StringRef nameref = t.concat(this->GetName()->ToStdString()).toStringRef(buf);
			return RTClass::FindConstant(mod, nameref);
			//return mod.getGlobalVariable(nameref, false);
		}
		llvm::Constant* NomClass::createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const
		{
			llvm::Twine t = "NOM_CD_";
			llvm::SmallVector<char, 64> buf;
			llvm::StringRef nameref = t.concat(this->GetName()->ToStdString()).toStringRef(buf);
			llvm::Constant* ret = RTClass::FindConstant(mod, nameref);
			if (ret == nullptr)
			{
				StructType* clsStructType = StructType::create(LLVMCONTEXT, "MONNOM_CLSDD_" + *this->GetSymbolName());
				StructType* clsSupersStructType = StructType::create(LLVMCONTEXT, "MONNOM_CLSSUPERS_" + *this->GetSymbolName());
				auto gvartype = RTClass::GetConstantType(GetHasRawInvoke() ? 24 : 16, GetMethodTableType(false), clsStructType, clsSupersStructType);
				GlobalVariable* gvar = new GlobalVariable(mod, gvartype, true, linkage, nullptr, nameref);
				auto superClassCount = GetSuperClassCount();
				ret = RTClass::CreateConstant(gvar, gvartype, this,
					GetDynamicFieldLookup(mod, linkage),
					GetDynamicFieldStore(mod, linkage),
					GetDynamicDispatcherLookup(mod, linkage, clsStructType),
					MakeInt(GetFieldCount()),
					MakeInt(this->GetTypeParametersCount()),
					MakeInt<size_t>(superClassCount),
					MakeInt<size_t>(GetInstantiations().size() - superClassCount),
					GetSuperInstances(mod, linkage, gvar, clsSupersStructType),
					GetMethodTable(mod, linkage),
					ConstantPointerNull::get(GetCheckReturnValueFunctionType()->getPointerTo()),
					GetInterfaceTableLookup(mod, linkage),
					GetSignature(mod, linkage),
					GetCastFunction(mod, linkage));
				new GlobalVariable(mod, ret->getType(), true, linkage, ret, "NOM_CDREF_" + this->GetName()->ToStdString());

				RegisterGlobalForAddressLookup(nameref.str());

				if (GetHasRawInvoke())
				{
					GetRawInvokeFunction(mod, linkage);
				}
			}
			return ret;
		}

		size_t NomClass::GetTypeArgOffset() const
		{
			auto sclass = GetSuperClass();
			size_t offset = 0;
			if (sclass.HasElem())
			{
				offset = sclass.Elem->GetTypeArgOffset() + sclass.Elem->GetTypeParametersCount();
			}
			return offset;
		}


		llvm::Function* NomClass::GetRawInvokeFunction(llvm::Module& mod) const
		{
			GetLLVMElement(mod);
			std::string name = "RT_NOM_RAWINVOKE_" + this->GetName()->ToStdString();
			return mod.getFunction(name);
		}

		llvm::Function* NomClass::GetRawInvokeFunction(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const
		{
			if (NomLambdaOptimizationLevel == 0)
			{
				return nullptr;
			}
			const NomMethod* method = nullptr;
			for (auto mte : MethodTable)
			{
				if (mte->Method->GetName().empty())
				{
					method = mte->Method;
					break;
				}
			}
			if (method == nullptr)
			{
				return nullptr;
			}

			llvm::SmallPtrSet<const NomInterface*, 16> supers;
			vector<const NomInterface*> worklist;
			auto curClass = this;
			while (curClass != nullptr)
			{
				supers.insert(curClass);
				worklist.push_back(curClass);
				curClass = curClass->GetSuperClass().Elem;
			}
			while (worklist.size() > 0)
			{
				auto current = worklist.back();
				worklist.pop_back();
				for (auto& super : current->GetSuperInterfaces())
				{
					if (supers.insert(super.Elem).second)
					{
						worklist.push_back(super.Elem);
					}
				}
			}

			llvm::SmallVector<const NomMethod*, 16> methods;
			for (auto super : supers)
			{
				for (auto& meth : super->Methods)
				{
					if (method->Overrides(meth))
					{
						methods.push_back(meth);
					}
				}
			}

			std::string name = "RT_NOM_RAWINVOKE_" + this->GetName()->ToStdString();
			llvm::Function* fun = Function::Create(GetIMTFunctionType(), linkage, name, mod);
			fun->setCallingConv(NOMCC);
			BasicBlock* startBlock = BasicBlock::Create(LLVMCONTEXT, "start", fun);

			NomBuilder builder;
			builder->SetInsertPoint(startBlock);

			auto argiter = fun->arg_begin();
			auto callTag = argiter;
			argiter++;
			auto varargs = makealloca(Value*, RTConfig_NumberOfVarargsArguments + 1);
			for (decltype(RTConfig_NumberOfVarargsArguments) i = 0; i <= RTConfig_NumberOfVarargsArguments; i++)
			{
				varargs[i] = argiter;
				argiter++;
			}

			for (auto& meth : methods)
			{
				BasicBlock* callBlock = BasicBlock::Create(LLVMCONTEXT, meth->GetName(), fun);
				BasicBlock* nextBlock = BasicBlock::Create(LLVMCONTEXT, "next", fun);

				auto methodCallTag = NomInterfaceCallTag::GetMethodKey(meth);

				auto callTagMatch = builder->CreateICmpEQ(builder->CreatePtrToInt(callTag, numtype(intptr_t)), ConstantExpr::getPtrToInt(methodCallTag->GetLLVMElement(mod), numtype(intptr_t)), "callTagMatch");
				builder->CreateIntrinsic(Intrinsic::expect, { inttype(1) }, { callTagMatch, MakeUInt(1,1) });
				builder->CreateCondBr(callTagMatch, callBlock, nextBlock, GetLikelyFirstBranchMetadata());
				builder->SetInsertPoint(callBlock);

				auto calledFunctionType = meth->GetLLVMFunctionType();
				auto implFunctionType = method->GetLLVMFunctionType();
				auto paramCount = implFunctionType->getNumParams();
				auto argsarr = makealloca(Value*, paramCount);

				for (decltype(paramCount) j = 0; j < paramCount; j++)
				{
					Value* curArg = nullptr;
					if (j < RTConfig_NumberOfVarargsArguments || RTConfig_NumberOfVarargsArguments + 1 == paramCount)
					{
						curArg = varargs[j];
					}
					else
					{
						curArg = MakeInvariantLoad(builder, builder->CreateGEP(varargs[RTConfig_NumberOfVarargsArguments], MakeInt32(j - RTConfig_NumberOfVarargsArguments)), "varArg", AtomicOrdering::NotAtomic);
					}
					auto calledType = calledFunctionType->getParamType(j);
					auto expectedType = implFunctionType->getParamType(j);
					curArg = EnsurePackedUnpacked(builder, curArg, calledType);
					curArg = EnsurePackedUnpacked(builder, curArg, expectedType);
					argsarr[j] = curArg;
				}
				auto callResult = builder->CreateCall(implFunctionType, method->GetLLVMElement(mod), ArrayRef<Value*>(argsarr, paramCount), method->GetQName());
				callResult->setCallingConv(NOMCC);
				auto actualResult = EnsurePackedUnpacked(builder, EnsurePackedUnpacked(builder, callResult, calledFunctionType->getReturnType()), POINTERTYPE);
				builder->CreateRet(actualResult);

				builder->SetInsertPoint(nextBlock);
			}
			{
				BasicBlock* callBlock = BasicBlock::Create(LLVMCONTEXT, method->GetName(), fun);
				BasicBlock* failBlock = RTOutput_Fail::GenerateFailOutputBlock(builder, "Invoked object with wrong arity!");
				auto lambdaCallTag = NomLambdaCallTag::GetCallTag(method->GetDirectTypeParametersCount(), method->GetArgumentCount());
				auto callTagMatch = builder->CreateICmpEQ(builder->CreatePtrToInt(callTag, numtype(intptr_t)), ConstantExpr::getPtrToInt(lambdaCallTag->GetLLVMElement(mod), numtype(intptr_t)), "callTagMatch");
				builder->CreateIntrinsic(Intrinsic::expect, { inttype(1) }, { callTagMatch, MakeUInt(1,1) });
				builder->CreateCondBr(callTagMatch, callBlock, failBlock, GetLikelyFirstBranchMetadata());

				builder->SetInsertPoint(callBlock);
				auto implFunctionType = method->GetLLVMFunctionType();
				auto paramCount = implFunctionType->getNumParams();
				auto argsarr = makealloca(Value*, paramCount);

				NomSubstitutionContextMemberContext nscmc(method);
				CastedValueCompileEnv cvce = CastedValueCompileEnv(method->GetDirectTypeParameters(), this->GetAllTypeParameters(), fun, 2, paramCount, ObjectHeader::GeneratePointerToTypeArguments(builder, varargs[0]));
				for (decltype(paramCount) j = 0; j < paramCount; j++)
				{
					Value* curArg = nullptr;
					if (j < RTConfig_NumberOfVarargsArguments || RTConfig_NumberOfVarargsArguments + 1 == paramCount)
					{
						curArg = varargs[j];
					}
					else
					{
						curArg = MakeInvariantLoad(builder, builder->CreateGEP(varargs[RTConfig_NumberOfVarargsArguments], MakeInt32(j - RTConfig_NumberOfVarargsArguments)), "varArg", AtomicOrdering::NotAtomic);
					}
					auto expectedType = implFunctionType->getParamType(j);
					if (j >= method->GetDirectTypeParametersCount())
					{
						curArg = EnsurePackedUnpacked(builder, curArg, REFTYPE);
						if (j > method->GetDirectTypeParametersCount())
						{
							curArg = RTCast::GenerateCast(builder, &cvce, curArg, method->GetArgumentTypes(&nscmc)[j - (method->GetDirectTypeParametersCount() + 1)]);
						}
					}
					curArg = EnsurePackedUnpacked(builder, curArg, expectedType);
					argsarr[j] = curArg;
				}
				auto callResult = builder->CreateCall(implFunctionType, method->GetLLVMElement(mod), ArrayRef<Value*>(argsarr, paramCount), method->GetQName());
				callResult->setCallingConv(NOMCC);
				auto actualResult = EnsurePackedUnpacked(builder, EnsurePackedUnpacked(builder, callResult, REFTYPE), POINTERTYPE);
				builder->CreateRet(actualResult);

			}

			llvm::raw_os_ostream out(std::cout);
			if (verifyFunction(*fun, &out))
			{
				out.flush();
				std::cout << "Could not verify class raw-invoke function";
				std::cout << this->GetName()->ToStdString();
				fun->print(out);
				out.flush();
				std::cout.flush();
				throw new std::exception();
			}

			return fun;
		}


		llvm::Function* NomClass::GetDynamicFieldLookup(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const
		{
			auto thisType = this->GetType(GetAllTypeVariables());
			std::string ddlname = "NOM_RT_DFL_" + *this->GetSymbolName();
			llvm::Function* fun = mod.getFunction(ddlname.data());
			if (fun == nullptr)
			{
				llvm::FunctionType* funtype = GetFieldReadFunctionType();
				fun = llvm::Function::Create(funtype, linkage, ddlname.data(), &mod);
				fun->setCallingConv(NOMCC);

				auto argiter = fun->arg_begin();
				llvm::Argument* namearg = argiter;
				argiter++;
				llvm::Argument* thisarg = argiter;

				NomBuilder builder;

				BasicBlock* start = BasicBlock::Create(LLVMCONTEXT, "", fun);
				BasicBlock* notfound = BasicBlock::Create(LLVMCONTEXT, "notFound", fun);

				builder->SetInsertPoint(start);
				auto nameSwitch = builder->CreateSwitch(namearg, notfound, this->AllFields.size());

				SimpleClassCompileEnv scce = SimpleClassCompileEnv(fun, this, nullarray(NomTypeParameterRef), TypeList({ NomIntClass::GetInstance()->GetType() }), thisType);
				scce[0] = thisarg;

				for (auto field : AllFields)
				{
					std::string fieldName = field->GetName()->ToStdString();
					BasicBlock* fieldBlock = BasicBlock::Create(LLVMCONTEXT, "field:" + fieldName, fun);
					nameSwitch->addCase(MakeInt<size_t>(NomNameRepository::Instance().GetNameID(fieldName)), fieldBlock);
					builder->SetInsertPoint(fieldBlock);
					builder->CreateRet(EnsurePacked(builder, field->GenerateRead(builder, &scce, NomValue(thisarg, thisType))));
				}
				builder->SetInsertPoint(notfound);
				static const char* lookupfailstr = "Could not find any fields with matching name!";
				CreateDummyReturn(builder, fun);
				llvm::raw_os_ostream out(std::cout);
				if (verifyFunction(*fun, &out))
				{
					out.flush();
					std::cout << "Could not verify field lookup for class ";
					std::cout << this->GetName()->ToStdString();
					fun->print(out);
					out.flush();
					std::cout.flush();
					throw new std::exception();
				}
			}
			return fun;
		}


		llvm::Function* NomClass::GetDynamicFieldStore(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const
		{
			auto thisType = this->GetType(GetAllTypeVariables());
			std::string ddlname = "NOM_RT_DFS_" + *this->GetSymbolName();
			llvm::Function* fun = mod.getFunction(ddlname.data());
			if (fun == nullptr)
			{
				llvm::FunctionType* funtype = GetFieldWriteFunctionType();
				fun = llvm::Function::Create(funtype, linkage, ddlname.data(), &mod);
				fun->setCallingConv(NOMCC);

				auto argiter = fun->arg_begin();
				llvm::Argument* namearg = argiter;
				argiter++;
				llvm::Argument* thisarg = argiter;
				argiter++;
				llvm::Value* newValue = argiter;

				NomBuilder builder;

				BasicBlock* start = BasicBlock::Create(LLVMCONTEXT, "", fun);
				BasicBlock* notfound = BasicBlock::Create(LLVMCONTEXT, "notFound", fun);
				BasicBlock* errorBlock = BasicBlock::Create(LLVMCONTEXT, "invalidWriteValue", fun);

				builder->SetInsertPoint(start);
				auto nameSwitch = builder->CreateSwitch(namearg, notfound, this->AllFields.size());

				SimpleClassCompileEnv scce = SimpleClassCompileEnv(fun, this, nullarray(NomTypeParameterRef), TypeList({ NomIntClass::GetInstance()->GetType(), &NomDynamicType::Instance() }), thisType);
				scce[0] = thisarg;

				for (auto field : AllFields)
				{
					std::string fieldName = field->GetName()->ToStdString();
					BasicBlock* fieldBlock = BasicBlock::Create(LLVMCONTEXT, "field:" + fieldName, fun);
					BasicBlock* fieldWriteBlock = BasicBlock::Create(LLVMCONTEXT, "fieldWrite:" + fieldName, fun);
					nameSwitch->addCase(MakeInt<size_t>(NomNameRepository::Instance().GetNameID(fieldName)), fieldBlock);
					builder->SetInsertPoint(fieldBlock);
					/*auto castResult =*/ RTCast::GenerateCast(builder, &scce, newValue, field->GetType());
					//builder->CreateIntrinsic(Intrinsic::expect, { inttype(1) }, { castResult, MakeUInt(1,1) });
					//builder->CreateCondBr(castResult, fieldWriteBlock, errorBlock, GetLikelyFirstBranchMetadata());
					builder->CreateBr(fieldWriteBlock);

					builder->SetInsertPoint(fieldWriteBlock);
					auto writeValue = newValue;
					if (field->GetType()->IsSubtype(NomIntClass::GetInstance()->GetType(), false))
					{
						if (!writeValue->getType()->isIntegerTy())
						{
							if (writeValue->getType() != REFTYPE)
							{
								throw new std::exception();
							}
							writeValue = UnpackInt(builder, writeValue);
						}
					}
					else if (field->GetType()->IsSubtype(NomFloatClass::GetInstance()->GetType(), false))
					{
						if (!writeValue->getType()->isFloatingPointTy())
						{
							if (writeValue->getType() != REFTYPE)
							{
								throw new std::exception();
							}
							writeValue = UnpackFloat(builder, writeValue);
						}
					}
					else
					{
						writeValue = EnsurePacked(builder, writeValue);
					}
					field->GenerateWrite(builder, &scce, NomValue(thisarg, thisType), NomValue(writeValue, field->GetType()));
					builder->CreateRetVoid();
				}
				builder->SetInsertPoint(notfound);
				static const char* lookupfailstr = "Could not find any fields with matching name!";
				CreateDummyReturn(builder, fun);

				builder->SetInsertPoint(errorBlock);
				static const char* generic_errorMessage = "Trying to write invalid value!";
				builder->CreateCall(RTOutput_Fail::GetLLVMElement(*fun->getParent()), GetLLVMPointer(generic_errorMessage))->setCallingConv(RTOutput_Fail::GetLLVMElement(*fun->getParent())->getCallingConv());
				CreateDummyReturn(builder, fun);
				llvm::raw_os_ostream out(std::cout);
				if (verifyFunction(*fun, &out))
				{
					out.flush();
					std::cout << "Could not verify field store fun for class ";
					std::cout << this->GetName()->ToStdString();
					fun->print(out);
					out.flush();
					std::cout.flush();
					throw new std::exception();
				}
			}
			return fun;
		}

		llvm::FunctionType* NomClass::GetInterfaceTableLookupType()
		{
			return GetIMTFunctionType();
		}

		llvm::Constant* NomClass::GetInterfaceDescriptor(llvm::Module& mod) const
		{
			return RTClass::GetInterfaceReference(GetLLVMElement(mod));
		}

		llvm::Constant* NomClass::GetCastFunction(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const
		{
			return ConstantPointerNull::get(GetCastFunctionType()->getPointerTo());
		}

		llvm::Constant* NomClass::GetDynamicDispatcherLookup(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage, llvm::StructType* stype) const
		{
			auto thisType = this->GetType(GetAllTypeVariables());
			auto rettype = arrtype(GetDynamicDispatchListEntryType()->getPointerTo(), IMTsize);
			auto retarr = makealloca(Constant*, IMTsize);
			auto typesarr = makealloca(Type*, IMTsize);

			for (decltype(IMTsize) i = 0; i < IMTsize; i++)
			{
				llvm::SmallVector<const NomMethod*, 16> methods;
				llvm::SmallVector<const NomTypedField*, 16> fields;
				for (auto& mte : MethodTable)
				{
					if (NomNameRepository::Instance().GetNameID(mte->Method->GetName()) % IMTsize == i)
					{
						bool found = false;
						for (auto& meth : methods)
						{
							if (meth->GetName()._Equal(mte->Method->GetName()))
							{
								found = true;
								break;
							}
						}
						if (!found)
						{
							methods.push_back(mte->Method);
						}
					}
				}

				for (auto field : AllFields)
				{
					if (field->GetType()->GetLLVMType() == REFTYPE && NomNameRepository::Instance().GetNameID(field->GetName()->ToStdString()) % IMTsize == i)
					{
						fields.push_back(field);
					}
				}

				auto entrycount = methods.size() + fields.size() + 1;
				auto entries = makealloca(Constant*, entrycount);

				int entryID = 0;
				for (auto& meth : methods)
				{
					auto fun = Function::Create(GetIMTFunctionType(), linkage, "NOMMON_RT_DD_" + *meth->GetSymbolName(), mod);
					fun->setCallingConv(NOMCC);

					NomBuilder builder;

					BasicBlock* startBlock = BasicBlock::Create(LLVMCONTEXT, "", fun);
					BasicBlock* doCallBlock = BasicBlock::Create(LLVMCONTEXT, "callOK", fun);
					std::string* errormsg = new std::string("Called method " + meth->GetName() + " with invalid number of arguments!");
					std::string* typeerrormsg = new std::string("Invalid argument arguments!");
					builder->SetInsertPoint(startBlock);
					BasicBlock* wrongArgumentCountBlock = RTOutput_Fail::GenerateFailOutputBlock(builder, errormsg->c_str());
					BasicBlock* invalidArgumentBlock = RTOutput_Fail::GenerateFailOutputBlock(builder, typeerrormsg->c_str());

					auto argiter = fun->arg_begin();
					auto callTag = argiter;
					argiter++;
					auto varargs = makealloca(Value*, RTConfig_NumberOfVarargsArguments + 1);
					for (decltype(RTConfig_NumberOfVarargsArguments) i = 0; i <= RTConfig_NumberOfVarargsArguments; i++)
					{
						varargs[i] = argiter;
						argiter++;
					}

					auto callTagMatch = CreatePointerEq(builder, callTag, NomLambdaCallTag::GetCallTag(meth->GetDirectTypeParametersCount(), meth->GetArgumentCount())->GetLLVMElement(mod));
					builder->CreateIntrinsic(Intrinsic::expect, { inttype(1) }, { callTagMatch, MakeUInt(1,1) });
					builder->CreateCondBr(callTagMatch, doCallBlock, wrongArgumentCountBlock, GetLikelyFirstBranchMetadata());

					builder->SetInsertPoint(doCallBlock);
					auto implFunctionType = meth->GetLLVMFunctionType();
					auto paramCount = implFunctionType->getNumParams();
					auto argsarr = makealloca(Value*, paramCount);

					NomSubstitutionContextMemberContext nscmc(meth);
					CastedValueCompileEnv cvce = CastedValueCompileEnv(meth->GetDirectTypeParameters(), this->GetAllTypeParameters(), fun, 2, paramCount, ObjectHeader::GeneratePointerToTypeArguments(builder, varargs[0]));
					for (decltype(paramCount) j = 0; j < paramCount; j++)
					{
						Value* curArg = nullptr;
						if (j < RTConfig_NumberOfVarargsArguments || RTConfig_NumberOfVarargsArguments + 1 == paramCount)
						{
							curArg = varargs[j];
						}
						else
						{
							curArg = MakeInvariantLoad(builder, builder->CreateGEP(varargs[RTConfig_NumberOfVarargsArguments], MakeInt32(j - RTConfig_NumberOfVarargsArguments)), "varArg", AtomicOrdering::NotAtomic);
						}
						auto expectedType = implFunctionType->getParamType(j);
						if (j >= meth->GetDirectTypeParametersCount())
						{
							curArg = EnsurePackedUnpacked(builder, curArg, REFTYPE);
							if (j > meth->GetDirectTypeParametersCount())
							{
								curArg = RTCast::GenerateCast(builder, &cvce, curArg, meth->GetArgumentTypes(&nscmc)[j - (meth->GetDirectTypeParametersCount() + 1)]);
							}
						}
						curArg = EnsurePackedUnpacked(builder, curArg, expectedType);
						argsarr[j] = curArg;
					}
					auto callResult = builder->CreateCall(implFunctionType, meth->GetLLVMElement(mod), ArrayRef<Value*>(argsarr, paramCount), meth->GetQName());
					callResult->setCallingConv(NOMCC);
					auto actualResult = EnsurePackedUnpacked(builder, EnsurePackedUnpacked(builder, callResult, REFTYPE), POINTERTYPE);
					builder->CreateRet(actualResult);


					llvm::raw_os_ostream out(std::cout);
					if (verifyFunction(*fun, &out))
					{
						out.flush();
						std::cout << "Could not verify dynamic dispatch function";
						std::cout << meth->GetName();
						fun->print(out);
						out.flush();
						std::cout.flush();
						throw new std::exception();
					}

					entries[entryID] = GetDynamicDispatchListEntryConstant(MakeInt<size_t>(NomNameRepository::Instance().GetNameID(meth->GetName())), MakeInt<size_t>(0), fun);
					entryID++;
				}
				for (auto& field : fields)
				{
					entries[entryID] = GetDynamicDispatchListEntryConstant(MakeInt<size_t>(NomNameRepository::Instance().GetNameID(field->GetName()->ToStdString())), MakeInt<size_t>(1), ConstantExpr::getIntToPtr(MakeInt<size_t>(((size_t)(field->Index + (GetHasRawInvoke() ? 1 : 0))) << 32), GetIMTFunctionType()->getPointerTo()));
					entryID++;
				}
				entries[entryID] = GetDynamicDispatchListEntryConstant(MakeInt<size_t>(0), MakeInt<size_t>(0), ConstantPointerNull::get(GetIMTFunctionType()->getPointerTo()));
				retarr[i] = ConstantArray::get(arrtype(GetDynamicDispatchListEntryType(), entryID + 1), ArrayRef<Constant*>(entries, entryID + 1));
				typesarr[i] = retarr[i]->getType();
			}
			if (stype->isOpaque())
			{
				stype->setBody(ArrayRef<Type*>(typesarr, IMTsize), false);
			}
			return ConstantStruct::get(stype, ArrayRef<Constant*>(retarr, IMTsize));
		}
		llvm::Constant* NomClass::GetInterfaceTableLookup(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const
		{
			auto rettype = arrtype(GetIMTFunctionType()->getPointerTo(), IMTsize);
			auto retarr = makealloca(Constant*, IMTsize);

			llvm::SmallPtrSet<const NomInterface*, 16> supers;
			vector<const NomInterface*> worklist;
			auto curClass = this;
			while (curClass != nullptr)
			{
				for (auto& super : curClass->GetSuperInterfaces())
				{
					if (supers.insert(super.Elem).second)
					{
						worklist.push_back(super.Elem);
					}
				}
				curClass = curClass->GetSuperClass().Elem;
			}
			while (worklist.size() > 0)
			{
				auto current = worklist.back();
				worklist.pop_back();
				for (auto& super : current->GetSuperInterfaces())
				{
					if (supers.insert(super.Elem).second)
					{
						worklist.push_back(super.Elem);
					}
				}
			}

			for (decltype(IMTsize) i = 0; i < IMTsize; i++)
			{
				llvm::SmallVector<const NomMethod*, 16> methods;
				for (auto super : supers)
				{
					for (auto& meth : super->Methods)
					{
						if (meth->GetContainer() == super && meth->GetIMTIndex() == i)
						{
							methods.push_back(meth);
						}
					}
				}

				auto fun = Function::Create(GetIMTFunctionType(), linkage, "MONNOM_RT_IMT_" + *GetSymbolName() + "_" + std::to_string(i), mod);
				fun->setCallingConv(NOMCC);
				BasicBlock* startBlock = BasicBlock::Create(LLVMCONTEXT, "", fun);
				NomBuilder builder;
				builder->SetInsertPoint(startBlock);

				auto argiter = fun->arg_begin();
				auto callTag = argiter;
				argiter++;
				auto varargs = makealloca(Value*, RTConfig_NumberOfVarargsArguments + 1);
				for (decltype(RTConfig_NumberOfVarargsArguments) i = 0; i <= RTConfig_NumberOfVarargsArguments; i++)
				{
					varargs[i] = argiter;
					argiter++;
				}

				for (auto& meth : methods)
				{
					BasicBlock* nextBlock = builder->GetInsertBlock();
					if (methods.size() > 1 || RTConfig_RunUnncessesaryCallTagChecks)
					{
						BasicBlock* callBlock = BasicBlock::Create(LLVMCONTEXT, meth->GetName(), fun);
						nextBlock = BasicBlock::Create(LLVMCONTEXT, "next", fun);

						auto methodCallTag = NomInterfaceCallTag::GetMethodKey(meth);

						auto callTagMatch = builder->CreateICmpEQ(builder->CreatePtrToInt(callTag, numtype(intptr_t)), ConstantExpr::getPtrToInt(methodCallTag->GetLLVMElement(mod), numtype(intptr_t)), "callTagMatch");
						builder->CreateIntrinsic(Intrinsic::expect, { inttype(1) }, { callTagMatch, MakeUInt(1,1) });
						builder->CreateCondBr(callTagMatch, callBlock, nextBlock, GetLikelyFirstBranchMetadata());
						builder->SetInsertPoint(callBlock);
					}

					bool found = false;
					for (auto& mte : MethodTable)
					{
						if (mte->Method->Overrides(meth))
						{
							auto calledFunctionType = meth->GetLLVMFunctionType();
							auto implFunctionType = mte->Method->GetLLVMFunctionType();
							auto paramCount = implFunctionType->getNumParams();
							auto argsarr = makealloca(Value*, paramCount);

							for (decltype(paramCount) j = 0; j < paramCount; j++)
							{
								Value* curArg = nullptr;
								if (j < RTConfig_NumberOfVarargsArguments || RTConfig_NumberOfVarargsArguments + 1 == paramCount)
								{
									curArg = varargs[j];
								}
								else
								{
									curArg = MakeInvariantLoad(builder, builder->CreateGEP(varargs[RTConfig_NumberOfVarargsArguments], MakeInt32(j - RTConfig_NumberOfVarargsArguments)), "varArg", AtomicOrdering::NotAtomic);
								}
								auto calledType = calledFunctionType->getParamType(j);
								auto expectedType = implFunctionType->getParamType(j);
								curArg = EnsurePackedUnpacked(builder, curArg, calledType);
								curArg = EnsurePackedUnpacked(builder, curArg, expectedType);
								argsarr[j] = curArg;
							}
							auto callResult = builder->CreateCall(implFunctionType, mte->Method->GetLLVMElement(mod), ArrayRef<Value*>(argsarr, paramCount), mte->Method->GetQName());
							callResult->setCallingConv(NOMCC);
							auto actualResult = EnsurePackedUnpacked(builder, EnsurePackedUnpacked(builder, callResult, calledFunctionType->getReturnType()), POINTERTYPE);
							builder->CreateRet(actualResult);
							found = true;
							break;
						}
					}
					if (!found)
					{
						throw new std::exception();
					}

					builder->SetInsertPoint(nextBlock);
				}
				if (methods.size() != 1 && !RTConfig_RunUnncessesaryCallTagChecks)
				{
					RTOutput_Fail::MakeBlockFailOutputBlock(builder, "RUNTIME ERROR: Called invalid IMT method!", builder->GetInsertBlock());
				}

				llvm::raw_os_ostream out(std::cout);
				if (verifyFunction(*fun, &out))
				{
					out.flush();
					std::cout << "Could not verify class IMT function";
					std::cout << this->GetName()->ToStdString();
					fun->print(out);
					out.flush();
					std::cout.flush();
					throw new std::exception();
				}

				retarr[i] = fun;
			}

			return ConstantArray::get(rettype, ArrayRef<Constant*>(retarr, IMTsize));
		}


		const NomInstantiationRef<NomClass> NomClassLoaded::GetSuperClass(const NomSubstitutionContext* context) const
		{
			if (context != nullptr && context->GetTypeArgumentCount() > 0)
			{
				return NomConstants::GetSuperClass(superClass)->GetClassType(context);
			}
			if (!superClassRef.HasElem() && superClass != 0)
			{
				NomSubstitutionContextMemberContext nscmc(this);
				superClassRef = NomConstants::GetSuperClass(superClass)->GetClassType(&nscmc);
			}
			return superClassRef;
		}

		const llvm::SmallVector<NomClassTypeRef, 16> NomClass::GetSuperNameds(llvm::ArrayRef<NomTypeRef> args) const
		{
			auto ret = NomInterface::GetSuperNameds(args);
			auto sc = GetSuperClass();
			if (sc.HasElem())
			{
				auto targcount = sc.TypeArgs.size();
				NomTypeRef* targs = makealloca(NomTypeRef, targcount /** 2*/);
				for (size_t i = 0; i < targcount; i++)
				{
					targs[i /** 2*/] = sc.TypeArgs[i];
					//targs[(i * 2) + 1] = sc.TypeArgs[i];
				}
				ret.push_back(sc.Elem->GetType(llvm::ArrayRef<NomTypeRef>(targs, targcount /** 2*/)));
			}
			return ret;
		}

		const NomField* NomClass::GetField(NomStringRef name) const
		{
			for (auto field : Fields)
			{
				if (NomStringEquality()(field->GetName(), name))
				{
					return field;
				}
			}
			auto sc = GetSuperClass();
			if (sc.HasElem())
			{
				return sc.Elem->GetField(name);
			}
			return NomDictField::GetInstance(name);
		}

		size_t NomClass::GetFieldCount() const
		{
			size_t ret = Fields.size();
			auto sc = GetSuperClass();
			if (sc.HasElem())
			{
				ret += sc.Elem->GetFieldCount();
			}
			return ret;
		}

		void NomClass::PreprocessInheritance() const
		{
			if (preprocessed == true)
			{
				return;
			}
			preprocessed = true;
			for (auto meth : Methods)
			{
				AllMethods.push_back(meth);
			}
			auto superinsts = GetSuperInterfaces();
			for (int sii = 0; sii < superinsts.size(); sii++)
			{
				const NomInstantiationRef<NomInterface>& super = superinsts[sii];
				super.Elem->PreprocessInheritance();
				for (auto& inst : super.Elem->GetInstantiations())
				{
					AddInstantiation(NomInstantiationRef<NomInterface>(inst.first, inst.second));
				}
				AddInstantiation(super);
				for (auto meth : super.Elem->AllMethods)
				{
					bool found = false;
					for (auto cmeth : AllMethods)
					{
						if (cmeth->Overrides(meth))
						{
							found = true;
							break;
						}
					}
					if (!found)
					{
						AllMethods.push_back(meth);
					}
				}
			}
			auto superClassRef = GetSuperClass();
			for (auto& sinst : superClassRef.Elem->GetInstantiations())
			{
				AddInstantiation(NomInstantiationRef<NomInterface>(sinst.first, sinst.second));
			}
			AddInstantiation(NomInstantiationRef<NomInterface>(superClassRef.Elem, superClassRef.TypeArgs));
			const NomClass* sclass = superClassRef.Elem;
			sclass->PreprocessInheritance();
			int offset = -sclass->MethodTable.size() - 1;
			bool found = false;
			auto mthis = const_cast<NomClass*>(this);

			int fieldIndex = sclass->GetFieldCount();
			for (auto field : Fields)
			{
				field->SetIndex(fieldIndex++);
				AllFields.push_back(field);
			}
			for (auto sfield : sclass->AllFields)
			{
				AllFields.push_back(sfield);
			}
			for (auto smeth : sclass->AllMethods)
			{
				bool found = false;
				for (auto cmeth : AllMethods)
				{
					if (cmeth->Overrides(smeth))
					{
						found = true;
						break;
					}
				}
				if (!found)
				{
					AllMethods.push_back(smeth);
				}
			}

			mthis->MethodTable = sclass->MethodTable;
			for (auto meth : mthis->Methods)
			{
				for (size_t i = 0, s = MethodTable.size(); i != s; i++)
				{
					if (meth->Overrides(MethodTable[i]->Method))
					{
						mthis->MethodTable[i] = new NomMethodTableEntry(meth, MethodTable[i]->CallableVersion->FunType, MethodTable[i]->Offset);
						if (meth->GetLLVMFunctionType() == MethodTable[i]->CallableVersion->FunType)
						{
							meth->SetOffset(MethodTable[i]->Offset);
						}
					}
				}
			}
			for (auto meth : mthis->Methods)
			{
				if (!meth->IsOffsetSet())
				{
					meth->SetOffset(mthis->MethodTable.size());
					mthis->MethodTable.push_back(new NomMethodTableEntry(meth, meth->GetLLVMFunctionType(), meth->GetOffset()));
				}
			}
			auto supers = GetSuperInterfaces();
			for (auto super : supers)
			{
				super.Elem->PreprocessInheritance();
			}
			auto supercount = supers.size();
			llvm::Constant** superEntries = makealloca(llvm::Constant*, supercount);
			for (auto super : supers)
			{
				NomSubstitutionContextList nscl(super.TypeArgs);
				for (size_t mts = mthis->MethodTable.size(), itoffset = 0; itoffset <= mts; itoffset++)
				{
					bool match = true;
					for (size_t i = 0, smts = super.Elem->MethodTable.size(); i != smts; i++)
					{
						auto superSig = super.Elem->MethodTable[i]->Method->Signature(&nscl);
						if (itoffset + i < mts)
						{
							if (!MethodTable[itoffset + i]->PerfectOverrideMatch(super.Elem->MethodTable[i], super.TypeArgs))
							{
								match = false;
								break;
							}
						}
						else
						{
							bool found = false;
							for (auto meth : MethodTable)
							{
								auto methSig = meth->Method->Signature();
								if (meth->Method->GetName() == super.Elem->MethodTable[i]->Method->GetName())
								{
									if (meth->Method->GetName() == "GetValue")
									{
										found = false;
									}
									if (methSig.Satisfies(superSig, false))
									{
										found = true;
										mthis->MethodTable.push_back(new NomMethodTableEntry(meth->Method, super.Elem->MethodTable[i]->CallableVersion->FunType, mthis->MethodTable.size()));
										break;
									}
									else
									{
										cout << meth->Method->GetName();
										throw new std::exception();
									}
								}
							}
							if (!found)
							{
								if (!found)
								{
									throw new std::exception();
								}
							}
						}
					}
					if (match)
					{
						mthis->InterfaceTableEntries[super.Elem->GetID()] = itoffset;
						break;
					}
				}
			}
		}

		NomClassInternal::NomClassInternal(NomStringRef name, const NomMemberContext* parent) : NomInterface(name->ToStdString()), NomInterfaceInternal(name, parent), symname(name->ToStdString())
		{
			RegisterClass(name, this);
		}

		void NomClassInternal::SetSuperClass(NomInstantiationRef<NomClass> superClass)
		{
			if (superClass.Elem == nullptr || this->superClass.Elem != nullptr)
			{
				throw new std::exception();
			}
			this->superClass = superClass;
		}

		void NomClassInternal::SetSuperClass()
		{
			SetSuperClass(NomInstantiationRef<NomClass>(NomObjectClass::GetInstance(), TypeList()));
		}

		const NomInstantiationRef<NomClass> NomClassInternal::GetSuperClass(const NomSubstitutionContext* context) const
		{
			if (context == nullptr || context->GetTypeArgumentCount() == 0 || superClass.TypeArgs.size() == 0)
			{
				return superClass;
			}
			size_t tcount = superClass.TypeArgs.size();
			NomTypeRef* tarr = (NomTypeRef*)gcalloc(sizeof(NomTypeRef) * tcount);
			for (size_t i = 0; i < tcount; i++)
			{
				tarr[i] = superClass.TypeArgs[i]->SubstituteSubtyping(context);
			}
			return NomInstantiationRef<NomClass>(superClass.Elem, TypeList(tarr, tcount));
		}

	}
}