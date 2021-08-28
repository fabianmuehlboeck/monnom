#include "NomInterface.h"
#include "NomConstants.h"
#include "RecursionBuffer.h"
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
#include "RTFunctionalInterface.h"
#include "RTInterfaceTableEntry.h"
#include "RTGeneralInterface.h"
#include "RTInterface.h"
#include "llvm/Support/raw_os_ostream.h"
#include "llvm/IR/Verifier.h"
#include "CallingConvConf.h"
#include "NomModule.h"
#include <set>
#include "NomVMInterface.h"
#include <vector>
#include "NomMethodKey.h"
#include "IMT.h"
#include "RTOutput.h"
#include "RTCast.h"
#include "RTSignature.h"

using namespace llvm;
using namespace std;
namespace Nom
{
	namespace Runtime
	{
		const llvm::SmallVector<NomClassTypeRef, 16> NomInterface::GetSuperNameds(llvm::ArrayRef<NomTypeRef> args) const
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
				NomMethodTableEntry* mte = new NomMethodTableEntry(*cmeth, (*cmeth)->GetLLVMFunctionType(), mthis->MethodTable.size());
				(*cmeth)->SetOffset(mthis->MethodTable.size());
				mthis->MethodTable.push_back(mte);
			}
			mthis->InterfaceTableEntries[GetID()] = 0;
			auto iteType = InterfaceTableEntryType();
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
		bool NomInterface::FindInstantiations(NomNamed* other, RecBufferTypeList& myArgs, InstantiationList& results) const
		{
			if (this == other)
			{
				results.emplace_back(myArgs);
				return true;
			}
			return false;
			//TODO: loop through superinterfaces
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
				switch (constant->Type)
				{
				case NomConstantType::CTClass:
					((NomClassConstant*)constant)->EnsureClassLoaded(mod);
					break;
				case NomConstantType::CTInterface:
					((NomInterfaceConstant*)constant)->EnsureInterfaceLoaded(mod);
					break;
				default:
					break; //other kind of constant; ignore
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
		llvm::Constant* NomInterface::GetMethodTable(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const
		{
			auto mtablesize = MethodTable.size();
			llvm::Constant** methodArr = makealloca(llvm::Constant*, mtablesize);
			for (auto mte : MethodTable)
			{
				methodArr[mtablesize - 1 - mte->Offset] = llvm::ConstantExpr::getPointerCast(mte->CallableVersion->GetLLVMElement(mod), POINTERTYPE);
			}
			return llvm::ConstantArray::get(GetMethodTableType(false), llvm::ArrayRef<llvm::Constant*>(methodArr, MethodTable.size()));
		}
		llvm::ArrayType* NomInterface::GetMethodTableType(bool generic) const
		{
			return arrtype(POINTERTYPE, generic ? 0 : MethodTable.size());
		}


		llvm::FunctionType* NomInterface::GetInterfaceTableLookupType()
		{
			static llvm::FunctionType* funtype = FunctionType::get(inttype(32), { numtype(InterfaceID) }, false);
			return funtype;
		}

		llvm::Constant* NomInterface::GetInterfaceTableLookup(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const
		{
			std::string ddlname = "NOM_RT_ITL_" + *this->GetSymbolName();
			llvm::Function* fun = mod.getFunction(ddlname.data());
			if (fun == nullptr)
			{
				llvm::FunctionType* ft = GetInterfaceTableLookupType();
				fun = llvm::Function::Create(ft, linkage, ddlname.data(), &mod);
				fun->setCallingConv(NOMCC);

				auto argiter = fun->arg_begin();
				llvm::Argument* ifaceid = argiter;

				NomBuilder builder;

				BasicBlock* start = BasicBlock::Create(LLVMCONTEXT, "", fun);
				BasicBlock* notfound = BasicBlock::Create(LLVMCONTEXT, "notFoundBlock", fun);


				builder->SetInsertPoint(start);
				auto idswitch = builder->CreateSwitch(ifaceid, notfound, InterfaceTableEntries.size());

				for (auto& ite : InterfaceTableEntries)
				{
					BasicBlock* interfaceBlock = BasicBlock::Create(LLVMCONTEXT, "", fun);
					idswitch->addCase(MakeInt32(ite.first), interfaceBlock);
					builder->SetInsertPoint(interfaceBlock);
					builder->CreateRet(MakeInt32(-ite.second));
				}

				builder->SetInsertPoint(notfound);
				builder->CreateIntrinsic(Intrinsic::trap, {}, {});
				builder->CreateRet(MakeInt32(0));

				llvm::raw_os_ostream out(std::cout);
				if (verifyFunction(*fun, &out))
				{
					out.flush();
					std::cout << "Could not verify interface table lookup for class ";
					std::cout << this->GetName()->ToStdString();
					fun->print(out);
					out.flush();
					std::cout.flush();
					throw new std::exception();
				}
			}
			return fun;
		}


		llvm::GlobalVariable* NomInterface::GetSuperInstances(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const
		{
			llvm::Twine t = "NOM_ST_";
			llvm::SmallVector<char, 64> buf;
			llvm::StringRef nameref = t.concat(this->GetName()->ToStdString()).toStringRef(buf);
			llvm::GlobalVariable* ret = mod.getGlobalVariable(nameref, false);
			if (ret == nullptr)
			{
				ret = new llvm::GlobalVariable(mod, GetSuperInstancesType(false), true, linkage, nullptr, nameref);
				llvm::Constant** entries = makealloca(llvm::Constant*, instantiations.size() + 1);
				auto selfInterface = mod.getNamedAlias("NOM_ALIAS_" + this->GetName()->ToStdString());
				{
					size_t instasize = GetTypeParametersCount();
					llvm::GlobalVariable* instaArgs = new GlobalVariable(mod, arrtype(TYPETYPE, instasize), true, linkage, nullptr);
					entries[0] = ConstantStruct::get(SuperInstanceEntryType(), selfInterface, ConstantExpr::getGetElementPtr(arrtype(TYPETYPE, instasize), instaArgs, ArrayRef<Constant*>({ MakeInt32(0), MakeInt32(instasize) })));
					llvm::Constant** instaArgBuf = makealloca(llvm::Constant*, instasize);
					auto allVars = GetAllTypeVariables();
					for (size_t i = 0; i < instasize; i++)
					{
						instaArgBuf[instasize - (i + 1)] = allVars[i]->GetLLVMElement(mod);
					}
					instaArgs->setInitializer(ConstantArray::get(arrtype(TYPETYPE, instasize), ArrayRef<Constant*>(instaArgBuf, instasize)));
				}
				size_t entrynum = 1;
				for (auto& instantiation : instantiations)
				{
					size_t instasize = instantiation.second.size();
					llvm::GlobalVariable* instaArgs = new GlobalVariable(mod, arrtype(TYPETYPE, instasize), true, linkage, nullptr, "NOM_ARGS_"+nameref.str()+"_"+std::to_string(entrynum));
					//auto insta = instantiation.first->GetType(instantiation.second);
					entries[entrynum] = ConstantStruct::get(SuperInstanceEntryType(), ConstantExpr::getPointerCast(instantiation.first->GetLLVMElement(mod), RTInterface::GetLLVMType()->getPointerTo()), ConstantExpr::getGetElementPtr(arrtype(TYPETYPE, instasize), instaArgs, ArrayRef<Constant*>({ MakeInt32(0), MakeInt32(instasize) }))); //insta->GetLLVMElement(mod);
					entrynum++;
					llvm::Constant** instaArgBuf = makealloca(llvm::Constant*, instasize);
					for (size_t i = 0; i < instasize; i++)
					{
						instaArgBuf[instasize - (i + 1)] = instantiation.second[i]->GetLLVMElement(mod);
					}
					instaArgs->setInitializer(ConstantArray::get(arrtype(TYPETYPE, instasize), ArrayRef<Constant*>(instaArgBuf, instasize)));
				}
				ret->setInitializer(llvm::ConstantArray::get(GetSuperInstancesType(false), llvm::ArrayRef<llvm::Constant*>(entries, entrynum)));
			}
			return ret;
		}
		llvm::ArrayType* NomInterface::GetSuperInstancesType(bool generic) const
		{
			return arrtype(SuperInstanceEntryType(), (generic ? 0 : (instantiations.size() + 1)));
		}

		llvm::Constant* NomInterface::createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const
		{
			llvm::Twine t = "NOM_ID_";
			llvm::SmallVector<char, 64> buf;
			llvm::StringRef nameref = t.concat(this->GetName()->ToStdString()).toStringRef(buf);
			llvm::Constant* ret = RTClass::FindConstant(mod, nameref);

			llvm::GlobalVariable* gvar = mod.getGlobalVariable(dictionary->SymbolName);
			if (ret == nullptr)
			{
				llvm::GlobalAlias* gvar_alias = GlobalAlias::create(RTInterface::GetLLVMType(), 0, GlobalValue::LinkageTypes::ExternalLinkage, "NOM_ALIAS_" + this->GetName()->ToStdString(), &mod);
				if (Methods.size() == 1 && Methods[0]->GetName().empty())
				{
					ret = RTFunctionalInterface::CreateGlobalConstant(mod, linkage, nameref, this, MakeInt32((uint32_t)this->GetTypeParametersCount()), MakeInt<size_t>(instantiations.size() + 1), GetSuperInstances(mod, linkage), GetLLVMPointer(&runtimeInstantiations));
				}
				else
				{
					ret = RTGeneralInterface::CreateGlobalConstant(mod, linkage, nameref, this,
						MakeInt(this->GetTypeParametersCount()),
						MakeInt(instantiations.size() + 1),
						GetSuperInstances(mod, linkage),
						GetLLVMPointer(&runtimeInstantiations));
				}
				gvar_alias->setAliasee(ConstantExpr::getPointerCast(ret, RTInterface::GetLLVMType()->getPointerTo()));

				RegisterGlobalForAddressLookup(nameref.str());
			}
			return ret;
		}

		llvm::Constant* NomInterface::findLLVMElement(llvm::Module& mod) const
		{
			llvm::Twine t = "NOM_ID_";
			llvm::SmallVector<char, 64> buf;
			llvm::StringRef nameref = t.concat(this->GetName()->ToStdString()).toStringRef(buf);
			if (Methods.size() == 1 && Methods[0]->GetName().empty())
			{
				return RTFunctionalInterface::FindConstant(mod, nameref);
			}
			else
			{
				return RTGeneralInterface::FindConstant(mod, nameref);
			}
		}


		InterfaceID NomInterface::idcounter()
		{
			static InterfaceID id = 0; return ++id;
		}

		NomInterface::NomInterface(const std::string& name) : NomDescriptor("RT_NOM_CLS_DICT_" + name), id(idcounter()) 
		{
			bgc_register_root(((char*)&(this->runtimeInstantiations)), ((char*)&(this->runtimeInstantiations)) + sizeof(RuntimeInstantiationDictionary));
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

		NomInterfaceLoaded::NomInterfaceLoaded(const ConstantID name, ConstantID typeArgs, const ConstantID superInterfaces, const NomMemberContext* parent) : NomInterface(NomConstants::GetString(name)->GetText()->ToStdString()), NomNamedLoaded(name, typeArgs, parent), /*NomMemberContextLoaded(parent, typeArgs),*/ superInterfaces(superInterfaces)
		{
			NomInterface::Register(NomConstants::GetString(name)->GetText(), this);
		}

		NomMethodLoaded* NomInterfaceLoaded::AddMethod(const std::string& name, const std::string& qname, const ConstantID typeParameters, const ConstantID returnType, const ConstantID argumentTypes, const RegIndex regcount, bool isFinal) {
			NomMethodLoaded* meth = new NomMethodLoaded(this, name, qname, returnType, argumentTypes, regcount, typeParameters, isFinal);
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
					types = (NomTypeRef*)gcalloc_uncollectable(sizeof(NomTypeRef) * instantiation.TypeArgs.size());
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
						auto targArr = (NomTypeRef*)gcalloc(sizeof(NomTypeRef) * (contextCount + typeArgs.size()));
						int i = 0;
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


		llvm::StructType* InterfaceTableEntryType() {
			return RTInterfaceTableEntry::GetLLVMType();
		}

		llvm::StructType* SuperInstanceEntryType()
		{
			static llvm::StructType* type = llvm::StructType::get(RTInterface::GetLLVMType()->getPointerTo(), TYPETYPE->getPointerTo());
			return type;
		}
		NomInterfaceInternal::NomInterfaceInternal(NomStringRef name, const NomMemberContext* parent) : NomInterface(name->ToStdString()), NomNamedInternal(name, parent)
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
			NomInstantiationRef<NomInterface>* arr = (NomInstantiationRef<NomInterface>*)gcalloc(sizeof(NomInstantiationRef<NomInterface>) * superInterfaces.size());
			size_t pos = 0;
			for (auto ir : superInterfaces)
			{
				size_t tcount = ir.TypeArgs.size();
				NomTypeRef* tarr = (NomTypeRef*)gcalloc(sizeof(NomTypeRef) * tcount);
				for (size_t i = 0; i < tcount; i++)
				{
					tarr[i] = ir.TypeArgs[i]->SubstituteSubtyping(context);
				}
				arr[pos] = NomInstantiationRef<NomInterface>(superInterfaces[pos].Elem, TypeList(tarr, tcount));
				pos++;
			}
			return llvm::ArrayRef<NomInstantiationRef<NomInterface>>(arr, pos);
		}
		void NomInterfaceInternal::SetSuperInterfaces(llvm::ArrayRef<NomInstantiationRef<NomInterface>> superInterfaces)
		{
			if (this->superInterfaces.data() != nullptr || superInterfaces.data() == nullptr)
			{
				throw new std::exception();
			}
			this->superInterfaces = superInterfaces;
		}
		void NomInterfaceInternal::SetSuperInterfaces()
		{
			SetSuperInterfaces(llvm::ArrayRef<NomInstantiationRef<NomInterface>>(reinterpret_cast<NomInstantiationRef<NomInterface>*>(this), (size_t)0));
		}
		void NomInterfaceInternal::GetClassDependencies(llvm::SmallVector<const NomClassInternal*, 4>& results) const
		{
		}
		void NomInterfaceInternal::GetInterfaceDependencies(llvm::SmallVector<const NomInterfaceInternal*, 4>& results) const
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
			SmallVector<pair<NomMethodKey*, NomMethodTableEntry*>, 8> crtPairs;

			for (auto meth : MethodTable)
			{
				NomMethodKey* nmk = NomMethodKey::GetMethodKey(meth->Method);

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
				//argsIter++;
				//Value* argsarr[3];
				//argsarr[0] = argsIter;
				//argsIter++;
				//argsarr[1] = argsIter;
				//argsIter++;
				//argsarr[2] = argsIter;

				for (auto& crtp : crtPairs)
				{
					builder->SetInsertPoint(nextBlock);
					BasicBlock* currentBlock = BasicBlock::Create(LLVMCONTEXT, "", fun);
					nextBlock = BasicBlock::Create(LLVMCONTEXT, "", fun);
					auto keyMatch = builder->CreateICmpEQ(builder->CreatePtrToInt(methodKeyArg, numtype(intptr_t)), ConstantExpr::getPtrToInt(crtp.first->GetLLVMElement(mod), numtype(intptr_t)));
					builder->CreateCondBr(keyMatch, currentBlock, nextBlock);

					builder->SetInsertPoint(currentBlock);
					//Value* methodTypeArgs = ConstantPointerNull::get(TYPETYPE);
					//auto tParamCount = crtp.second->Method->GetDirectTypeParametersCount();
					//if (tParamCount > 0)
					//{
					//	auto mtaAlloca = builder->CreateAlloca(TYPETYPE, tParamCount);
					//	methodTypeArgs = builder->CreateGEP(mtaAlloca, MakeInt32(tParamCount));
					//	for (decltype(tParamCount) i = 0; i < tParamCount; i++)
					//	{
					//		if (i < 2 || (tParamCount == 3 && crtp.second->Method->GetArgumentCount() == 0))
					//		{
					//			MakeStore(builder, builder->CreatePointerCast(argsarr[i], TYPETYPE), builder->CreateGEP(methodTypeArgs, MakeInt32(-(i + 1))));
					//		}
					//		else
					//		{
					//			Value* restArgArr = MakeLoad(builder, argsarr[2]);
					//			for (; i < tParamCount; i++)
					//			{
					//				MakeStore(builder, builder->CreatePointerCast(MakeLoad(builder, builder->CreateGEP(restArgArr, MakeInt32(i - 2))), TYPETYPE), builder->CreateGEP(methodTypeArgs, MakeInt32(-(i + 1))));
					//			}
					//		}
					//	}
					//}

					auto llvmReturnType = crtp.second->CallableVersion->FunType->getReturnType();
					if (crtp.second->Method->GetContainer() != this)
					{
						throw new std::exception(); //TODO: implement
					}
					else
					{
						CastedValueCompileEnv cvce = CastedValueCompileEnv(crtp.second->Method->GetDirectTypeParameters(), crtp.second->Method->GetParent()->GetAllTypeParameters(), fun, 3, crtp.second->Method->GetArgumentCount(), typeArgsPtrArg);
						auto castResult = RTCast::GenerateCast(builder, &cvce, checkValue, crtp.second->Method->GetReturnType(nullptr));
						builder->CreateCondBr(castResult, nextBlock, castFailBlock);
					}
				}

				builder->SetInsertPoint(nextBlock);
				builder->CreateRetVoid();
				//builder->CreateRet(MakeUInt(1, 1));
			}

			return fun;
		}
	}
}

using namespace Nom::Runtime;
extern "C" DLLEXPORT void* RT_NOM_GetUniqueInstantiation(NomInterface * iface, void* rtinterface, void** typearr, size_t * hasharr, int arrsize)
{
	static void* (*createNewClassTypeFun)(void*, void*, void**, int, size_t, void*) = (void* (*)(void*, void*, void**, int, size_t, void*))((intptr_t)NomJIT::Instance().getSymbolAddress("RT_NOM_InstantiateClassType"));
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