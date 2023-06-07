#include "NomInterfaceCallTag.h"
#include <unordered_map>
#include "NomMethod.h"
#include "NomNameRepository.h"
#include "IMT.h"
#include "CompileHelpers.h"
#include "CallingConvConf.h"
#include "NomPartialApplication.h"
#include "NomRecord.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/raw_os_ostream.h"
#include <iostream>
#include "RTCast.h"
#include "RTOutput.h"
#include "RTCompileConfig.h"
#include "StructuralValueHeader.h"
#include "NomInterface.h"
#include "RTInterface.h"
#include "RTVTable.h"
#include "NomRecordCallTag.h"
#include "RTClassType.h"
#include "RefValueHeader.h"
#include "NomLambdaCallTag.h"
#include "Metadata.h"
#include "CastStats.h"
#include "NomClassType.h"

using namespace std;
using namespace llvm;
namespace Nom
{
	namespace Runtime
	{
		NomInterfaceCallTag::NomInterfaceCallTag(std::string&& key, const NomMethod* method, DICTKEYTYPE&& name, llvm::SmallVector<TypeReferenceType, 8>&& argTRTs) :
			method(method), name(name)
		{
			this->key = key;
			this->argTRTs = argTRTs;
		}
		NomInterfaceCallTag* NomInterfaceCallTag::GetMethodKey(const NomMethod* method)
		{
			static unordered_map<string, NomInterfaceCallTag*> methodKeys;

			std::string key = *method->GetSymbolName();
			key.append("/");
			key.append(std::to_string(method->GetDirectTypeParametersCount()));
			key.append("/");
			TypeList args = method->GetArgumentTypes(nullptr);
			llvm::SmallVector<TypeReferenceType, 8> argTRTs;
			for (auto arg : args)
			{
				key.append(arg->GetSymbolRep());
				argTRTs.push_back(arg->GetTypeReferenceType());
			}
			key.append(method->GetReturnType(nullptr)->GetSymbolRep());

			auto foundElem = methodKeys.find(key);
			if (foundElem != methodKeys.end())
			{
				return (foundElem->second);
			}
			else
			{
				NomInterfaceCallTag* nmk = new NomInterfaceCallTag(std::move(key), method, std::move(NomNameRepository::Instance().GetNameID(method->GetName())), std::move(argTRTs));
				methodKeys[nmk->key] = nmk;
				return nmk;
			}
		}
		llvm::Function* NomInterfaceCallTag::createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const
		{
			Function* fun = Function::Create(GetIMTCastFunctionType(), linkage, "MONNOM_RT_ICT_" + key, mod);
			fun->setCallingConv(NOMCC);
			BasicBlock* block = BasicBlock::Create(LLVMCONTEXT, "", fun);

			auto returnType = method->GetReturnType(nullptr);
			auto targcount = method->GetDirectTypeParametersCount();
			auto argcount = targcount + argTRTs.size();
			Value** argbuf = makealloca(Value*, 3 + RTConfig_NumberOfVarargsArguments);
			NomBuilder builder;
			builder->SetInsertPoint(block);
			argbuf++;

			auto fargs = fun->arg_begin();
			fargs++; //ignore first argument, which is just this function
			argbuf[1] = fargs; //receiver; we know this is not a primitive value, as such a value would never call the method key
			fargs++;
			for (decltype(RTConfig_NumberOfVarargsArguments) i = 0; i < RTConfig_NumberOfVarargsArguments; i++)
			{
				argbuf[i + 2] = fargs;
				fargs++;
			}

			Value** varargs = argbuf + 2;
			size_t argbufpos = 0;

			auto calledFunctionType = method->GetLLVMFunctionType();
			auto paramCount = calledFunctionType->getNumParams();

			for (decltype(paramCount) j = 0; j < paramCount; j++)
			{
				Value* curArg = nullptr;
				auto calledType = calledFunctionType->getParamType(j);
				if (calledType != REFTYPE && calledType != TYPETYPE)
				{
					if (j < RTConfig_NumberOfVarargsArguments || RTConfig_NumberOfVarargsArguments + 1 == paramCount)
					{
						curArg = varargs[j];
					}
					else
					{
						curArg = MakeLoad(builder, POINTERTYPE, builder->CreateGEP(arrtype(POINTERTYPE,0), varargs[RTConfig_NumberOfVarargsArguments], MakeInt32(j - RTConfig_NumberOfVarargsArguments)), "varArg", AtomicOrdering::NotAtomic);
					}

					//curArg = EnsurePackedUnpacked(builder, curArg, REFTYPE);
					curArg = EnsurePackedUnpacked(builder, curArg, calledType);
					curArg = EnsurePackedUnpacked(builder, curArg, REFTYPE);
					curArg = EnsurePackedUnpacked(builder, curArg, POINTERTYPE);
					if (j < RTConfig_NumberOfVarargsArguments || RTConfig_NumberOfVarargsArguments + 1 == paramCount)
					{
						varargs[j] = curArg;
					}
					else
					{
						MakeStore(builder, curArg, builder->CreateGEP(arrtype(POINTERTYPE,0), varargs[RTConfig_NumberOfVarargsArguments], MakeInt32(j - RTConfig_NumberOfVarargsArguments)), AtomicOrdering::NotAtomic);
					}
				}
			}
			Value* actualResult = nullptr;
			if (method->GetName().empty() && NomLambdaOptimizationLevel > 0)
			{
				argbuf[0] = builder->CreatePointerCast(NomLambdaCallTag::GetCallTag(targcount, argTRTs.size())->GetLLVMElement(mod), POINTERTYPE);
				auto callResult = builder->CreateCall(GetIMTFunctionType(), builder->CreatePointerCast(fun->arg_begin(), GetIMTFunctionType()->getPointerTo()), ArrayRef<Value*>(argbuf, 2 + RTConfig_NumberOfVarargsArguments), method->GetQName());
				callResult->setCallingConv(NOMCC);
				actualResult = EnsurePackedUnpacked(builder, callResult, REFTYPE);
			}
			else
			{
				argbuf[0] = builder->CreatePointerCast(NomRecordCallTag::GetCallTag(this->method->GetName(), targcount, argTRTs.size())->GetLLVMElement(mod), POINTERTYPE);
				auto receiver = builder->CreatePointerCast(argbuf[1], REFTYPE);
				auto vtable = RefValueHeader::GenerateReadVTablePointer(builder, receiver);
				auto dispatcher = RTVTable::GenerateReadInterfaceMethodTableEntry(builder, vtable, MakeInt32(name % IMTsize));
				auto callResult = builder->CreateCall(GetIMTFunctionType(), dispatcher, ArrayRef<Value*>(argbuf, 2 + RTConfig_NumberOfVarargsArguments), method->GetQName());
				callResult->setCallingConv(NOMCC);
				actualResult = EnsurePackedUnpacked(builder, callResult, REFTYPE);
			}
			llvm::Value* retval = nullptr;
			bool simpleReturnType = false;
			{
				auto actualReturnType = method->GetReturnType();
				if (actualReturnType->GetKind() == TypeKind::TKClass)
				{
					auto classReturnType = (NomClassTypeRef)actualReturnType;
					if (!classReturnType->Named->IsInterface()&&!classReturnType->ContainsVariables())
					{
						simpleReturnType = true;
					}
				}
				if (actualReturnType->GetKind() == TypeKind::TKVariable)
				{
					simpleReturnType = true;
				}
			}
			if ((!RTConfig_OmitCallTagCasts)||simpleReturnType)
			{
				auto structCastType = StructuralValueHeader::GenerateReadCastData(builder, argbuf[1]);
				auto structCastTypeAsInt = builder->CreatePtrToInt(structCastType, numtype(intptr_t));
				auto structCastTypeTag = builder->CreateTrunc(structCastTypeAsInt, inttype(1));
				builder->CreateIntrinsic(Intrinsic::expect, { inttype(1) }, { structCastTypeTag, MakeUInt(1,0) });
				static const char* unimplemented_msg = "UNIMPLEMENTED!";
				BasicBlock* multiCastListBlock = RTOutput_Fail::GenerateFailOutputBlock(builder, unimplemented_msg);
				static const char* castfail_msg = "Cast failed!";
				BasicBlock* castFailBlock = RTOutput_Fail::GenerateFailOutputBlock(builder, castfail_msg);
				BasicBlock* singleCastBlock = BasicBlock::Create(LLVMCONTEXT, "singleCastCast", fun);
				BasicBlock* outBlock = BasicBlock::Create(LLVMCONTEXT, "out", fun);
				BasicBlock* curBlock = builder->GetInsertBlock();
				builder->SetInsertPoint(outBlock);
				auto outPHI = builder->CreatePHI(actualResult->getType(), 2);
				builder->SetInsertPoint(curBlock);

				builder->CreateCondBr(structCastTypeTag, multiCastListBlock, singleCastBlock, GetLikelySecondBranchMetadata());
				{
					builder->SetInsertPoint(singleCastBlock);

					BasicBlock* bestMatchBlock = BasicBlock::Create(LLVMCONTEXT, "castTypePerfectMatch", fun);
					BasicBlock* mismatchBlock = BasicBlock::Create(LLVMCONTEXT, "castTypeMismatch", fun);

					auto iface = RTClassType::GenerateReadClassDescriptorLink(builder, structCastType);
					auto ifaceMatch = CreatePointerEq(builder, iface, method->GetContainer()->GetLLVMElement(mod));
					builder->CreateIntrinsic(Intrinsic::expect, { inttype(1) }, { ifaceMatch, MakeUInt(1,1) });
					builder->CreateCondBr(ifaceMatch, bestMatchBlock, mismatchBlock, GetLikelyFirstBranchMetadata());

					builder->SetInsertPoint(bestMatchBlock);
					{
						if (NomCastStats)
						{
							builder->CreateCall(GetIncPerfectCallTagTypeMatchesFunction(*fun->getParent()), {});
						}
						CastedValueCompileEnv cvce = CastedValueCompileEnv(method->GetDirectTypeParameters(), method->GetParent()->GetAllTypeParameters(), fun, 2, argTRTs.size(), builder->CreateGEP(RTClassType::GetLLVMType(), structCastType, { MakeInt32(0), MakeInt32(RTClassTypeFields::TypeArgs) }));
						auto castResult = RTCast::GenerateCast(builder, &cvce, actualResult, method->GetReturnType());
						//builder->CreateIntrinsic(Intrinsic::expect, { inttype(1) }, { castResult, MakeUInt(1,1) });
						//builder->CreateCondBr(castResult, outBlock, castFailBlock, GetLikelyFirstBranchMetadata());
						outPHI->addIncoming(castResult, builder->GetInsertBlock());
						builder->CreateBr(outBlock);
					}

					{
						builder->SetInsertPoint(mismatchBlock);
						if (NomCastStats)
						{
							builder->CreateCall(GetIncCallTagTypeMismatchesFunction(*fun->getParent()), {});
						}
						auto typeArgRefStore = builder->CreateAlloca(NLLVMPointer(TYPETYPE), MakeUInt(64, 1));
						builder->CreateIntrinsic(Intrinsic::lifetime_start, { POINTERTYPE }, { MakeInt<int64_t>(GetNomJITDataLayout().getTypeAllocSize(NLLVMPointer(TYPETYPE))), builder->CreatePointerCast(typeArgRefStore, POINTERTYPE) });
						MakeInvariantStore(builder, builder->CreateGEP(RTClassType::GetLLVMType(), structCastType, { MakeInt32(0), MakeInt32(RTClassTypeFields::TypeArgs) }), typeArgRefStore, AtomicOrdering::NotAtomic);
						auto invariantID = builder->CreateIntrinsic(Intrinsic::invariant_start, { POINTERTYPE }, { MakeInt<int64_t>(GetNomJITDataLayout().getTypeAllocSize(NLLVMPointer(TYPETYPE))), builder->CreatePointerCast(typeArgRefStore, POINTERTYPE) });
						argbuf--;
						argbuf[0] = builder->CreatePointerCast(fun, POINTERTYPE);
						argbuf[1] = typeArgRefStore;
						argbuf[2] = actualResult;
						auto rvcf = RTInterface::GenerateReadReturnValueCheckFunction(builder, iface);
						builder->CreateCall(GetCheckReturnValueFunctionType(), rvcf, llvm::ArrayRef<Value*>(argbuf, 3 + RTConfig_NumberOfVarargsArguments))->setCallingConv(NOMCC);

						builder->CreateIntrinsic(llvm::Intrinsic::invariant_end, { POINTERTYPE }, { invariantID, MakeInt<int64_t>(GetNomJITDataLayout().getTypeAllocSize(NLLVMPointer(TYPETYPE))), builder->CreatePointerCast(typeArgRefStore, POINTERTYPE) });
						builder->CreateIntrinsic(llvm::Intrinsic::lifetime_end, { POINTERTYPE }, { MakeInt<int64_t>(GetNomJITDataLayout().getTypeAllocSize(NLLVMPointer(TYPETYPE))), builder->CreatePointerCast(typeArgRefStore, POINTERTYPE) });
						outPHI->addIncoming(actualResult, builder->GetInsertBlock());
						builder->CreateBr(outBlock);
					}
				}

				builder->SetInsertPoint(outBlock);
				retval = EnsurePackedUnpacked(builder, EnsurePackedUnpacked(builder, outPHI, calledFunctionType->getReturnType()), POINTERTYPE);
			}
			else
			{
				retval = EnsurePackedUnpacked(builder, EnsurePackedUnpacked(builder, actualResult, calledFunctionType->getReturnType()), POINTERTYPE);
			}
			builder->CreateRet(retval);

			llvm::raw_os_ostream out(std::cout);
			//For some reason, verifyFunction is supposed to return false if there are no problems
			if (verifyFunction(*fun, &out))
			{
				out.flush();
				std::cout << "Could not verify method key method!";
				fun->print(out);
				out.flush();
				std::cout.flush();
				throw name;
			}
			return fun;
		}
		llvm::Function* NomInterfaceCallTag::findLLVMElement(llvm::Module& mod) const
		{
			return mod.getFunction("MONNOM_RT_ICT_" + key);
		}
	}
}