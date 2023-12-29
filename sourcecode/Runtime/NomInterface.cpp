PUSHDIAGSUPPRESSION
#include "llvm/Support/raw_os_ostream.h"
#include "llvm/IR/Verifier.h"
POPDIAGSUPPRESSION
#include "NomInterface.h"
#include "NomConstants.h"
#include "NomJIT.h"
#include "NomClassType.h"
#include "NomAlloc.h"
#include "GlobalNameAddressLookupList.h"
#include "CompileHelpers.h"
#include "RTVTable.h"
#include "NomMethodTableEntry.h"
#include "NomCallableVersion.h"
#include "NomLambda.h"
#include "NomConstructor.h"
#include "NomTypeVar.h"
#include "RTInterfaceTableEntry.h"
#include "RTInterface.h"
#include "CallingConvConf.h"
#include "NomModule.h"
#include <set>
#include "NomVMInterface.h"
#include <vector>
#include "NomInterfaceCallTag.h"
#include "IMT.h"
#include "RTOutput.h"
#include "RTCast.h"
#include "RTSignature.h"
#include "Metadata.h"
#include "RTConfig.h"

using namespace llvm;
using namespace std;
namespace Nom
{
	namespace Runtime
	{
		const llvm::SmallVector<NomClassTypeRef, 16> NomInterface::GetSuperNameds() const
		{
			llvm::SmallVector<NomClassTypeRef, 16> ret;
			for (auto nomref : GetSuperInterfaces())
			{
				auto targcount = nomref.TypeArgs.size();
				NomTypeRef* targs = makealloca(NomTypeRef, targcount);
				for (size_t i = 0; i < targcount; i++)
				{
					targs[i] = nomref.TypeArgs[i];
				}
				ret.push_back(nomref.Elem->GetType(llvm::ArrayRef<NomTypeRef>(targs, targcount)));
			}
			return ret;
		}
		void NomInterface::PreprocessInheritance() const
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
			for (auto super : GetSuperInterfaces())
			{
				super.Elem->PreprocessInheritance();
				for (auto& inst : super.Elem->instantiations)
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
			auto mthis = const_cast<NomInterface*>(this);
			for (auto cmeth = Methods.begin(); cmeth != Methods.end(); cmeth++)
			{
				if (NomLambdaOptimizationLevel == 0 || !(*cmeth)->GetName().empty())
				{
					NomMethodTableEntry* mte = new NomMethodTableEntry(*cmeth, (*cmeth)->GetLLVMFunctionType(), mthis->MethodTable.size());
					(*cmeth)->SetOffset(mthis->MethodTable.size());
					mthis->MethodTable.push_back(mte);
				}
			}
			mthis->InterfaceTableEntries[GetID()] = 0;
			auto supers = GetSuperInterfaces();
			for (auto super : supers)
			{
				super.Elem->PreprocessInheritance();
			}
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
							mthis->MethodTable.push_back(new NomMethodTableEntry(super.Elem->MethodTable[i]->Method, super.Elem->MethodTable[i]->CallableVersion->FunType, mthis->MethodTable.size()));
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
		const llvm::ArrayRef<NomInstantiationRef<NomInterface>> NomInterfaceLoaded::GetSuperInterfaces(const NomSubstitutionContext* context) const
		{
			if (context != nullptr && context->GetTypeArgumentCount() > 0)
			{
				return NomConstants::GetSuperInterfaces(superInterfaces)->GetSuperInterfaces(context);
			}
			else
			{
				if (superInterfacesBuf.data() == nullptr)
				{
					NomSubstitutionContextMemberContext nscmc(this);
					superInterfacesBuf = NomConstants::GetSuperInterfaces(superInterfaces)->GetSuperInterfaces(&nscmc);
				}
				return superInterfacesBuf;
			}
		}
		void NomInterfaceLoaded::ResolveDependencies(NomModule* mod) const
		{
			std::set<ConstantID> seenIDs;
			std::queue<ConstantID> constantQueue;
			NOM_CONSTANT_DEPENCENCY_CONTAINER depBuf;
			PushDependencies(seenIDs);
			for (auto cid : seenIDs)
			{
				if (cid != 0)
				{
					constantQueue.push(cid);
				}
			}
			while (!constantQueue.empty())
			{
				depBuf.clear();
				ConstantID first = constantQueue.front();
				constantQueue.pop();
				NomConstant* constant = NomConstants::Get(first);
				constant->FillConstantDependencies(depBuf);
				for (auto dep : depBuf)
				{
					if (dep == 0 || seenIDs.find(dep) != seenIDs.end())
					{
						continue;
					}
					constantQueue.push(dep);
					seenIDs.insert(dep);
				}
				if (constant->Type == NomConstantType::CTClass)
				{
					(static_cast<NomClassConstant*>(constant))->EnsureClassLoaded(mod);
				}
				else if (constant->Type == NomConstantType::CTInterface)
				{
					(static_cast<NomInterfaceConstant*>(constant))->EnsureInterfaceLoaded(mod);
				}
				else
				{
					//other kind of constant; ignore
				}
			}
		}
		void NomInterfaceLoaded::PushDependencies(std::set<ConstantID>& set) const
		{
			set.insert(this->name);
			set.insert(this->typeParametersID);
			set.insert(this->superInterfaces);
			for (auto method : Methods)
			{
				dynamic_cast<NomCallableLoaded*>(method)->PushDependencies(set);
			}
		}
		llvm::Constant* NomInterface::GetMethodTable(llvm::Module& mod, [[maybe_unused]] llvm::GlobalValue::LinkageTypes linkage) const
		{
			auto mtablesize = MethodTable.size();
			llvm::Constant** methodArr = makealloca(llvm::Constant*, mtablesize);
			for (auto mte : MethodTable)
			{
				methodArr[mtablesize - 1 - static_cast<size_t>(mte->Offset)] = llvm::ConstantExpr::getPointerCast(mte->CallableVersion->GetLLVMElement(mod), POINTERTYPE);
			}
			return llvm::ConstantArray::get(GetMethodTableType(false), llvm::ArrayRef<llvm::Constant*>(methodArr, MethodTable.size()));
		}
		llvm::ArrayType* NomInterface::GetMethodTableType(bool generic) const
		{
			return arrtype(POINTERTYPE, generic ? 0 : MethodTable.size());
		}

		size_t NomInterface::GetSuperClassCount() const
		{
			return 0;
		}
		llvm::Constant* NomInterface::GetSuperInstances(llvm::Module& mod, [[maybe_unused]] llvm::GlobalValue::LinkageTypes linkage, llvm::GlobalVariable* gvar, llvm::StructType* stetype) const
		{
			auto _instantiations = GetInstantiations();
			auto instaCount = _instantiations.size();
			auto entryArr = makealloca(Constant*, instaCount + 1);
			auto fieldArr = makealloca(Constant*, instaCount + 2);
			auto typeArr = makealloca(Type*, instaCount + 2);
			auto orderedInstas = makealloca(const NomInterface*, instaCount);
			typeArr[0] = arrtype(SuperInstanceEntryType(), instaCount + 1);
			int pos = 0;
			entryArr++;
			fieldArr++;
			typeArr++;
			for (auto& instantiation : _instantiations)
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
				}
				pos++;
			}
			auto selfInterface = mod.getNamedAlias("NOM_ALIAS_" + this->GetName()->ToStdString());
			{
				size_t instasize = GetTypeParametersCount();
				llvm::Constant** instaArgBuf = makealloca(llvm::Constant*, instasize);
				auto allVars = GetAllTypeVariables();
				for (size_t i = 0; i < instasize; i++)
				{
					instaArgBuf[instasize - (i + 1)] = allVars[i]->GetLLVMElement(mod);
				}
				fieldArr[0] = ConstantArray::get(arrtype(TYPETYPE, instasize), ArrayRef<Constant*>(instaArgBuf, instasize));
				typeArr[0] = arrtype(TYPETYPE, instasize);
			}
			typeArr--;
			if (stetype->isOpaque())
			{
				stetype->setBody(ArrayRef<Type*>(typeArr, instaCount + 2), false);
			}
			while (pos > 0)
			{
				pos--;
				auto instantiation = _instantiations[orderedInstas[pos]];
				size_t instasize = instantiation.size();
				entryArr[pos] = ConstantStruct::get(SuperInstanceEntryType(),
					ConstantExpr::getPointerCast(orderedInstas[pos]->GetLLVMElement(mod), RTInterface::GetLLVMType()->getPointerTo()),
					ConstantExpr::getGetElementPtr(gvar->getValueType(), gvar, ArrayRef<Constant*>({ MakeInt32(0), MakeInt32(gvar->getValueType()->getStructNumElements() - 1), MakeInt32(pos + 2), MakeInt32(instasize) })));
			}
			entryArr--;
			{
				size_t instasize = GetTypeParametersCount();
				entryArr[0] = ConstantStruct::get(SuperInstanceEntryType(),
					selfInterface,
					ConstantExpr::getGetElementPtr(gvar->getValueType(), gvar, ArrayRef<Constant*>({ MakeInt32(0), MakeInt32(gvar->getValueType()->getStructNumElements() - 1), MakeInt32(1), MakeInt32(instasize) })));
			}
			fieldArr--;
			fieldArr[0] = ConstantArray::get(arrtype(SuperInstanceEntryType(), instaCount + 1), ArrayRef<Constant*>(entryArr, instaCount + 1));
			return ConstantStruct::get(stetype, ArrayRef<Constant*>(fieldArr, instaCount + 2));
		}

		llvm::ArrayType* NomInterface::GetSuperInstancesType(bool generic) const
		{
			return arrtype(SuperInstanceEntryType(), (generic ? 0 : (instantiations.size() + 1)));
		}

		llvm::Constant* NomInterface::createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const
		{
			std::string lookupName = "MONNOM_INTERFACE_" + *GetSymbolName();
			llvm::Constant* ret = RTClass::FindConstant(mod, lookupName);

			if (ret == nullptr)
			{
				auto stetype = StructType::create(LLVMCONTEXT, "MONNOM_IFCSUPERS_" + *GetSymbolName());
				auto gvartype = StructType::get(LLVMCONTEXT, { RTInterface::GetLLVMType() , stetype }, false);
				auto retgvar = new GlobalVariable(mod, gvartype, true, linkage, nullptr, lookupName);
				ret = retgvar;
				llvm::GlobalAlias* gvar_alias = GlobalAlias::create(RTInterface::GetLLVMType(), 0, GlobalValue::LinkageTypes::ExternalLinkage, "NOM_ALIAS_" + this->GetName()->ToStdString(), &mod);
				auto flags = RTInterfaceFlags::IsInterface;
				llvm::Constant* signature = nullptr;
				if (Methods.size() == 1 && Methods[0]->GetName().empty())
				{
					flags = flags | RTInterfaceFlags::IsFunctional;
					signature = RTSignature::CreateGlobalConstant(mod, linkage, "MONNOM_RT_SIG_LAMBDA_" + this->GetName()->ToStdString(), Methods[0]);
				}
				auto superInsts = GetSuperInstances(mod, linkage, retgvar, stetype);
				auto stePointer = ConstantExpr::getGetElementPtr(gvartype, retgvar, ArrayRef<Constant*>({ MakeInt32(0), MakeInt32(1), MakeInt32(0), MakeInt32(0) }));
				auto cnst = RTInterface::CreateConstant(this, flags, MakeInt32(this->GetTypeParametersCount()), MakeInt<size_t>(0), MakeInt<size_t>(instantiations.size() + 1), stePointer, GetCheckReturnTypeFunction(mod, linkage), GetLLVMPointer(&runtimeInstantiations), signature);
				retgvar->setInitializer(ConstantStruct::get(gvartype, { cnst, superInsts }));

				gvar_alias->setAliasee(ConstantExpr::getPointerCast(ret, RTInterface::GetLLVMType()->getPointerTo()));

				RegisterGlobalForAddressLookup(lookupName);
			}
			return ConstantExpr::getPointerCast(ret, RTInterface::GetLLVMPointerType());
		}

		llvm::Constant* NomInterface::findLLVMElement(llvm::Module& mod) const
		{
			std::string lookupName = "MONNOM_INTERFACE_" + *GetSymbolName();
			auto cnst = mod.getGlobalVariable(lookupName);
			if (cnst == nullptr)
			{
				return cnst;
			}
			else
			{
				return ConstantExpr::getPointerCast(cnst, RTInterface::GetLLVMPointerType());
			}
		}


		InterfaceID NomInterface::idcounter()
		{
			static InterfaceID id = 0; return ++id;
		}

		NomInterface::NomInterface() : id(idcounter())
		{
			bgc_register_root((reinterpret_cast<char*>(& (this->runtimeInstantiations))), (reinterpret_cast<char*>(& (this->runtimeInstantiations))) + sizeof(RuntimeInstantiationDictionary));
		}

		bool NomInterface::GetHasRawInvoke() const
		{
			for (auto meth : AllMethods)
			{
				if (meth->GetName().empty())
				{
					return true;
				}
			}
			return false;
		}

		bool NomInterface::HasLambdaMethod() const
		{
			for (auto& meth : Methods)
			{
				if (meth->GetName().empty())
				{
					return true;
				}
			}
			return false;
		}

		bool NomInterface::HasNoMethods() const
		{
			return Methods.size() == 0;
		}

		NomMethod* NomInterface::GetLambdaMethod() const
		{
			for (auto& meth : Methods)
			{
				if (meth->GetName().empty())
				{
					return meth;
				}
			}
			throw new std::exception();
		}

		NomInterfaceLoaded::NomInterfaceLoaded(const ConstantID _name, ConstantID _typeArgs, const ConstantID _superInterfaces, const NomMemberContext* _parent) : NomInterface(), NomNamedLoaded(_name, _typeArgs, _parent), superInterfaces(_superInterfaces)
		{
			NomInterface::Register(NomConstants::GetString(_name)->GetText(), this);
		}

		NomMethodLoaded* NomInterfaceLoaded::AddMethod(const std::string& _name, const std::string& _qname, const ConstantID _typeParameters, const ConstantID _returnType, const ConstantID _argumentTypes, const RegIndex _regcount, bool _isFinal) {
			NomMethodLoaded* meth = new NomMethodLoaded(this, _name, _qname, _returnType, _argumentTypes, _regcount, _typeParameters, _isFinal);
			NomInterface::AddMethod(meth);
			return meth;
		}

		void NomInterface::AddInstantiation(const NomInstantiationRef<NomInterface> instantiation) const
		{
			if (instantiations.count(instantiation.Elem) > 0)
			{
				if (instantiation.TypeArgs.size() > 0)
				{
					if (NomClassType::ArgumentsSubtypes(instantiation.TypeArgs, instantiations[instantiation.Elem]))
					{
						auto types = const_cast<NomTypeRef*>(instantiations[instantiation.Elem].data());
						for (size_t i = instantiation.TypeArgs.size(); i > 0;)
						{
							i--;
							if (!(types[i]->IsSubtype(instantiation.TypeArgs[i]) && instantiation.TypeArgs[i]->IsSubtype(types[i])))
							{
								throw new std::exception();
							}
							types[i] = instantiation.TypeArgs[i];
						}
					}
				}
			}
			else
			{
				NomTypeRef* types = nullptr;
				if (instantiation.TypeArgs.size() > 0)
				{
					types = makegcalloc_uncollectable(NomTypeRef, instantiation.TypeArgs.size());
					for (size_t i = instantiation.TypeArgs.size(); i > 0;)
					{
						i--;
						types[i] = instantiation.TypeArgs[i];
					}
				}
				instantiations[instantiation.Elem] = llvm::ArrayRef<NomTypeRef>(types, instantiation.TypeArgs.size());
			}
		}

		NomInstantiationRef<const NomMethod> NomInterface::GetMethod(const NomSubstitutionContext* context, NomStringRef methodName, const TypeList typeArgs, const TypeList argTypes) const
		{
			for (auto meth : Methods)
			{
				if (meth->GetName() == methodName->ToStdString() && meth->Satisfies(context, typeArgs, argTypes))
				{
					if (context->GetTypeArgumentCount() == 0)
					{
						return NomInstantiationRef<const NomMethod>(meth, typeArgs);
					}
					else if (typeArgs.size() == 0)
					{
						return NomInstantiationRef<const NomMethod>(meth, context->GetTypeParameters());
					}
					else
					{
						decltype(context->GetTypeArgumentCount()) contextCount = context->GetTypeArgumentCount();
						auto targArr = makegcalloc(NomTypeRef, (contextCount + typeArgs.size()));
						size_t i = 0;
						while (i < contextCount)
						{
							targArr[i] = context->GetTypeVariable(i);
							i++;
						}
						while (i < contextCount + typeArgs.size())
						{
							targArr[i] = typeArgs[i - contextCount];
						}
						return NomInstantiationRef<const NomMethod>(meth, TypeList(targArr, contextCount + typeArgs.size()));
					}
				}
			}
			throw methodName;
		}

		const std::unordered_map<const NomInterface*, TypeList>& NomInterface::GetInstantiations() const
		{
			return instantiations;
		}


		llvm::StructType* SuperInstanceEntryType()
		{
			static llvm::StructType* type = llvm::StructType::get(RTInterface::GetLLVMType()->getPointerTo(), TYPETYPE->getPointerTo());
			return type;
		}
		NomInterfaceInternal::NomInterfaceInternal(NomStringRef name, const NomMemberContext* parent) : NomInterface(), NomNamedInternal(name, parent)
		{
			NomInterface::Register(name, this);
		}
		void NomInterfaceInternal::ResolveDependencies(NomModule* mod) const
		{
			llvm::SmallVector<const NomInterfaceInternal*, 4> ifacebuf;
			llvm::SmallVector<const NomClassInternal*, 4> clsbuf;
			GetInterfaceDependencies(ifacebuf);
			GetClassDependencies(clsbuf);
			for (auto iface : ifacebuf)
			{
				mod->AddInternalInterface(iface);
			}
			for (auto cls : clsbuf)
			{
				mod->AddInternalClass(cls);
			}
		}
		const llvm::ArrayRef<NomInstantiationRef<NomInterface>> NomInterfaceInternal::GetSuperInterfaces(const NomSubstitutionContext* context) const
		{
			if (context == nullptr || context->GetTypeArgumentCount() == 0 || superInterfaces.size() == 0)
			{
				return superInterfaces;
			}
			NomInstantiationRef<NomInterface>* arr = makegcalloc(NomInstantiationRef<NomInterface>, superInterfaces.size());
			size_t pos = 0;
			for (auto ir : superInterfaces)
			{
				size_t tcount = ir.TypeArgs.size();
				NomTypeRef* tarr = makegcalloc(NomTypeRef, tcount);
				for (size_t i = 0; i < tcount; i++)
				{
					tarr[i] = ir.TypeArgs[i]->SubstituteSubtyping(context);
				}
				arr[pos] = NomInstantiationRef<NomInterface>(superInterfaces[pos].Elem, TypeList(tarr, tcount));
				pos++;
			}
			return llvm::ArrayRef<NomInstantiationRef<NomInterface>>(arr, pos);
		}
		void NomInterfaceInternal::SetSuperInterfaces(llvm::ArrayRef<NomInstantiationRef<NomInterface>> _superInterfaces)
		{
			if (this->superInterfaces.data() != nullptr || (_superInterfaces.data() == nullptr && _superInterfaces.size()>0))
			{
				throw new std::exception();
			}
			this->superInterfaces = _superInterfaces;
		}
		void NomInterfaceInternal::SetSuperInterfaces()
		{
			SetSuperInterfaces(llvm::ArrayRef<NomInstantiationRef<NomInterface>>(static_cast<NomInstantiationRef<NomInterface>*>(nullptr), static_cast<size_t>(0)));
		}
		void NomInterfaceInternal::GetClassDependencies([[maybe_unused]] llvm::SmallVector<const NomClassInternal*, 4>& results) const
		{
		}
		void NomInterfaceInternal::GetInterfaceDependencies([[maybe_unused]] llvm::SmallVector<const NomInterfaceInternal*, 4>& results) const
		{
		}
		llvm::FunctionType* NomInterface::GetGetUniqueInstantiationFunctionType()
		{
			static FunctionType* ft = FunctionType::get(TYPETYPE, { POINTERTYPE, RTInterface::GetLLVMType()->getPointerTo(), TYPETYPE->getPointerTo(), numtype(size_t)->getPointerTo(), inttype(32) }, false);
			return ft;
		}
		llvm::Function* NomInterface::GetGetUniqueInstantiationFunction(llvm::Module& mod)
		{
			Function* fun = mod.getFunction("RT_NOM_GetUniqueInstantiation");
			if (fun == nullptr)
			{
				fun = Function::Create(GetGetUniqueInstantiationFunctionType(), GlobalValue::LinkageTypes::ExternalLinkage, "RT_NOM_GetUniqueInstantiation", mod);
				fun->setCallingConv(CallingConv::C);
			}
			return fun;
		}

		llvm::Constant* NomInterface::GetSignature(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const
		{
			for (auto meth : MethodTable)
			{
				if (meth->Method->GetName().empty())
				{
					return RTSignature::CreateGlobalConstant(mod, linkage, "MONNOM_RT_SIGNATURE_" + GetName()->ToStdString(), meth->Method);
				}
			}
			return ConstantPointerNull::get(RTSignature::GetLLVMType()->getPointerTo());
		}
		llvm::Constant* NomInterface::GetCheckReturnTypeFunction(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const
		{
			SmallVector<pair<NomInterfaceCallTag*, NomMethodTableEntry*>, 8> crtPairs;

			for (auto meth : MethodTable)
			{
				NomInterfaceCallTag* nmk = NomInterfaceCallTag::GetMethodKey(meth->Method);

				bool found = false;
				for (auto& pair : crtPairs)
				{
					if (pair.first == nmk)
					{
						found = true;
						break;
					}
				}
				if (!found)
				{
					crtPairs.push_back(make_pair(nmk, meth));
				}
			}

			std::string ddlname = "NOM_RT_CRT_" + *this->GetSymbolName();
			llvm::Function* fun = mod.getFunction(ddlname.data());
			if (fun == nullptr)
			{
				fun = Function::Create(GetCheckReturnValueFunctionType(), linkage, ddlname, mod);
				BasicBlock* startBlock = BasicBlock::Create(LLVMCONTEXT, "start", fun);
				BasicBlock* nextBlock = startBlock;
				NomBuilder builder;
				static const char* castFailMsg = "Cast failed!";
				BasicBlock* castFailBlock = BasicBlock::Create(LLVMCONTEXT, "castFail", fun);
				RTOutput_Fail::MakeBlockFailOutputBlock(builder, castFailMsg, castFailBlock);

				auto argsIter = fun->arg_begin();
				Value* methodKeyArg = argsIter;
				argsIter++;
				Value* typeArgsPtrArg = argsIter;
				argsIter++;
				Value* checkValue = argsIter;

				for (auto& crtp : crtPairs)
				{
					builder->SetInsertPoint(nextBlock);
					BasicBlock* currentBlock = BasicBlock::Create(LLVMCONTEXT, "", fun);
					nextBlock = BasicBlock::Create(LLVMCONTEXT, "", fun);
					auto keyMatch = builder->CreateICmpEQ(builder->CreatePtrToInt(methodKeyArg, numtype(intptr_t)), ConstantExpr::getPtrToInt(crtp.first->GetLLVMElement(mod), numtype(intptr_t)));
					builder->CreateCondBr(keyMatch, currentBlock, nextBlock);

					builder->SetInsertPoint(currentBlock);
					auto llvmReturnType = crtp.second->CallableVersion->FunType->getReturnType();
					if (crtp.second->Method->GetContainer() != this)
					{
						throw new std::exception(); //TODO: implement
					}
					else
					{
						CastedValueCompileEnvIndirect cvce = CastedValueCompileEnvIndirect(builder, crtp.second->Method->GetDirectTypeParameters(), crtp.second->Method->GetParent()->GetAllTypeParameters(), fun, 3, crtp.second->Method->GetArgumentCount(), typeArgsPtrArg);
						auto castResult = RTCast::GenerateCast(builder, &cvce, RTValue::GetValue(builder, checkValue, NomType::DynamicRef), crtp.second->Method->GetReturnType(nullptr));
						//builder->CreateIntrinsic(Intrinsic::expect, { inttype(1) }, { castResult, MakeUInt(1,1) });
						//builder->CreateCondBr(castResult, nextBlock, castFailBlock, GetLikelyFirstBranchMetadata());
						builder->CreateBr(nextBlock);
					}
				}

				builder->SetInsertPoint(nextBlock);
				builder->CreateRetVoid();
				//builder->CreateRet(MakeUInt(1, 1));
			}

			return fun;
		}
		llvm::Constant* NomInterface::GetInterfaceDescriptor(llvm::Module& mod) const
		{
			return GetLLVMElement(mod);
		}
	}
}

using namespace Nom::Runtime;
extern "C" DLLEXPORT void* RT_NOM_GetUniqueInstantiation(NomInterface * iface, void* rtinterface, void** typearr, size_t * hasharr, int arrsize)
{
	static void* (*createNewClassTypeFun)(void*, void*, void**, int, size_t, void*) = reinterpret_cast<void* (*)(void*, void*, void**, int, size_t, void*)>(NomJIT::Instance().getSymbolAddress("RT_NOM_InstantiateClassType"));
	static NomRTHashArrHash RTArrHash;
	size_t arrhash = RTArrHash(hasharr, arrsize);
	std::tuple<void**, int, size_t> key = make_tuple(typearr, arrsize, arrhash);

	const std::lock_guard<std::mutex> lock(iface->runtimeInstantiationsMutex);
	auto lookupResult = iface->runtimeInstantiations.find(key);
	if (lookupResult != iface->runtimeInstantiations.end())
	{
		return lookupResult->second;
	}
	size_t newHash = NomClassType::GetHashCode(iface, arrhash);
	void* newCT = createNewClassTypeFun(CPP_NOM_CLASSTYPEALLOC(iface->GetTypeParametersCount()), rtinterface, typearr, arrsize, newHash, nullptr);
	iface->runtimeInstantiations[key] = newCT;
	return newCT;
}
