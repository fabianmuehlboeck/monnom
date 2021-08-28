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
#include "RawInvoke.h"
#include "NomMethodKey.h"
#include "EnsureDynamicMethodInstruction.h"

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

		NomStruct* NomClassLoaded::AddStruct(const ConstantID structID, ConstantID closureTypeParams, RegIndex regcount, RegIndex endargregcount, ConstantID initializerArgTypes) {
			NomStruct* structure = new NomStruct(structID, nullptr, closureTypeParams, regcount, endargregcount, initializerArgTypes);
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

		llvm::GlobalVariable* NomClass::GetSuperInstances(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const
		{
			llvm::Twine t = "NOM_ST_";
			llvm::SmallVector<char, 64> buf;
			llvm::StringRef nameref = t.concat(this->GetName()->ToStdString()).toStringRef(buf);
			llvm::GlobalVariable* ret = mod.getGlobalVariable(nameref, false);
			if (ret == nullptr)
			{
				ret = new llvm::GlobalVariable(mod, GetSuperInstancesType(false), true, linkage, nullptr, nameref);
				llvm::Constant** entries = makealloca(llvm::Constant*, GetInstantiations().size());
				size_t entrynum = 0;
				for (auto &instantiation : GetInstantiations())
				{
					size_t instasize = instantiation.second.size();
					llvm::GlobalVariable* instaArgs = new GlobalVariable(mod, arrtype(TYPETYPE, instasize), true, linkage, nullptr);
					//auto insta = instantiation.first->GetType(instantiation.second);
					entries[entrynum] = ConstantStruct::get(SuperInstanceEntryType(), ConstantExpr::getPointerCast(instantiation.first->GetLLVMElement(mod), RTInterface::GetLLVMType()->getPointerTo()), ConstantExpr::getGetElementPtr(arrtype(TYPETYPE, instasize), instaArgs, ArrayRef<Constant*>({ MakeInt32(0), MakeInt32(instasize) }))); //insta->GetLLVMElement(mod);
					entrynum++;
					llvm::Constant** instaArgBuf = makealloca(llvm::Constant*, instasize);
					for (size_t i = 0; i < instasize; i++)
					{
						instaArgBuf[instasize-(i+1)] = instantiation.second[i]->GetLLVMElement(mod);
					}
					instaArgs->setInitializer(ConstantArray::get(arrtype(TYPETYPE, instasize), ArrayRef<Constant*>(instaArgBuf, instasize)));
				}
				ret->setInitializer(llvm::ConstantArray::get(GetSuperInstancesType(false), llvm::ArrayRef<llvm::Constant*>(entries, entrynum)));
			}
			return ret;
		}

		llvm::ArrayType* NomClass::GetSuperInstancesType(bool generic) const
		{
			return arrtype(SuperInstanceEntryType(), (generic ? 0 : GetInstantiations().size()));
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
				auto gvartype = RTClass::GetConstantType(nullptr, GetMethodTableType(false));
				GlobalVariable* gvar = new GlobalVariable(mod, gvartype, true, linkage, nullptr, nameref);
				ret = RTClass::CreateConstant( gvar, gvartype, this,
					GetDynamicFieldLookup(mod, linkage),
					GetDynamicFieldStore(mod, linkage),
					GetDynamicDispatcherLookup(mod, linkage),
					MakeInt(GetFieldCount()),
					MakeInt(this->GetTypeParametersCount()),
					MakeInt(GetInstantiations().size()),
					GetSuperInstances(mod, linkage),
					GetMethodTable(mod, linkage),
					ConstantPointerNull::get(GetCheckReturnValueFunctionType()->getPointerTo()),
					GetMethodEnsureFunction(mod, linkage),
					GetInterfaceTableLookup(mod, linkage),
					GetSignature(mod,linkage));
				new GlobalVariable(mod, ret->getType(), true, linkage, ret, "NOM_CDREF_" + this->GetName()->ToStdString());

				RegisterGlobalForAddressLookup(nameref.str());

				if (linkage == GlobalValue::LinkageTypes::ExternalLinkage)
				{
					GenerateDictionaryEntries(mod);
				}
				if (GetHasRawInvoke())
				{
					GetRawInvokeFunction(mod, linkage);
				}
			}
			return ret;
		}

		llvm::Function* NomClass::GetMethodEnsureFunction(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const
		{
			Function* fun = Function::Create(GetMethodEnsureFunctionType(), linkage, "MONNOM_RT_METHODENSURE_" + this->GetName()->ToStdString(), mod);
			NomBuilder builder;

			auto args = fun->arg_begin();
			Value* receiver = args;
			args++;
			Value* methodName = args;

			BasicBlock* startBlock = BasicBlock::Create(LLVMCONTEXT, "start", fun);
			BasicBlock* falseBlock = BasicBlock::Create(LLVMCONTEXT, "methodDoesNotExist", fun);
			builder->SetInsertPoint(startBlock);

			if (this->Methods.size() > 0)
			{
				BasicBlock* trueBlock = BasicBlock::Create(LLVMCONTEXT, "methodExists", fun);
				auto nameSwitch = builder->CreateSwitch(methodName, falseBlock, this->Methods.size());
				for (auto& meth : this->Methods)
				{
					nameSwitch->addCase(MakeIntLike(methodName,NomNameRepository::Instance().GetNameID(meth->GetName())), trueBlock);
				}
				builder->SetInsertPoint(trueBlock);
				builder->CreateRet(MakeUInt(1, 1));
			}
			else
			{
				builder->CreateBr(falseBlock);
			}

			builder->SetInsertPoint(falseBlock);
			builder->CreateRet(MakeUInt(1, 0));

			return fun;
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

		size_t NomClass::GenerateTypeArgInitialization(NomBuilder& builder, CompileEnv* env, llvm::Value* newObj, TypeList args) const
		{
			size_t offset = 0;
			auto sclass = GetSuperClass();
			NomSubstitutionContextList nscl(args);
			if (sclass.HasElem())
			{
				auto instantiation = sclass;
				const NomType** types = makealloca(const NomType*, instantiation.TypeArgs.size());
				int i = 0;
				for (auto targ : instantiation.TypeArgs)
				{
					types[i++] = targ->SubstituteSubtyping(&nscl);
				}
				offset = instantiation.Elem->GenerateTypeArgInitialization(builder, env, newObj, TypeList(types, instantiation.TypeArgs.size()));
			}
			for (NomTypeRef type : args)
			{
				ObjectHeader::GenerateWriteTypeArgument(builder, newObj, offset, type->GenerateRTInstantiation(builder, env));
				offset++;
			}
			return offset;
		}

		llvm::FunctionType* NomClass::GetExpandoReaderType()
		{
			static llvm::FunctionType* ft = FunctionType::get(REFTYPE, { REFTYPE, numtype(size_t) }, false);
			return ft;
		}

		llvm::Function* NomClass::GetExpandoReaderFunction(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const
		{
			std::string ername = "NOM_RT_ER_" + *this->GetSymbolName();
			llvm::Function* fun = mod.getFunction(ername);
			if (fun == nullptr)
			{
				fun = Function::Create(GetExpandoReaderType(), linkage, ername, &mod);
				fun->setCallingConv(NOMCC);

				llvm::Argument* thisArg = fun->arg_begin();
				llvm::Value* nameIndex = thisArg + 1;

				NomBuilder builder;

				BasicBlock* start = BasicBlock::Create(LLVMCONTEXT, "", fun);
				BasicBlock* notfound = BasicBlock::Create(LLVMCONTEXT, "", fun);

				builder->SetInsertPoint(start);
				SwitchInst* nameSwitch = builder->CreateSwitch(nameIndex, notfound, this->Fields.size());

				builder->SetInsertPoint(notfound);
				static const char* msg = "Could not find field!";
				builder->CreateRet(builder->CreateCall(RTOutput_Fail::GetLLVMElement(mod), { GetLLVMPointer(msg) }));

				for (auto field : Fields)
				{
					BasicBlock* fieldBlock = BasicBlock::Create(LLVMCONTEXT, "", fun);

					builder->SetInsertPoint(fieldBlock);
					auto load = MakeLoad(builder, mod, builder->CreateGEP(thisArg, { MakeInt32(0), MakeInt32((unsigned char)ObjectHeaderFields::Fields), MakeInt32((-(field->Index)) - 1) }));
					builder->CreateRet(load);
					nameSwitch->addCase(MakeInt(NomNameRepository::Instance().GetNameID(field->GetName()->ToStdString())), fieldBlock);
				}
				llvm::raw_os_ostream out(std::cout);
				if (verifyFunction(*fun, &out))
				{
					out.flush();
					std::cout << "Could not verify " << ername << "!";
					fun->print(out);
					out.flush();
					std::cout.flush();
					throw new std::exception();
				}

				return fun;
			}
			return fun;
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


			std::string name = "RT_NOM_RAWINVOKE_" + this->GetName()->ToStdString();
			FunctionType* mainFunType = method->GetRawInvokeLLVMFunctionType(nullptr);
			llvm::Function* fun = Function::Create(mainFunType, linkage, name, mod);
			fun->setCallingConv(NOMCC);
			BasicBlock* startBlock = BasicBlock::Create(LLVMCONTEXT, "start", fun);

			NomBuilder builder;
			builder->SetInsertPoint(startBlock);

			auto argcount = mainFunType->getNumParams();
			Value** argarr = makealloca(Value*, argcount);
			auto args = fun->arg_begin();
			//Argument* iidArg = args;

			for (decltype(argcount) i = 0; i < argcount; i++)
			{
				argarr[i] = args;
				args++;
			}

			GenerateRawInvoke(builder, this, method, 
				[](NomBuilder& b, const NomMethod* meth, llvm::ArrayRef<llvm::Value*> cargs) 
				{
					auto call = b->CreateCall(meth->GetLLVMElement(*b->GetInsertBlock()->getParent()->getParent()), cargs);
					call->setCallingConv(NOMCC);
					return call;
				}, 
				llvm::ArrayRef<llvm::Value*>(argarr, argcount));


			llvm::raw_os_ostream out(std::cout);
			if (verifyFunction(*fun, &out))
			{
				std::cout << "Could not verify raw-invoke method!";
				fun->print(out);
				out.flush();
				std::cout.flush();
				throw name;
			}
			return fun;
		}



		void NomClass::GenerateDictionaryEntries(llvm::Module& mod) const
		{
			llvm::GlobalVariable* gvar = mod.getGlobalVariable(dictionary->SymbolName);
			auto targcount = this->GetTypeParametersCount();
			NomTypeRef* selfArgsBuf = makealloca(NomTypeRef, targcount);
			for (decltype(targcount) i = 0; i < targcount; i++)
			{
				selfArgsBuf[i] = this->GetTypeParameter(i)->GetVariable();
			}
			auto thisType = this->GetType(TypeList(selfArgsBuf, targcount));
			if (gvar == nullptr)
			{
				vector<Constant*> constants;
				for (NomTypedField* field : AllFields)
				{
					auto cnstnt = new GlobalVariable(mod, RTDescriptorDictionaryEntry::GetLLVMType(), true, GlobalValue::LinkageTypes::InternalLinkage, RTDescriptorDictionaryEntry::CreateConstant(RTDescriptorDictionaryEntryKind::Field, field->IsReadOnly(), field->GetVisibility(), field->GetType()->GetLLVMElement(mod), field->Index, field->GetType()->IsSubtype(NomIntClass::GetInstance()->GetType(), false), field->GetType()->IsSubtype(NomFloatClass::GetInstance()->GetType(), false)));
					constants.push_back(cnstnt);
					DICTKEYTYPE dictKey = NomNameRepository::Instance().GetNameID(NomConstants::GetString(field->Name)->GetText()->ToStdString());
					dictionary->AddEntryKey(dictKey);
				}

				unordered_map<size_t, vector<const NomCallable*>> overloadings;


				for (NomMethodTableEntry* mte : MethodTable)
				{
					auto namekey = NomNameRepository::Instance().GetNameID(mte->Method->GetName());
					auto match = overloadings.find(namekey);
					if (match == overloadings.end())
					{
						overloadings[namekey] = vector<const NomCallable*>();
					}
					bool found = false;
					for (auto ole : overloadings[namekey])
					{
						if (ole == mte->Method)
						{
							found = true;
						}
					}
					if (!found)
					{
						overloadings[namekey].push_back(mte->Method);
					}
				}

				for (auto& ovlpair : overloadings)
				{
					std::string symname = *this->GetSymbolName() + "$" + ovlpair.second.at(0)->GetName();
					NomPartialApplication* npa = new NomPartialApplication(symname, ovlpair.second, this, thisType);

					auto cnstnt = new GlobalVariable(mod, RTDescriptorDictionaryEntry::GetLLVMType(), true, GlobalValue::LinkageTypes::InternalLinkage, RTDescriptorDictionaryEntry::CreateConstant(RTDescriptorDictionaryEntryKind::PartialApp, true, Visibility::Public, npa->GetLLVMElement(mod), 0, false, false));
					constants.push_back(cnstnt);
					dictionary->AddEntryKey(ovlpair.first);

				}

				llvm::ArrayType* type = arrtype(RTDescriptorDictionaryEntry::GetLLVMPointerType(), constants.size());
				gvar = new llvm::GlobalVariable(mod, type, true, GlobalValue::LinkageTypes::ExternalLinkage, ConstantArray::get(type, constants), dictionary->SymbolName);
				dictionary->EnsurePreparation();
			}
		}


		llvm::FunctionType* NomClass::GetDynamicFieldLookupType()
		{
			static llvm::FunctionType* funtype = FunctionType::get(REFTYPE, { REFTYPE, numtype(size_t) }, false);
			return funtype;
		}
		llvm::Function* NomClass::GetDynamicFieldLookup(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const
		{
			auto thisType = this->GetType(GetAllTypeVariables());
			std::string ddlname = "NOM_RT_DFL_" + *this->GetSymbolName();
			llvm::Function* fun = mod.getFunction(ddlname.data());
			if (fun == nullptr)
			{
				llvm::FunctionType* funtype = GetDynamicFieldLookupType();
				fun = llvm::Function::Create(funtype, linkage, ddlname.data(), &mod);
				fun->setCallingConv(NOMCC);

				auto argiter = fun->arg_begin();
				llvm::Argument* thisarg = argiter;
				argiter++;
				llvm::Argument* namearg = argiter;

				NomBuilder builder;

				BasicBlock* start = BasicBlock::Create(LLVMCONTEXT, "", fun);
				BasicBlock* notfound = BasicBlock::Create(LLVMCONTEXT, "notFound", fun);

				builder->SetInsertPoint(start);
				auto nameSwitch = builder->CreateSwitch(namearg, notfound, this->AllFields.size());

				SimpleClassCompileEnv scce = SimpleClassCompileEnv(fun, this, nullarray(NomTypeParameterRef), TypeList({ NomIntClass::GetInstance()->GetType() }), thisType);

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

		llvm::FunctionType* NomClass::GetDynamicFieldStoreType()
		{
			static llvm::FunctionType* funtype = FunctionType::get(llvm::Type::getVoidTy(LLVMCONTEXT), { REFTYPE, numtype(size_t), REFTYPE }, false);
			return funtype;
		}

		llvm::Function* NomClass::GetDynamicFieldStore(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const
		{
			auto thisType = this->GetType(GetAllTypeVariables());
			std::string ddlname = "NOM_RT_DFS_" + *this->GetSymbolName();
			llvm::Function* fun = mod.getFunction(ddlname.data());
			if (fun == nullptr)
			{
				llvm::FunctionType* funtype = GetDynamicFieldStoreType();
				fun = llvm::Function::Create(funtype, linkage, ddlname.data(), &mod);
				fun->setCallingConv(NOMCC);

				auto argiter = fun->arg_begin();
				llvm::Argument* thisarg = argiter;
				argiter++;
				llvm::Argument* namearg = argiter;
				argiter++;
				llvm::Value* newValue = argiter;

				NomBuilder builder;

				BasicBlock* start = BasicBlock::Create(LLVMCONTEXT, "", fun);
				BasicBlock* notfound = BasicBlock::Create(LLVMCONTEXT, "notFound", fun);
				BasicBlock* errorBlock = BasicBlock::Create(LLVMCONTEXT, "invalidWriteValue", fun);

				builder->SetInsertPoint(start);
				auto nameSwitch = builder->CreateSwitch(namearg, notfound, this->AllFields.size());

				SimpleClassCompileEnv scce = SimpleClassCompileEnv(fun, this, nullarray(NomTypeParameterRef), TypeList({ NomIntClass::GetInstance()->GetType(), &NomDynamicType::Instance() }), thisType);

				for (auto field : AllFields)
				{
					std::string fieldName = field->GetName()->ToStdString();
					BasicBlock* fieldBlock = BasicBlock::Create(LLVMCONTEXT, "field:" + fieldName, fun);
					BasicBlock* fieldWriteBlock = BasicBlock::Create(LLVMCONTEXT, "fieldWrite:" + fieldName, fun);
					nameSwitch->addCase(MakeInt<size_t>(NomNameRepository::Instance().GetNameID(fieldName)), fieldBlock);
					builder->SetInsertPoint(fieldBlock);
					builder->CreateCondBr(RTCast::GenerateCast(builder, &scce, newValue, field->GetType()), fieldWriteBlock, errorBlock);

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

		llvm::StructType* NomClass::GetDynamicDispatcherLookupResultType()
		{
			static llvm::StructType* st = StructType::create(LLVMCONTEXT, { NomPartialApplication::GetDynamicDispatcherType()->getPointerTo(), REFTYPE }, "DynamicDispatcherResultPair");
			return st;
		}

		llvm::FunctionType* NomClass::GetDynamicDispatcherLookupType()
		{
			static llvm::FunctionType* funtype = FunctionType::get(GetDynamicDispatcherLookupResultType(), { REFTYPE, numtype(size_t)/*, numtype(int32_t), numtype(int32_t)*/ }, false);
			return funtype;
		}

		llvm::FunctionType* NomClass::GetInterfaceTableLookupType()
		{
			return GetIMTFunctionType();
		}
		
		llvm::Constant* NomClass::GetInterfaceTableLookup(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const
		{
			Constant** imtFuns = makealloca(Constant*, IMTsize);
			SmallVector<pair<NomMethodKey*, Function*>, 8> imtPairs;

			for (int i = 0; i < IMTsize; i++)
			{
				imtPairs.clear();
				for (auto ifc : GetSuperInterfaces())
				{
					for (auto meth : ifc.Elem->Methods)
					{
						if (meth->GetIMTIndex() == i)
						{
							NomMethodKey* nmk = NomMethodKey::GetMethodKey(meth);
							bool found = false;
							for (auto& pair : imtPairs)
							{
								if (pair.first == nmk)
								{
									found = true;
									break;
								}
							}
							if (!found)
							{
								imtPairs.emplace_back(nmk, MethodTable[InterfaceTableEntries.at(ifc.Elem->GetID()) + meth->GetOffset()]->CallableVersion->GetLLVMElement(mod));
							}
						}
					}
				}

				std::string ddlname = "NOM_RT_ITL_" + *this->GetSymbolName() + '_' + std::to_string(i);
				llvm::Function* fun = mod.getFunction(ddlname.data());
				if (fun == nullptr)
				{
					fun = GenerateIMT(&mod, linkage, ddlname, imtPairs);
				}
				imtFuns[i] = fun;
			}

			return llvm::ConstantArray::get(arrtype(GetIMTFunctionType()->getPointerTo(), IMTsize), ArrayRef<Constant*>(imtFuns, IMTsize));

		}

		llvm::Function* NomClass::GetDynamicDispatcherLookup(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const
		{
			auto thisType = this->GetType(GetAllTypeVariables());
			std::string ddlname = "NOM_RT_DD_" + *this->GetSymbolName();
			llvm::Function* fun = mod.getFunction(ddlname.data());
			if (fun == nullptr)
			{
				llvm::FunctionType* funtype = GetDynamicDispatcherLookupType();
				fun = llvm::Function::Create(funtype, linkage, ddlname.data(), &mod);
				fun->setCallingConv(NOMCC);

				auto argiter = fun->arg_begin();
				llvm::Argument* thisarg = argiter;
				argiter++;
				llvm::Argument* namearg = argiter;
				//argiter++;
				//llvm::Argument* tacarg = argiter;
				//argiter++;
				//llvm::Argument* argcarg = argiter;

				NomBuilder builder;

				BasicBlock* start = BasicBlock::Create(LLVMCONTEXT, "", fun);
				BasicBlock* notfound = BasicBlock::Create(LLVMCONTEXT, "notFoundBlock", fun);

				unordered_map<size_t, vector<const NomCallable*>> overloadings;

				for (NomMethodTableEntry* mte : MethodTable)
				{
					auto namekey = NomNameRepository::Instance().GetNameID(mte->Method->GetName());
					auto match = overloadings.find(namekey);
					if (match == overloadings.end())
					{
						overloadings[namekey] = vector<const NomCallable*>();
					}
					//uint32_t dtac = mte->Method->GetDirectTypeParametersCount();
					auto& ole = overloadings[namekey];
					/*auto tamatch = ole.find(dtac);
					if (tamatch == ole.end())
					{
						ole[dtac] = unordered_map<uint32_t, vector<const NomCallable* >>();
					}
					auto& ole2 = ole[dtac];
					uint32_t tpc = mte->Method->GetArgumentCount();
					auto argmatch = ole2.find(tpc);
					if (argmatch == ole2.end())
					{
						ole2[tpc] = vector<const NomCallable*>();
					}*/
					ole.push_back(mte->Method);
				}

				builder->SetInsertPoint(start);
				SwitchInst* switch1 = builder->CreateSwitch(namearg, notfound, overloadings.size() + AllFields.size());

				builder->SetInsertPoint(notfound);
				static const char* emptystr = "";
				static const char* lookupfailstr = "Could not find any methods with matching name, type argument count, and argument count: ";
				builder->CreateCall(RTOutput_Name::GetLLVMElement(mod), { GetLLVMPointer(lookupfailstr), namearg });
				builder->CreateCall(RTOutput_Fail::GetLLVMElement(mod), { GetLLVMPointer(emptystr) });
				builder->CreateRet(UndefValue::get(fun->getReturnType()));

				//builder->SetInsertPoint(namenotfound);
				SimpleClassCompileEnv scce = SimpleClassCompileEnv(fun, this, nullarray(NomTypeParameterRef), TypeList({ /*&NomDynamicType::Instance(),*/ NomIntClass::GetInstance()->GetType()/*, NomIntClass::GetInstance()->GetType(), NomIntClass::GetInstance()->GetType()*/ }), thisType);
				for (auto field : AllFields)
				{
					std::string fieldName = field->GetName()->ToStdString();
					BasicBlock* fieldBlock = BasicBlock::Create(LLVMCONTEXT, "fieldInvoke:" + fieldName, fun);
					switch1->addCase(MakeInt<size_t>(NomNameRepository::Instance().GetNameID(fieldName)), fieldBlock);
					builder->SetInsertPoint(fieldBlock);
					auto fieldValue = EnsurePacked(builder, field->GenerateRead(builder, &scce, NomValue(thisarg, thisType)));
					auto fieldInvokeDispatcher = EnsureDynamicMethodInstruction::GenerateGetBestInvokeDispatcherDyn(builder, fieldValue/*, tacarg, argcarg*/);
					auto retStruct = builder->CreateInsertValue(UndefValue::get(GetDynamicDispatcherLookupResultType()), builder->CreateExtractValue(fieldInvokeDispatcher, { 0 }), { 0 });
					retStruct = builder->CreateInsertValue(retStruct, fieldValue, { 1 });
					builder->CreateRet(retStruct);
				}


				for (auto& ole1 : overloadings)
				{
					BasicBlock* ob1 = BasicBlock::Create(LLVMCONTEXT, "methodname:" + *NomNameRepository::Instance().GetNameFromID(ole1.first), fun);
					switch1->addCase(MakeInt<size_t>(ole1.first), ob1);
					builder->SetInsertPoint(ob1);
					/*SwitchInst* switch2 = builder->CreateSwitch(tacarg, notfound, ole1.second.size());

					for (auto& ole2 : ole1.second)
					{
						BasicBlock* ob2 = BasicBlock::Create(LLVMCONTEXT, "typeArgCount:" + to_string(ole2.first), fun);
						switch2->addCase(MakeInt<uint32_t>(ole2.first), ob2);
						builder->SetInsertPoint(ob2);
						SwitchInst* switch3 = builder->CreateSwitch(argcarg, notfound, ole2.second.size());

						for (auto& ole3 : ole2.second)
						{
							BasicBlock* ob3 = BasicBlock::Create(LLVMCONTEXT, "argCount" + to_string(ole3.first), fun);
							switch3->addCase(MakeInt<uint32_t>(ole3.first), ob3);
							builder->SetInsertPoint(ob3);*/

							//Dynamic dispatching never changes through casting: methods do not become more accepting of arguments - if they already restrict arguments in some way,
							//then that is grounds to reject a cast to more permissive arguments right away
							//If basic methods are changed and not just interface table entries on casting, then the dispatchers for such classes should do a method table lookup
							//instead of a direct statically bound call, but that should suffice

							Function* dispatcher = NomPartialApplication::GetDispatcherEntry(mod, linkage, /*ole2.first, ole3.first, */ole1.second, this/*, thisType*/);
							auto retStruct = builder->CreateInsertValue(UndefValue::get(GetDynamicDispatcherLookupResultType()),/* llvm::ConstantExpr::getPointerCast(*/dispatcher/*, POINTERTYPE)*/, { 0 });
							retStruct = builder->CreateInsertValue(retStruct, thisarg, { 1 });
							builder->CreateRet(retStruct);

					//	}
					//}
				}
				llvm::raw_os_ostream out(std::cout);
				if (verifyFunction(*fun, &out))
				{
					out.flush();
					std::cout << "Could not verify dispatcher lookup for class ";
					std::cout << this->GetName()->ToStdString();
					fun->print(out);
					out.flush();
					std::cout.flush();
					throw new std::exception();
				}
			}
			return fun;
		}

		bool NomClass::FindInstantiations(NomNamed* other, RecBufferTypeList& myArgs, InstantiationList& results) const
		{
			if (NomInterface::FindInstantiations(other, myArgs, results))
			{
				return true;
			}
			this->GetSuperClass();
			return false;
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
			for (int sii = 0; sii<superinsts.size(); sii++)
			{
				const NomInstantiationRef<NomInterface>& super = superinsts[sii];
				super.Elem->PreprocessInheritance();
				for (auto &inst : super.Elem->GetInstantiations())
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
			for (auto &sinst : superClassRef.Elem->GetInstantiations())
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
			auto iteType = InterfaceTableEntryType();
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