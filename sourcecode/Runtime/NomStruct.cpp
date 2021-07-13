#include "NomStruct.h"
#include "NomField.h"
#include "NomMemberContext.h"
#include "NomStructType.h"
#include "NomStructMethod.h"
#include "RTStruct.h"
#include "StructHeader.h"
#include "NomType.h"
#include "CompileHelpers.h"
#include "NomNameRepository.h"
#include "NomPartialApplication.h"
#include "IntClass.h"
#include "FloatClass.h"
#include <iostream>
#include "llvm/Support/raw_os_ostream.h"
#include "llvm/IR/Verifier.h"
#include "NomTypeVar.h"
#include "RTOutput.h"
#include "instructions/CallDispatchBestMethod.h"
#include "NomDynamicType.h"
#include "RTCast.h"
#include "NomClassType.h"
#include "NomTypeParameter.h"
#include "CallingConvConf.h"
#include "RTCompileConfig.h"

using namespace llvm;
using namespace std;
namespace Nom
{
	namespace Runtime
	{
		NomStruct::NomStruct(ConstantID structID, NomMemberContext* context, ConstantID closureTypeParams, RegIndex registerCount, RegIndex endargregcount, ConstantID initializerArgTypes) : NomCallableLoaded("STRUCT_" + to_string(structID), context, "STRUCT_" + to_string(structID) + ".$Constructor", registerCount, closureTypeParams, initializerArgTypes), NomDescriptor("STRUCTDICT_" + to_string(structID)), StructID(structID), /*InitializerArgTypes(initializerArgTypes),*/ EndArgRegisterCount(endargregcount)
		{
			NomConstants::GetStruct(structID)->SetStruct(this);
		}
		llvm::Function* NomStruct::createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const
		{
			Function* fun = Function::Create(GetLLVMFunctionType(), linkage, *GetSymbolName(), &mod);
			fun->setCallingConv(NOMCC);

			NomBuilder builder;
			BasicBlock* startBlock = BasicBlock::Create(LLVMCONTEXT, "", fun);
			builder->SetInsertPoint(startBlock);

			auto targc = GetDirectTypeParametersCount();
			auto argc = GetArgumentCount();

			Value** typeArgBuf = makealloca(Value*, targc);
			//Value** argBuf = makealloca(Value*, argc);

			auto carg = fun->arg_begin();
			for (decltype(targc) i = 0; i < targc; i++, carg++)
			{
				typeArgBuf[i] = carg;
			}
			//for (decltype(argc) i = 0; i < argc; i++, carg++)
			//{
			//	argBuf[i] = carg;
			//}

			auto constant = RTStruct::CreateConstant(this, GetDynamicFieldLookup(mod, linkage), GetDynamicFieldStore(mod, linkage), GetDynamicDispatcherLookup(mod, linkage)/*, GetLLVMPointer(&(dictionary->Dictionary))*/);
			GlobalVariable* gv = new GlobalVariable(mod, constant->getType(), false, linkage, constant, "RT_NOM_STRUCTDESC_" + to_string(StructID));

			StructInstantiationCompileEnv sice = StructInstantiationCompileEnv(regcount, fun, GetAllTypeParameters(), GetArgumentTypes(nullptr), this, EndArgRegisterCount);

			StructHeader::GenerateConstructorCode(builder, ArrayRef<Value*>(typeArgBuf, targc),/*, ArrayRef<Value*>(argBuf, argc)*/&sice, gv, GetInstructions());

			if (linkage == GlobalValue::LinkageTypes::ExternalLinkage)
			{
				GenerateDictionaryEntries(mod);
			}
			return fun;
		}
		NomTypeRef NomStruct::GetReturnType(const NomSubstitutionContext* context) const
		{
			return &NomStructType::Instance();
		}
		//TypeList NomStruct::GetArgumentTypes(const NomSubstitutionContext* context) const
		//{
		//	if (context != nullptr && context->GetTypeArgumentCount() > 0)
		//	{
		//		return NomConstants::GetTypeList(InitializerArgTypes)->GetTypeList(context);
		//	}
		//	else
		//	{
		//		if (argtypes.data() == nullptr)
		//		{
		//			NomSubstitutionContextMemberContext nscmc = NomSubstitutionContextMemberContext(this);
		//			argtypes = NomConstants::GetTypeList(InitializerArgTypes)->GetTypeList(&nscmc);
		//		}
		//		return argtypes;
		//	}
			//return NomConstants::GetTypeList(InitializerArgTypes)->GetTypeList(this);
			////if (argtypes.size() != Fields.size())
			////{
			////	argtypes.clear();
			////	for (auto field : Fields)
			////	{
			////		argtypes.push_back(field->GetType());
			////	}
			////}
			////return argtypes;
		//}
		//int NomStruct::GetArgumentCount() const
		//{
		//	if (argtypes.data() == nullptr)
		//	{
		//		return NomConstants::GetTypeList(InitializerArgTypes)->GetSize();
		//	}
		//	return argtypes.size();
		//}
		const NomField* NomStruct::GetField(NomStringRef name) const
		{
			for (auto field : Fields)
			{
				if (NomStringEquality()(field->GetName(), name))
				{
					return field;
				}
			}
			throw new std::exception();
		}

		NomStructMethod* NomStruct::AddMethod(std::string& name, std::string& qname, ConstantID typeParameters, ConstantID returnType, ConstantID argTypes, RegIndex regcount)
		{
			NomStructMethod* meth = new NomStructMethod(this, name, qname, typeParameters, returnType, argTypes, regcount);
			Methods.push_back(meth);
			return meth;
		}

		NomStructField* NomStruct::AddField(const ConstantID name, const ConstantID type, bool isReadOnly, RegIndex valueRegister) {
			NomStructField* field = new NomStructField(this, name, type, isReadOnly, Fields.size(), valueRegister);
			Fields.push_back(field);
			return field;
		}

		llvm::FunctionType* NomStruct::GetDynamicFieldLookupType()
		{
			static llvm::FunctionType* funtype = FunctionType::get(REFTYPE, { REFTYPE, numtype(size_t) }, false);
			return funtype;
		}
		llvm::Function* NomStruct::GetDynamicFieldLookup(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const
		{
			auto targcount = this->GetDirectTypeParametersCount();
			NomTypeRef* selfArgsBuf = makealloca(NomTypeRef, targcount);
			for (decltype(targcount) i = 0; i < targcount; i++)
			{
				selfArgsBuf[i] = this->GetDirectTypeParameters()[i]->GetVariable();
			}
			auto thisType = &NomStructType::Instance();
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
				auto nameSwitch = builder->CreateSwitch(namearg, notfound, this->Fields.size());

				SimpleClassCompileEnv scce = SimpleClassCompileEnv(fun, this, nullarray(NomTypeParameterRef), TypeList({ NomIntClass::GetInstance()->GetType() }), thisType);

				for (auto field : Fields)
				{
					std::string fieldName = field->GetName()->ToStdString();
					BasicBlock* fieldBlock = BasicBlock::Create(LLVMCONTEXT, "field:" + fieldName, fun);
					nameSwitch->addCase(MakeInt<size_t>(NomNameRepository::Instance().GetNameID(fieldName)), fieldBlock);
					builder->SetInsertPoint(fieldBlock);
					builder->CreateRet(field->GenerateRead(builder, &scce, NomValue(thisarg, thisType)));//removed EnsurePacked from here, because struct values are always supposed to be packed
				}
				builder->SetInsertPoint(notfound);
				static const char* lookupfailstr = "Could not find any fields with matching name!";
				CreateDummyReturn(builder, fun);
				llvm::raw_os_ostream out(std::cout);
				if (verifyFunction(*fun, &out))
				{
					out.flush();
					std::cout << "Could not verify field lookup for struct";
					//std::cout << this->GetName()->ToStdString();
					fun->print(out);
					out.flush();
					std::cout.flush();
					throw new std::exception();
				}
			}
			return fun;
		}

		llvm::FunctionType* NomStruct::GetDynamicFieldStoreType()
		{
			static llvm::FunctionType* funtype = FunctionType::get(llvm::Type::getVoidTy(LLVMCONTEXT), { REFTYPE, numtype(size_t), REFTYPE }, false);
			return funtype;
		}

		llvm::Function* NomStruct::GetDynamicFieldStore(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const
		{
			auto targcount = this->GetDirectTypeParametersCount();
			NomTypeRef* selfArgsBuf = makealloca(NomTypeRef, targcount);
			for (decltype(targcount) i = 0; i < targcount; i++)
			{
				selfArgsBuf[i] = this->GetDirectTypeParameters()[i]->GetVariable();
			}
			auto thisType = &NomStructType::Instance();
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
				auto nameSwitch = builder->CreateSwitch(namearg, notfound, this->Fields.size());

				SimpleClassCompileEnv scce = SimpleClassCompileEnv(fun, this, nullarray(NomTypeParameterRef), TypeList({ NomIntClass::GetInstance()->GetType(), &NomDynamicType::Instance() }), thisType);

				for (auto field : Fields)
				{
					std::string fieldName = field->GetName()->ToStdString();
					BasicBlock* fieldBlock = BasicBlock::Create(LLVMCONTEXT, "field:" + fieldName, fun);
					BasicBlock* fieldWriteBlock = BasicBlock::Create(LLVMCONTEXT, "fieldWrite:" + fieldName, fun);
					nameSwitch->addCase(MakeInt<size_t>(NomNameRepository::Instance().GetNameID(fieldName)), fieldBlock);
					builder->SetInsertPoint(fieldBlock);
					builder->CreateCondBr(RTCast::GenerateCast(builder, &scce, newValue, field->GetType()), fieldWriteBlock, errorBlock);

					builder->SetInsertPoint(fieldWriteBlock);
					auto writeValue = newValue;
					//if (field->GetType()->IsSubtype(NomIntClass::GetInstance()->GetType(), false))
					//{
					//	if (!writeValue->getType()->isIntegerTy())
					//	{
					//		if (writeValue->getType() != REFTYPE)
					//		{
					//			throw new std::exception();
					//		}
					//		writeValue = UnpackInt(builder, writeValue);
					//	}
					//}
					//else if (field->GetType()->IsSubtype(NomFloatClass::GetInstance()->GetType(), false))
					//{
					//	if (!writeValue->getType()->isFloatingPointTy())
					//	{
					//		if (writeValue->getType() != REFTYPE)
					//		{
					//			throw new std::exception();
					//		}
					//		writeValue = UnpackFloat(builder, writeValue);
					//	}
					//}
					//else
					//{
						writeValue = EnsurePacked(builder, writeValue);
					//}
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
					std::cout << "Could not verify field store fun for struct";
					//std::cout << this->GetName()->ToStdString();
					fun->print(out);
					out.flush();
					std::cout.flush();
					throw new std::exception();
				}
			}
			return fun;
		}

		llvm::StructType* NomStruct::GetDynamicDispatcherLookupResultType()
		{
			return NomClass::GetDynamicDispatcherLookupResultType();
		}

		llvm::FunctionType* NomStruct::GetDynamicDispatcherLookupType()
		{
			static llvm::FunctionType* funtype = FunctionType::get(GetDynamicDispatcherLookupResultType(), { REFTYPE, numtype(size_t), numtype(int32_t), numtype(int32_t) }, false);
			return funtype;
		}
		llvm::Function* NomStruct::GetDynamicDispatcherLookup(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const
		{
			auto targcount = this->GetDirectTypeParametersCount();
			NomTypeRef* selfArgsBuf = makealloca(NomTypeRef, targcount);
			for (decltype(targcount) i = 0; i < targcount; i++)
			{
				selfArgsBuf[i] = this->GetDirectTypeParameters()[i]->GetVariable();
			}
			auto thisType = &NomStructType::Instance();
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
				argiter++;
				llvm::Argument* tacarg = argiter;
				argiter++;
				llvm::Argument* argcarg = argiter;

				NomBuilder builder;

				BasicBlock* start = BasicBlock::Create(LLVMCONTEXT, "", fun);
				BasicBlock* notfound = BasicBlock::Create(LLVMCONTEXT, "notFoundBlock", fun);
				//BasicBlock* namenotfound = BasicBlock::Create(LLVMCONTEXT, "nameNotFoundBlock", fun);

				unordered_map<size_t, unordered_map<uint32_t, unordered_map<uint32_t, vector<const NomCallable*>>>> overloadings;

				for (NomStructMethod* nsm : Methods)
				{
					auto namekey = NomNameRepository::Instance().GetNameID(nsm->GetName());
					auto match = overloadings.find(namekey);
					if (match == overloadings.end())
					{
						overloadings[namekey] = unordered_map<uint32_t, unordered_map<uint32_t, vector<const NomCallable*>>>();
					}
					uint32_t dtac = nsm->GetDirectTypeParametersCount();
					auto& ole = overloadings[namekey];
					auto tamatch = ole.find(dtac);
					if (tamatch == ole.end())
					{
						ole[dtac] = unordered_map<uint32_t, vector<const NomCallable* >>();
					}
					auto& ole2 = ole[dtac];
					uint32_t tpc = nsm->GetArgumentCount();
					auto argmatch = ole2.find(tpc);
					if (argmatch == ole2.end())
					{
						ole2[tpc] = vector<const NomCallable*>();
					}
					ole2[tpc].push_back(nsm);
				}

				builder->SetInsertPoint(start);
				SwitchInst* switch1 = builder->CreateSwitch(namearg, notfound, overloadings.size() + Fields.size());

				builder->SetInsertPoint(notfound);
				static const char* emptystr = "";
				static const char* lookupfailstr = "Could not find any methods with matching name, type argument count, and argument count: ";
				builder->CreateCall(RTOutput_Name::GetLLVMElement(mod), { GetLLVMPointer(lookupfailstr), namearg });
				builder->CreateCall(RTOutput_Fail::GetLLVMElement(mod), { GetLLVMPointer(emptystr) });
				builder->CreateRet(UndefValue::get(fun->getReturnType()));

				//builder->SetInsertPoint(namenotfound);
				SimpleClassCompileEnv scce = SimpleClassCompileEnv(fun, this, nullarray(NomTypeParameterRef), TypeList({ &NomDynamicType::Instance(), NomIntClass::GetInstance()->GetType(), NomIntClass::GetInstance()->GetType(), NomIntClass::GetInstance()->GetType() }), /*thisType*/ nullptr);
				for (auto field : Fields)
				{
					std::string fieldName = field->GetName()->ToStdString();
					BasicBlock* fieldBlock = BasicBlock::Create(LLVMCONTEXT, "fieldInvoke:" + fieldName, fun);
					switch1->addCase(MakeInt<size_t>(NomNameRepository::Instance().GetNameID(fieldName)), fieldBlock);
					builder->SetInsertPoint(fieldBlock);
					auto fieldValue = EnsurePacked(builder, field->GenerateRead(builder, &scce, NomValue(thisarg, thisType)));
					auto fieldInvokeDispatcher = CallDispatchBestMethod::GenerateGetBestInvokeDispatcherDyn(builder, fieldValue, tacarg, argcarg);
					auto retStruct = builder->CreateInsertValue(UndefValue::get(GetDynamicDispatcherLookupResultType()), builder->CreateExtractValue(fieldInvokeDispatcher, { 0 }), { 0 });
					retStruct = builder->CreateInsertValue(retStruct, fieldValue, { 1 });
					builder->CreateRet(retStruct);
				}


				for (auto& ole1 : overloadings)
				{
					BasicBlock* ob1 = BasicBlock::Create(LLVMCONTEXT, "methodname:" + *NomNameRepository::Instance().GetNameFromID(ole1.first), fun);
					switch1->addCase(MakeInt<size_t>(ole1.first), ob1);
					builder->SetInsertPoint(ob1);
					SwitchInst* switch2 = builder->CreateSwitch(tacarg, notfound, ole1.second.size());

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
							builder->SetInsertPoint(ob3);

							//Dynamic dispatching never changes through casting: methods do not become more accepting of arguments - if they already restrict arguments in some way,
							//then that is grounds to reject a cast to more permissive arguments right away
							//If basic methods are changed and not just interface table entries on casting, then the dispatchers for such classes should do a method table lookup
							//instead of a direct statically bound call, but that should suffice

							Function* dispatcher = NomPartialApplication::GetDispatcherEntry(mod, linkage, ole2.first, ole3.first, ole3.second, this, thisType);
							/*Function::Create(NomPartialApplication::GetDynamicDispatcherType(ole2.first, ole3.first), linkage, "", &mod);*/
							//builder->CreateRet();
							auto retStruct = builder->CreateInsertValue(UndefValue::get(GetDynamicDispatcherLookupResultType()), llvm::ConstantExpr::getPointerCast(dispatcher, POINTERTYPE), { 0 });
							retStruct = builder->CreateInsertValue(retStruct, thisarg, { 1 });
							builder->CreateRet(retStruct);
							//
							//BasicBlock* dispblock = BasicBlock::Create(LLVMCONTEXT, "", dispatcher);
							//builder->SetInsertPoint(dispblock);
							//llvm::Value* targalloca;
							//if (ole2.first > 0)
							//{
							//	targalloca = builder->CreateAlloca(RTSubtyping::TypeArgumentListStackType());
							//	auto argfields = builder->CreateAlloca(TYPETYPE, MakeInt<uint32_t>(ole2.first));
							//	MakeStore(builder, mod, ConstantPointerNull::get(RTSubtyping::TypeArgumentListStackType()->getPointerTo()), builder->CreateGEP(targalloca, { MakeInt32(0), MakeInt32(TypeArgumentListStackFields::Next) }));
							//	MakeStore(builder, mod, argfields, builder->CreateGEP(targalloca, { MakeInt32(0), MakeInt32(TypeArgumentListStackFields::Types) }));
							//	auto disparg = dispatcher->arg_begin();
							//	for (uint32_t i = 0; i < ole2.first; i++, disparg++)
							//	{
							//		MakeStore(builder, mod, disparg, builder->CreateGEP(argfields, MakeInt32(i)));
							//	}
							//}
							//else
							//{
							//	targalloca = llvm::ConstantPointerNull::get(RTSubtyping::TypeArgumentListStackType()->getPointerTo());
							//}
							//llvm::Value** methodargs = makealloca(llvm::Value*, ole2.first + 1 + ole3.first);
							//llvm::Value** typeargtypes = makealloca(llvm::Value*, ole2.first);
							//llvm::Value** valargtypes = makealloca(llvm::Value*, ole3.first); 
							//auto disparg = dispatcher->arg_begin();
							//for (uint32_t i = 0; i < ole2.first; i++, disparg++)
							//{
							//	typeargtypes[i] = disparg;
							//	methodargs[i] = disparg;
							//}
							////we know the this-pointer is ok; that's where we got the dispatcher from in the first place
							//methodargs[ole2.first] = disparg;
							//disparg++;
							//for (uint32_t i = 0; i < ole3.first; i++, disparg++)
							//{
							//	valargtypes[i] = ObjectHeader::CreateExtractType(builder, mod, disparg);
							//	methodargs[i + ole2.first + 1] = disparg;
							//}
							//for (auto& meth : ole3.second)
							//{
							//	BasicBlock* nextMethodBlock = BasicBlock::Create(LLVMCONTEXT, "", dispatcher);
							//	for (uint32_t i = 0; i < ole2.first; i++)
							//	{
							//		throw new std::exception(); //TODO: implement checking type argument constraints
							//	}
							//	auto methargtypes = meth->GetArgumentTypes();
							//	for (uint32_t i = 0; i < ole3.first; i++)
							//	{
							//		llvm::Value* agg = RTSubtyping::CreateTypeSubtypingCheck(builder, mod, valargtypes[i], methargtypes[i]->GetLLVMElement(mod), ConstantPointerNull::get(RTSubtyping::TypeArgumentListStackType()->getPointerTo()), targalloca);
							//		auto nextBlock = BasicBlock::Create(LLVMCONTEXT, "", dispatcher);
							//		builder->CreateCondBr(agg, nextBlock, nextMethodBlock);
							//		builder->SetInsertPoint(nextBlock);
							//	}
							//	auto fcargs = ArrayRef<llvm::Value*>(methodargs, ole2.first + 1 + ole3.first);
							//	auto methcall = GenerateFunctionCall(builder, mod, meth->GetLLVMElement(mod), fcargs, true);
							//	methcall->setTailCallKind(llvm::CallInst::TailCallKind::TCK_Tail);
							//	auto mctype = methcall->getType();
							//	llvm::Value* retval = methcall;
							//	if (mctype->isIntegerTy(1))
							//	{
							//		retval = PackBool(builder, mod, methcall);
							//	}
							//	else if (mctype->isIntegerTy(INTTYPE->getPrimitiveSizeInBits()))
							//	{
							//		retval = PackInt(builder, methcall);
							//	}
							//	else if (mctype->isFloatingPointTy())
							//	{
							//		retval = PackFloat(builder, methcall);
							//	}
							//	builder->CreateRet(retval);
							//	builder->SetInsertPoint(nextMethodBlock);
							//}
							//static const char* failstr = "Could not find applicable method!";
							//builder->CreateRet(builder->CreateCall(RTOutput_Fail::GetLLVMElement(mod), { GetLLVMPointer(failstr) }));

							//llvm::raw_os_ostream out(std::cout);
							//if (verifyFunction(*dispatcher, &out))
							//{
							//	out.flush();
							//	std::cout << "Could not verify Dispatcher for method name ";
							//	std::cout << ole3.second.at(0)->GetName();
							//	fun->print(out);
							//	out.flush();
							//	std::cout.flush();
							//	throw new std::exception();
							//}
						}
					}
				}
				llvm::raw_os_ostream out(std::cout);
				if (verifyFunction(*fun, &out))
				{
					out.flush();
					std::cout << "Could not verify dispatcher lookup for struct";
					//std::cout << this->GetName()->ToStdString();
					fun->print(out);
					out.flush();
					std::cout.flush();
					throw new std::exception();
				}
			}
			return fun;
		}

		void NomStruct::GenerateDictionaryEntries(llvm::Module& mod) const
		{
			llvm::GlobalVariable* gvar = mod.getGlobalVariable(dictionary->SymbolName);
			if (gvar == nullptr)
			{
				vector<Constant*> constants;
				for (NomStructField* field : Fields)
				{
					auto cnstnt = new GlobalVariable(mod, RTDescriptorDictionaryEntry::GetLLVMType(), true, GlobalValue::LinkageTypes::InternalLinkage, RTDescriptorDictionaryEntry::CreateConstant(RTDescriptorDictionaryEntryKind::Field, field->IsReadOnly(), field->GetVisibility(), field->GetType()->GetLLVMElement(mod), field->Index, field->GetType()->IsSubtype(NomIntClass::GetInstance()->GetType(), false), field->GetType()->IsSubtype(NomFloatClass::GetInstance()->GetType(), false)));
					constants.push_back(cnstnt);
					DICTKEYTYPE dictKey = NomNameRepository::Instance().GetNameID(NomConstants::GetString(field->Name)->GetText()->ToStdString());
					dictionary->AddEntryKey(dictKey);
				}

				unordered_map<size_t, vector<NomCallable*>> overloadings;

				for (NomStructMethod* meth : Methods)
				{
					auto namekey = NomNameRepository::Instance().GetNameID(meth->GetName());
					auto match = overloadings.find(namekey);
					if (match == overloadings.end())
					{
						overloadings[namekey] = vector<NomCallable*>();
					}
					overloadings[namekey].push_back(meth);
				}

				for (auto& ovlpair : overloadings)
				{
					std::string symname = *this->GetSymbolName() + "$" + ovlpair.second.at(0)->GetName();
					NomPartialApplication* npa = new NomPartialApplication(symname, ovlpair.second, this, &NomStructType::Instance());

					auto cnstnt = new GlobalVariable(mod, RTDescriptorDictionaryEntry::GetLLVMType(), true, GlobalValue::LinkageTypes::InternalLinkage, RTDescriptorDictionaryEntry::CreateConstant(RTDescriptorDictionaryEntryKind::PartialApp, true, Visibility::Public, npa->GetLLVMElement(mod), 0, false, false));
					constants.push_back(cnstnt);
					dictionary->AddEntryKey(ovlpair.first);

					//this->childDescriptors.push_back(npa);
				}

				llvm::ArrayType* type = arrtype(RTDescriptorDictionaryEntry::GetLLVMPointerType(), constants.size());
				gvar = new llvm::GlobalVariable(mod, type, true, GlobalValue::LinkageTypes::ExternalLinkage, ConstantArray::get(type, constants), dictionary->SymbolName);
			}
		}
		bool NomStruct::GetHasRawInvoke() const
		{
			return NomLambdaOptimizationLevel>0;
		}
		llvm::FunctionType* NomStruct::GetLLVMFunctionType(const NomSubstitutionContext* context) const
		{
			std::vector<llvm::Type* > args(GetTypeParametersCount() + GetArgumentCount());
			unsigned int j;
			for (j = 0; j < GetTypeParametersCount(); j++)
			{
				args[j] = TYPETYPE;
			}
			unsigned int i;
			auto argtypes = (GetArgumentTypes(context));
			for (i = 0; i < argtypes.size(); i++)
			{
				args[j + i] = argtypes[i]->GetLLVMType();
			}
			return llvm::FunctionType::get(GetReturnType(context)->GetLLVMType(), args, false);
		}
		llvm::ArrayRef<NomTypeParameterRef> NomStruct::GetArgumentTypeParameters() const
		{
			return GetAllTypeParameters();
		}
		inline void NomStruct::PushDependencies(std::set<ConstantID>& set) const
		{
			NomCallableLoaded::PushDependencies(set);
			for (auto nsm : Methods)
			{
				nsm->PushDependencies(set);
			}
			for (auto nsf : Fields)
			{
				nsf->PushDependencies(set);
			}
		}
	}
}