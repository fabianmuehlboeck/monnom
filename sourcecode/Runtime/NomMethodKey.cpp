#include "NomMethodKey.h"
#include <unordered_map>
#include "NomMethod.h"
#include "NomNameRepository.h"
#include "IMT.h"
#include "StructHeader.h"
#include "CompileHelpers.h"
#include "CallingConvConf.h"
#include "NomPartialApplication.h"
#include "NomStruct.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/raw_os_ostream.h"
#include <iostream>
#include "RTCast.h"
#include "RTOutput.h"

using namespace std;
using namespace llvm;
namespace Nom
{
	namespace Runtime
	{
		NomMethodKey::NomMethodKey(std::string&& key, const NomMethod *method, DICTKEYTYPE&& name, llvm::SmallVector<TypeReferenceType, 8>&& argTRTs) :
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
			Value** argbuf = makealloca(Value*, argcount+1);
			NomBuilder builder;
			builder->SetInsertPoint(block);
			auto localTypeArr = builder->CreateGEP(builder->CreateAlloca(TYPETYPE, targcount), MakeInt32(targcount));

			auto fargs = fun->arg_begin();
			Value* instanceTypeArr = fargs;
			fargs++;
			size_t argbufpos = 0;
			while (argbufpos < argcount+1)
			{
				if (argbufpos < 3 || argcount <= 3)
				{
					argbuf[argbufpos] = fargs;
					fargs++;
				}
				else
				{
					argbuf[argbufpos] = MakeLoad(builder, builder->CreateGEP(builder->CreatePointerCast(fargs, fargs->getType()->getPointerTo()), MakeInt32(argbufpos - 3)));
				}
				if (argbufpos>0 && argbufpos <= targcount)
				{
					MakeStore(builder, builder->CreatePointerCast(argbuf[argbufpos], TYPETYPE), builder->CreateGEP(localTypeArr, MakeInt32(-(argbufpos + 1))));
				}
				if (argbufpos == 0)
				{
					argbuf[argbufpos] = builder->CreatePointerCast(argbuf[argbufpos], REFTYPE);
				}
				if (argbufpos > targcount)
				{
					switch (argTRTs[argbufpos - targcount - 1])
					{
					case TypeReferenceType::Reference:
						argbuf[argbufpos] = builder->CreatePointerCast(argbuf[argbufpos], REFTYPE);
						break;
					case TypeReferenceType::UnpackedInteger:
						argbuf[argbufpos] = PackInt(builder, builder->CreatePtrToInt(argbuf[argbufpos], INTTYPE));
						break;
					case TypeReferenceType::UnpackedFloat:
						argbuf[argbufpos] = PackFloat(builder, builder->CreateBitCast(builder->CreatePtrToInt(argbuf[argbufpos], INTTYPE), FLOATTYPE));
						break;
					case TypeReferenceType::UnpackedBool:
						argbuf[argbufpos] = PackBool(builder, builder->CreatePtrToInt(argbuf[argbufpos], BOOLTYPE));
						break;
					}
				}
				argbufpos++;
			}
			auto structDesc = StructHeader::GenerateReadStructDescriptor(builder, argbuf[0]);
			auto structDispatcherLookupPtr = RTStruct::GenerateReadDispatcherLookup(builder, structDesc);
			auto structDispatcher = builder->CreateCall(NomStruct::GetDynamicDispatcherLookupType(), structDispatcherLookupPtr, { argbuf[0], MakeInt<DICTKEYTYPE>(name), MakeInt<int32_t>(targcount), MakeInt<int32_t>(argTRTs.size()) });
			structDispatcher->setCallingConv(NOMCC);

			argbuf[0] = builder->CreateExtractValue(structDispatcher, { 1 });

			llvm::CallInst* structDispatcherCallInst = builder->CreateCall(NomPartialApplication::GetDynamicDispatcherType(targcount, argTRTs.size()), builder->CreatePointerCast(builder->CreateExtractValue(structDispatcher, { 0 }), NomPartialApplication::GetDynamicDispatcherType(targcount, argTRTs.size())->getPointerTo()), llvm::ArrayRef<Value*>(argbuf, argcount+1), key + "()");
			structDispatcherCallInst->setCallingConv(NOMCC);


			BasicBlock* castSuccessBlock = BasicBlock::Create(LLVMCONTEXT, "castSuccess", fun);
			std::string* errormsg = new std::string("Expected a " + returnType->GetSymbolRep());
			BasicBlock* castFailBlock = RTOutput_Fail::GenerateFailOutputBlock(builder, errormsg->c_str());

			CastedValueCompileEnv cvce = CastedValueCompileEnv(method->GetDirectTypeParameters(), method->GetParent()->GetAllTypeParameters(), localTypeArr, instanceTypeArr);
			Value* castReturnValue = RTCast::GenerateCast(builder, &cvce, structDispatcherCallInst, returnType);
			builder->CreateCondBr(castReturnValue, castSuccessBlock, castFailBlock);

			builder->SetInsertPoint(castSuccessBlock);
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