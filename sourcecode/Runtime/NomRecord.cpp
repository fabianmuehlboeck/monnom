#include "NomRecord.h"
#include "NomField.h"
#include "NomMemberContext.h"
#include "NomRecordMethod.h"
#include "RTRecord.h"
#include "RecordHeader.h"
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
#include "RTVTable.h"
#include "RefValueHeader.h"
#include "EnsureDynamicMethodInstruction.h"
#include "RTInterface.h"
#include "IMT.h"
#include "NomLambdaCallTag.h"
#include "Metadata.h"
#include "NomRecordCallTag.h"
#include "PWObject.h"
#include "PWTypeArr.h"

using namespace llvm;
using namespace std;
namespace Nom
{
	namespace Runtime
	{
		NomRecord::NomRecord(ConstantID structID, NomMemberContext* context, ConstantID closureTypeParams, RegIndex registerCount, RegIndex endargregcount, ConstantID initializerArgTypes) : NomCallableLoaded("STRUCT_" + to_string(structID), context, "STRUCT_" + to_string(structID) + ".$Constructor", registerCount, closureTypeParams, initializerArgTypes), StructID(structID), /*InitializerArgTypes(initializerArgTypes),*/ EndArgRegisterCount(endargregcount)
		{
			NomConstants::GetRecord(structID)->SetStruct(this);
		}
		llvm::Type* NomRecord::GetLLVMType() const
		{
			return RecordHeader::GetLLVMType(this->Fields.size());
		}
		llvm::Function* NomRecord::createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const
		{
			Function* fun = Function::Create(GetLLVMFunctionType(), linkage, *GetSymbolName(), &mod);
			fun->setCallingConv(NOMCC);

			NomBuilder builder;

			BasicBlock* startBlock = BasicBlock::Create(LLVMCONTEXT, "", fun);
			builder->SetInsertPoint(startBlock);

			auto targc = GetDirectTypeParametersCount();
			auto argc = GetArgumentCount();

			Value** typeArgBuf = makealloca(Value*, targc);

			auto carg = fun->arg_begin();
			for (decltype(targc) i = 0; i < targc; i++, carg++)
			{
				typeArgBuf[i] = carg;
			}

			bool hasRawInvoke = false;

			auto ddstructType = StructType::create(LLVMCONTEXT, "MONNOM_RT_STRUCTDD_" + to_string(StructID));
			auto gvartype = StructType::get(LLVMCONTEXT, { arrtype(inttype(64), hasRawInvoke ? 1 : 0), RTRecord::GetLLVMType(), ddstructType });
			Constant* pushArr[1] = { MakeUInt(64,0) };
			GlobalVariable* gv = new GlobalVariable(mod, gvartype, true, linkage, nullptr, "RT_NOM_STRUCTDESC_" + to_string(StructID));
			gv->setAlignment(Align(256));
			auto ddtable = GetDynamicDispatcherLookup(mod, linkage, ddstructType);
			auto ddarr = makealloca(Constant*, IMTsize);
			for (decltype(IMTsize) i = 0; i < IMTsize; i++)
			{
				ddarr[i] = ConstantExpr::getPointerCast(ConstantExpr::getGetElementPtr(gvartype, gv, ArrayRef<Constant*>({ MakeInt32(0), MakeInt32(2), MakeInt32(i) })), GetDynamicDispatchListEntryType()->getPointerTo());
			}

			auto ddreftable = ConstantArray::get(arrtype(GetDynamicDispatchListEntryType()->getPointerTo(), IMTsize), ArrayRef<Constant*>(ddarr, IMTsize));

			auto constant = RTRecord::CreateConstant(this, GetDynamicFieldLookup(mod, linkage), GetDynamicFieldStore(mod, linkage), GetInterfaceTableLookup(mod, linkage), ddreftable);
			gv->setInitializer(ConstantStruct::get(gvartype, { ConstantArray::get(arrtype(inttype(64), hasRawInvoke ? 1 : 0), ArrayRef<Constant*>(pushArr, hasRawInvoke ? 1 : 0)), constant, ddtable }));

			StructInstantiationCompileEnv sice = StructInstantiationCompileEnv(regcount, fun, GetAllTypeParameters(), GetArgumentTypes(nullptr), this, EndArgRegisterCount);

			RecordHeader::GenerateConstructorCode(builder, ArrayRef<Value*>(typeArgBuf, targc), &sice, ConstantExpr::getGetElementPtr(gvartype, gv, ArrayRef<Constant*>({ MakeInt32(0), MakeInt32(1) })), GetInstructions());

			return fun;
		}
		NomTypeRef NomRecord::GetReturnType(const NomSubstitutionContext* context) const
		{
			return &NomDynamicType::RecordInstance();
		}
		const NomField* NomRecord::GetField(NomStringRef name) const
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

		NomRecordMethod* NomRecord::AddMethod(std::string& name, std::string& qname, ConstantID typeParameters, ConstantID returnType, ConstantID argTypes, RegIndex regcount)
		{
			NomRecordMethod* meth = new NomRecordMethod(this, name, qname, typeParameters, returnType, argTypes, regcount);
			Methods.push_back(meth);
			return meth;
		}

		NomRecordField* NomRecord::AddField(const ConstantID name, const ConstantID type, bool isReadOnly, RegIndex valueRegister) {
			NomRecordField* field = new NomRecordField(this, name, type, isReadOnly, Fields.size(), valueRegister);
			Fields.push_back(field);
			return field;
		}

		llvm::Function* NomRecord::GetDynamicFieldLookup(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const
		{
			auto targcount = this->GetDirectTypeParametersCount();
			NomTypeRef* selfArgsBuf = makealloca(NomTypeRef, targcount);
			for (decltype(targcount) i = 0; i < targcount; i++)
			{
				selfArgsBuf[i] = this->GetDirectTypeParameters()[i]->GetVariable();
			}
			auto thisType = &NomDynamicType::RecordInstance();
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

		llvm::Function* NomRecord::GetDynamicFieldStore(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const
		{
			auto targcount = this->GetDirectTypeParametersCount();
			NomTypeRef* selfArgsBuf = makealloca(NomTypeRef, targcount);
			for (decltype(targcount) i = 0; i < targcount; i++)
			{
				selfArgsBuf[i] = this->GetDirectTypeParameters()[i]->GetVariable();
			}
			auto thisType = &NomDynamicType::RecordInstance();
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
				auto nameSwitch = builder->CreateSwitch(namearg, notfound, this->Fields.size());

				SimpleClassCompileEnv scce = SimpleClassCompileEnv(fun, this, nullarray(NomTypeParameterRef), TypeList({ NomIntClass::GetInstance()->GetType(), &NomDynamicType::Instance() }), thisType);

				for (auto field : Fields)
				{
					std::string fieldName = field->GetName()->ToStdString();
					BasicBlock* fieldBlock = BasicBlock::Create(LLVMCONTEXT, "field:" + fieldName, fun);
					nameSwitch->addCase(MakeInt<size_t>(NomNameRepository::Instance().GetNameID(fieldName)), fieldBlock);
					builder->SetInsertPoint(fieldBlock);
					RTCast::GenerateCast(builder, &scce, newValue, field->GetType());
					auto writeValue = newValue;
					writeValue = EnsurePacked(builder, writeValue);
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
					fun->print(out);
					out.flush();
					std::cout.flush();
					throw new std::exception();
				}
			}
			return fun;
		}
		llvm::Constant* NomRecord::GetInterfaceTableLookup(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const
		{
			auto rettype = arrtype(GetIMTFunctionType()->getPointerTo(), IMTsize);
			auto retarr = makealloca(Constant*, IMTsize);

			for (decltype(IMTsize) i = 0; i < IMTsize; i++)
			{
				auto fun = Function::Create(GetIMTFunctionType(), linkage, "MONNOM_RT_RECORDIMT_" + *GetSymbolName() + "_" + std::to_string(i), mod);
				fun->setCallingConv(NOMCC);
				BasicBlock* startBlock = BasicBlock::Create(LLVMCONTEXT, "", fun);
				NomBuilder builder;
				builder->SetInsertPoint(startBlock);

				auto argiter = fun->arg_begin();
				auto argarr = makealloca(Value*, 2 + RTConfig_NumberOfVarargsArguments);
				argarr[0] = argiter;

				auto callTag = argiter;
				argiter++;
				auto varargs = makealloca(Value*, RTConfig_NumberOfVarargsArguments + 1);
				for (decltype(RTConfig_NumberOfVarargsArguments) i = 0; i <= RTConfig_NumberOfVarargsArguments; i++)
				{
					varargs[i] = argiter;
					argarr[i + 1] = argiter;
					argiter++;
				}

				for (auto& meth : Methods)
				{
					if (NomNameRepository::Instance().GetNameID(meth->GetName()) % IMTsize == i)
					{
						BasicBlock* callBlock = BasicBlock::Create(LLVMCONTEXT, meth->GetName(), fun);
						BasicBlock* nextBlock = BasicBlock::Create(LLVMCONTEXT, "next", fun);

						auto methodCallTag = NomRecordCallTag::GetCallTag(meth->GetName(), meth->GetDirectTypeParametersCount(), meth->GetArgumentCount());

						auto callTagMatch = builder->CreateICmpEQ(builder->CreatePtrToInt(callTag, numtype(intptr_t)), ConstantExpr::getPtrToInt(methodCallTag->GetLLVMElement(mod), numtype(intptr_t)), "callTagMatch");
						builder->CreateIntrinsic(Intrinsic::expect, { inttype(1) }, { callTagMatch, MakeUInt(1,1) });
						builder->CreateCondBr(callTagMatch, callBlock, nextBlock, GetLikelyFirstBranchMetadata());
						builder->SetInsertPoint(callBlock);

						auto implFunctionType = meth->GetLLVMFunctionType();
						auto paramCount = implFunctionType->getNumParams();
						auto argsarr = makealloca(Value*, paramCount);		

						NomSubstitutionContextMemberContext nscmc(meth);
						CastedValueCompileEnv cvce = CastedValueCompileEnv(meth->GetDirectTypeParameters(), this->GetAllTypeParameters(), fun, 2, paramCount, PWObject(varargs[0]).PointerToTypeArguments(builder));

						for (decltype(paramCount) j = 0; j < paramCount; j++)
						{
							Value* curArg = nullptr;
							if (j < RTConfig_NumberOfVarargsArguments || RTConfig_NumberOfVarargsArguments + 1 == paramCount)
							{
								curArg = varargs[j];
							}
							else
							{
								curArg = MakeInvariantLoad(builder, POINTERTYPE, builder->CreateGEP(POINTERTYPE, varargs[RTConfig_NumberOfVarargsArguments], MakeInt32(j - RTConfig_NumberOfVarargsArguments)), "varArg", AtomicOrdering::NotAtomic);
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

						builder->SetInsertPoint(nextBlock);
					}
				}

				auto callTagFun = builder->CreatePointerCast(callTag, GetIMTCastFunctionType()->getPointerTo());
				auto tagCall = builder->CreateCall(GetIMTCastFunctionType(), callTagFun, ArrayRef<Value*>(argarr, 2 + RTConfig_NumberOfVarargsArguments));
				tagCall->setCallingConv(NOMCC);
				tagCall->setTailCallKind(CallInst::TailCallKind::TCK_MustTail);
				builder->CreateRet(tagCall);

				llvm::raw_os_ostream out(std::cout);
				if (verifyFunction(*fun, &out))
				{
					out.flush();
					std::cout << "Could not verify record IMT function";
					fun->print(out);
					out.flush();
					std::cout.flush();
					throw new std::exception();
				}

				retarr[i] = fun;
			}

			return ConstantArray::get(rettype, ArrayRef<Constant*>(retarr, IMTsize));
		}
		llvm::Constant* NomRecord::GetDynamicDispatcherLookup(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage, llvm::StructType* stype) const
		{
			auto thisType = NomDynamicType::RecordInstance();
			auto rettype = arrtype(GetDynamicDispatchListEntryType()->getPointerTo(), IMTsize);
			auto retarr = makealloca(Constant*, IMTsize);
			auto typesarr = makealloca(Type*, IMTsize);

			for (decltype(IMTsize) i = 0; i < IMTsize; i++)
			{
				llvm::SmallVector<const NomRecordMethod*, 16> methods;
				llvm::SmallVector<const NomRecordField*, 16> fields;
				for (auto& meth : Methods)
				{
					if (NomNameRepository::Instance().GetNameID(meth->GetName()) % IMTsize == i)
					{
						methods.push_back(meth);
					}
				}

				for (auto field : Fields)
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
					std::string* errormsg = new std::string("Called record method " + meth->GetName() + " with invalid number of arguments!");
					builder->SetInsertPoint(startBlock);
					BasicBlock* wrongArgumentCountBlock = RTOutput_Fail::GenerateFailOutputBlock(builder, errormsg->c_str());

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

					for (decltype(paramCount) j = 0; j < paramCount; j++)
					{
						Value* curArg = nullptr;
						if (j < RTConfig_NumberOfVarargsArguments || RTConfig_NumberOfVarargsArguments + 1 == paramCount)
						{
							curArg = varargs[j];
						}
						else
						{
							curArg = MakeInvariantLoad(builder, POINTERTYPE, builder->CreateGEP(POINTERTYPE, varargs[RTConfig_NumberOfVarargsArguments], MakeInt32(j - RTConfig_NumberOfVarargsArguments)), "varArg", AtomicOrdering::NotAtomic);
						}
						auto expectedType = implFunctionType->getParamType(j);
						curArg = EnsurePackedUnpacked(builder, curArg, REFTYPE);
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
				auto bigptr = ConstantExpr::getGetElementPtr(RecordHeader::GetLLVMType(Fields.size()), ConstantPointerNull::get(RecordHeader::GetLLVMType(Fields.size())->getPointerTo()), ArrayRef<Constant*>({ MakeInt32(0), MakeInt32(StructHeaderFields::Fields) }));
				auto littleptr = ConstantExpr::getGetElementPtr(RecordHeader::GetLLVMType(), ConstantPointerNull::get(RecordHeader::GetLLVMType()->getPointerTo()), ArrayRef<Constant*>({ MakeInt32(0), MakeInt32(StructHeaderFields::Fields) }));
				auto fieldsOffset = ConstantFoldBinaryInstruction(BinaryOperator::UDiv, ConstantExpr::getSub(ConstantExpr::getPtrToInt(bigptr, numtype(size_t)), ConstantExpr::getPtrToInt(littleptr, numtype(size_t))), MakeInt<size_t>(8));
				for (auto& field : fields)
				{
					entries[entryID] = GetDynamicDispatchListEntryConstant(MakeInt<size_t>(NomNameRepository::Instance().GetNameID(field->GetName()->ToStdString())), MakeInt<size_t>(1), ConstantExpr::getIntToPtr(ConstantExpr::getAdd(MakeInt<size_t>((((size_t)(field->Index)) << 32)), fieldsOffset), GetIMTFunctionType()->getPointerTo()));
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

		bool NomRecord::GetHasRawInvoke() const
		{
			return NomLambdaOptimizationLevel > 0;
		}
		llvm::FunctionType* NomRecord::GetLLVMFunctionType(const NomSubstitutionContext* context) const
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
		llvm::ArrayRef<NomTypeParameterRef> NomRecord::GetArgumentTypeParameters() const
		{
			return GetAllTypeParameters();
		}
		inline void NomRecord::PushDependencies(std::set<ConstantID>& set) const
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