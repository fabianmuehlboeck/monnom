#include "NomMethodKey.h"
#include <unordered_map>
#include "NomMethod.h"
#include "NomNameRepository.h"
#include "IMT.h"
#include "CompileHelpers.h"
#include "CallingConvConf.h"
#include "NomPartialApplication.h"
#include "NomStruct.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/raw_os_ostream.h"
#include <iostream>
#include "RTCast.h"
#include "RTOutput.h"
#include "RTCompileConfig.h"
#include "StructuralValueHeader.h"
#include "RTSTable.h"
#include "NomInterface.h"
#include "RTInterface.h"
#include "RTVTable.h"

using namespace std;
using namespace llvm;
namespace Nom
{
	namespace Runtime
	{
		NomMethodKey::NomMethodKey(std::string&& key, const NomMethod* method, DICTKEYTYPE&& name, llvm::SmallVector<TypeReferenceType, 8>&& argTRTs) :
			method(method), name(name)
		{
			this->key = key;
			this->argTRTs = argTRTs;
		}
		NomMethodKey* NomMethodKey::GetMethodKey(const NomMethod* method)
		{
			static unordered_map<string, NomMethodKey*> methodKeys;

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
				NomMethodKey* nmk = new NomMethodKey(std::move(key), method, std::move(NomNameRepository::Instance().GetNameID(method->GetName())), std::move(argTRTs));
				methodKeys[nmk->key] = nmk;
				return nmk;
			}
		}
		llvm::Function* NomMethodKey::createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const
		{
			Function* fun = Function::Create(GetIMTCastFunctionType(), linkage, "RT_NOM_MK_" + key, mod);
			fun->setCallingConv(NOMCC);
			BasicBlock* block = BasicBlock::Create(LLVMCONTEXT, "", fun);

			auto returnType = method->GetReturnType(nullptr);
			auto targcount = method->GetDirectTypeParametersCount();
			auto argcount = targcount + argTRTs.size();
			Value** argbuf = makealloca(Value*, 3 + RTConfig_NumberOfVarargsArguments);
			argbuf[0] = MakeInt32(targcount);
			argbuf[1] = MakeInt32(argTRTs.size());
			NomBuilder builder;
			builder->SetInsertPoint(block);
			//auto localTypeArr = builder->CreateGEP(builder->CreateAlloca(TYPETYPE, targcount), MakeInt32(targcount));

			auto fargs = fun->arg_begin();
			fargs++; //ignore first argument, which is just this function
			argbuf[2] = builder->CreatePointerCast(fargs, REFTYPE); //receiver; we know this is not a primitive value, as such a value would never call the method key
			fargs++;
			for (decltype(RTConfig_NumberOfVarargsArguments) i = 0; i < RTConfig_NumberOfVarargsArguments; i++)
			{
				argbuf[i + 3] = fargs;
				fargs++;
			}
			size_t argbufpos = 0;
			while (argbufpos < argcount)
			{
				if (argbufpos >= targcount)
				{
					if (argTRTs[argbufpos - targcount] != TypeReferenceType::Reference)
					{
						Value* curArg;
						if (argbufpos < RTConfig_NumberOfVarargsArguments - 1 || argcount <= RTConfig_NumberOfVarargsArguments)
						{
							curArg = argbuf[argbufpos + 3];
						}
						else
						{
							curArg = MakeInvariantLoad(builder, builder->CreateGEP(builder->CreatePointerCast(fargs, fargs->getType()->getPointerTo()), MakeInt32(argbufpos - (RTConfig_NumberOfVarargsArguments - 1))));
						}
						switch (argTRTs[argbufpos - targcount])
						{
						case TypeReferenceType::UnpackedInteger:
							curArg = PackInt(builder, builder->CreatePtrToInt(curArg, INTTYPE));
							break;
						case TypeReferenceType::UnpackedFloat:
							curArg = PackFloat(builder, builder->CreateBitCast(builder->CreatePtrToInt(curArg, INTTYPE), FLOATTYPE));
							break;
						case TypeReferenceType::UnpackedBool:
							curArg = PackBool(builder, builder->CreatePtrToInt(curArg, BOOLTYPE));
							break;
						default:
							throw new std::exception();
						}
						if (argbufpos < RTConfig_NumberOfVarargsArguments - 1 || argcount <= RTConfig_NumberOfVarargsArguments)
						{
							argbuf[argbufpos + 3] = curArg;
						}
						else
						{
							MakeStore(builder, curArg, builder->CreateGEP(builder->CreatePointerCast(fargs, fargs->getType()->getPointerTo()), MakeInt32(argbufpos - (RTConfig_NumberOfVarargsArguments - 1))));
						}
					}
				}
				argbufpos++;
			}
			//	//if (argbufpos>0 && argbufpos <= targcount)
			//	//{
			//	//	MakeStore(builder, builder->CreatePointerCast(argbuf[argbufpos], TYPETYPE), builder->CreateGEP(localTypeArr, MakeInt32(-(argbufpos + 1))));
			//	//}
			//	if (argbufpos == 0)
			//	{
			//		argbuf[argbufpos] = builder->CreatePointerCast(argbuf[argbufpos], REFTYPE);
			//	}
			//	if (argbufpos > targcount)
			//	{
			//		switch (argTRTs[argbufpos - targcount - 1])
			//		{
			//		case TypeReferenceType::Reference:
			//			argbuf[argbufpos] = builder->CreatePointerCast(argbuf[argbufpos], REFTYPE);
			//			break;
			//		case TypeReferenceType::UnpackedInteger:
			//			argbuf[argbufpos] = PackInt(builder, builder->CreatePtrToInt(argbuf[argbufpos], INTTYPE));
			//			break;
			//		case TypeReferenceType::UnpackedFloat:
			//			argbuf[argbufpos] = PackFloat(builder, builder->CreateBitCast(builder->CreatePtrToInt(argbuf[argbufpos], INTTYPE), FLOATTYPE));
			//			break;
			//		case TypeReferenceType::UnpackedBool:
			//			argbuf[argbufpos] = PackBool(builder, builder->CreatePtrToInt(argbuf[argbufpos], BOOLTYPE));
			//			break;
			//		}
			//	}
			//	argbufpos++;
			//}
			auto structDesc = StructuralValueHeader::GenerateReadSTablePtr(builder, argbuf[2]);
			auto structDispatcherLookupPtr = RTStruct::GenerateReadDispatcherLookup(builder, structDesc); //this can't be a lambda or partial application because that cast would have failed
			auto structDispatcher = builder->CreateCall(NomStruct::GetDynamicDispatcherLookupType(), structDispatcherLookupPtr, { argbuf[2], MakeInt<DICTKEYTYPE>(name)/*, MakeInt<int32_t>(targcount), MakeInt<int32_t>(argTRTs.size())*/ });
			structDispatcher->setCallingConv(NOMCC);

			argbuf[2] = builder->CreateExtractValue(structDispatcher, { 1 });

			llvm::CallInst* structDispatcherCallInst = builder->CreateCall(NomPartialApplication::GetDynamicDispatcherType(/*targcount, argTRTs.size()*/), /*builder->CreatePointerCast(*/builder->CreateExtractValue(structDispatcher, { 0 })/*, NomPartialApplication::GetDynamicDispatcherType(targcount, argTRTs.size())->getPointerTo())*/, llvm::ArrayRef<Value*>(argbuf, 3 + RTConfig_NumberOfVarargsArguments), key + "()");
			structDispatcherCallInst->setCallingConv(NOMCC);

			auto structCastType = StructuralValueHeader::GenerateReadCastData(builder, argbuf[2]);
			auto structCastTypeAsInt = builder->CreatePtrToInt(structCastType, numtype(intptr_t));
			auto structCastTypeTag = builder->CreateTrunc(structCastTypeAsInt, inttype(1));

			static const char* unimplemented_msg = "UNIMPLEMENTED!";
			BasicBlock* multiCastListBlock = RTOutput_Fail::GenerateFailOutputBlock(builder, unimplemented_msg);
			static const char* castfail_msg = "Cast failed!";
			BasicBlock* castFailBlock = RTOutput_Fail::GenerateFailOutputBlock(builder, castfail_msg);
			BasicBlock* singleCastBlock = BasicBlock::Create(LLVMCONTEXT, "singleCastCast", fun);
			BasicBlock* outBlock = BasicBlock::Create(LLVMCONTEXT, "out", fun);

			builder->CreateCondBr(structCastTypeTag, multiCastListBlock, singleCastBlock);
			{
				builder->SetInsertPoint(singleCastBlock);

				BasicBlock* bestMatchBlock = BasicBlock::Create(LLVMCONTEXT, "castTypePerfectMatch", fun);
				BasicBlock* mismatchBlock = BasicBlock::Create(LLVMCONTEXT, "castTypeMismatch", fun);

				auto iface = RTClassType::GenerateReadClassDescriptorLink(builder, structCastType);
				auto ifaceMatch = CreatePointerEq(builder, iface, method->GetContainer()->GetLLVMElement(mod));
				builder->CreateCondBr(ifaceMatch, bestMatchBlock, mismatchBlock);

				builder->SetInsertPoint(bestMatchBlock);
				{
					CastedValueCompileEnv cvce = CastedValueCompileEnv(method->GetDirectTypeParameters(), method->GetParent()->GetAllTypeParameters(), fun, 2, argTRTs.size(), builder->CreatePointerCast(builder->CreateGEP(builder->CreatePointerCast(structCastType, RTClassType::GetLLVMPointerType()), { MakeInt32(0), MakeInt32(RTClassTypeFields::TypeArgs) }), TYPETYPE->getPointerTo()));
					auto castResult = RTCast::GenerateCast(builder, &cvce, structDispatcherCallInst, method->GetReturnType());
					builder->CreateCondBr(castResult, outBlock, castFailBlock);
				}

				{
					builder->SetInsertPoint(mismatchBlock);
					auto typeArgRefStore = builder->CreateAlloca(TYPETYPE->getPointerTo(), MakeUInt(64, 1));
					builder->CreateIntrinsic(Intrinsic::lifetime_start, { POINTERTYPE }, { MakeInt<int64_t>(GetNomJITDataLayout().getTypeAllocSize(TYPETYPE->getPointerTo())), builder->CreatePointerCast(typeArgRefStore, POINTERTYPE) });
					MakeInvariantStore(builder, builder->CreatePointerCast(builder->CreateGEP(builder->CreatePointerCast(structCastType, RTClassType::GetLLVMPointerType()), { MakeInt32(0), MakeInt32(RTClassTypeFields::TypeArgs) }), TYPETYPE->getPointerTo()), typeArgRefStore, AtomicOrdering::NotAtomic);
					auto invariantID =	builder->CreateIntrinsic(Intrinsic::invariant_start, { POINTERTYPE }, { MakeInt<int64_t>(GetNomJITDataLayout().getTypeAllocSize(TYPETYPE->getPointerTo())), builder->CreatePointerCast(typeArgRefStore, POINTERTYPE) });
					argbuf[0] = builder->CreatePointerCast(fun, POINTERTYPE);
					argbuf[1] = typeArgRefStore;
					argbuf[2] = structDispatcherCallInst;
					auto rvcf = RTVTable::GenerateReadReturnValueCheckFunction(builder, iface);
					builder->CreateCall(GetCheckReturnValueFunctionType(), rvcf, llvm::ArrayRef<Value*>(argbuf, 3 + RTConfig_NumberOfVarargsArguments))->setCallingConv(NOMCC);

					builder->CreateIntrinsic(llvm::Intrinsic::invariant_end, { POINTERTYPE }, { invariantID, MakeInt<int64_t>(GetNomJITDataLayout().getTypeAllocSize(TYPETYPE->getPointerTo())), builder->CreatePointerCast(typeArgRefStore, POINTERTYPE) });
					builder->CreateIntrinsic(llvm::Intrinsic::lifetime_end, { POINTERTYPE }, { MakeInt<int64_t>(GetNomJITDataLayout().getTypeAllocSize(TYPETYPE->getPointerTo())), builder->CreatePointerCast(typeArgRefStore, POINTERTYPE) });
					builder->CreateBr(outBlock);
				}
			}

			//BasicBlock* castSuccessBlock = BasicBlock::Create(LLVMCONTEXT, "castSuccess", fun);
			//std::string* errormsg = new std::string("Expected a " + returnType->GetSymbolRep());
			//BasicBlock* castFailBlock = RTOutput_Fail::GenerateFailOutputBlock(builder, errormsg->c_str());

			//CastedValueCompileEnv cvce = CastedValueCompileEnv(method->GetDirectTypeParameters(), method->GetParent()->GetAllTypeParameters(), localTypeArr, instanceTypeArr);
			//Value* castReturnValue = RTCast::GenerateCast(builder, &cvce, structDispatcherCallInst, returnType);
			//builder->CreateCondBr(castReturnValue, castSuccessBlock, castFailBlock);

			builder->SetInsertPoint(outBlock);
			//builder->SetInsertPoint(castSuccessBlock);
			llvm::Value* retval = structDispatcherCallInst;
			switch (returnType->GetTypeReferenceType())
			{
			case TypeReferenceType::Reference:
				retval = builder->CreatePointerCast(retval, POINTERTYPE);
				break;
			case TypeReferenceType::UnpackedInteger:
				retval = builder->CreateIntToPtr(UnpackInt(builder, retval), POINTERTYPE);
				break;
			case TypeReferenceType::UnpackedFloat:
				retval = builder->CreateIntToPtr(builder->CreateBitCast(UnpackFloat(builder, retval), numtype(intptr_t)), POINTERTYPE);
				break;
			case TypeReferenceType::UnpackedBool:
				retval = builder->CreateIntToPtr(UnpackBool(builder, retval), POINTERTYPE);
				break;
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
		llvm::Function* NomMethodKey::findLLVMElement(llvm::Module& mod) const
		{
			return mod.getFunction("RT_NOM_MK_" + key);
		}
	}
}