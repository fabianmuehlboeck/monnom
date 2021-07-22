#include "IMT.h"
#include "Defs.h"
#include "llvm/IR/IRBuilder.h"
#include "Context.h"
//#include "llvm/ADT/VariadicFunction.h"
#include <map>
#include <vector>
#include "NomMethodTableEntry.h"
#include "NomInterface.h"
#include "CompileHelpers.h"
#include "NomCallableVersion.h"
#include "CallingConvConf.h"
#include <iostream>
#include "llvm/Support/raw_os_ostream.h"
#include "llvm/IR/Verifier.h"
#include "RefValueHeader.h"
#include "RTMulticastedInterface.h"
#include <stdarg.h>
#include "RawInvoke.h"
#include "NomMethodKey.h"

using namespace llvm;
using namespace std;
namespace Nom
{
	namespace Runtime
	{
		llvm::FunctionType* GetIMTCastFunctionType()
		{
			static auto ft = FunctionType::get(POINTERTYPE, { TYPETYPE->getPointerTo(), POINTERTYPE, POINTERTYPE, POINTERTYPE, POINTERTYPE }, false);
			return ft;
		}

		llvm::FunctionType* GetIMTFunctionType()
		{
			static auto ft = FunctionType::get(POINTERTYPE, { GetIMTCastFunctionType()->getPointerTo(), POINTERTYPE, POINTERTYPE, POINTERTYPE, POINTERTYPE }, false);
			return ft;
		}

		llvm::Function* GenerateRawInvokeWrap(llvm::Module* mod, llvm::GlobalValue::LinkageTypes linkage, const llvm::Twine name, const NomInterface* ifc, llvm::Function* callCode)
		{
			NomMethod* invokeMethod = ifc->Methods[0];
			FunctionType * funtype = invokeMethod->GetRawInvokeLLVMFunctionType();

			Function* fun = Function::Create(funtype, linkage, name, mod);
			fun->setCallingConv(NOMCC);

			NomBuilder builder;
			BasicBlock* startBlock = BasicBlock::Create(LLVMCONTEXT, "start", fun);
			builder->SetInsertPoint(startBlock);

			auto argiter = fun->arg_begin();
			auto argcount = funtype->getNumParams();
			llvm::Value** argarr = makealloca(llvm::Value*, argcount);
			for (decltype(argcount) i = 0; i < argcount; i++)
			{
				argarr[i] = argiter;
				argiter++;
			}

			GenerateRawInvoke(builder, ifc, invokeMethod,
				[callCode](NomBuilder& b, const NomMethod* meth, llvm::ArrayRef<llvm::Value*> cargs)
				{
					auto call = b->CreateCall(callCode, cargs);
					call->setCallingConv(NOMCC);
					return call;
				},
				llvm::ArrayRef<llvm::Value*>(argarr,argcount));

			llvm::raw_os_ostream out(std::cout);
			if (verifyFunction(*fun, &out))
			{
				std::cout << "Could not verify raw-invoke wrapper method!";
				fun->print(out);
				out.flush();
				std::cout.flush();
				throw name;
			}
			return fun;
		}

		llvm::Function* GenerateIMT(Module* mod, GlobalValue::LinkageTypes linkage, const llvm::Twine name, SmallVector<pair<NomMethodKey*, Function*>, 8>& imtPairs)
		{
			FunctionType* funtype = GetIMTFunctionType();
			Function* fun = Function::Create(funtype, linkage, name, mod);
			fun->setCallingConv(NOMCC);
			NomBuilder builder;


			auto argiter = fun->arg_begin();
			Argument* id_and_dynhandler = argiter;
			argiter++;

			Argument* args[4];

			args[0] = argiter;
			argiter++;
			args[1] = argiter;
			argiter++;
			args[2] = argiter;
			argiter++;
			args[3] = argiter;

			BasicBlock* startBlock = BasicBlock::Create(LLVMCONTEXT, "", fun);
			SmallVector<Value*, 8> argBuf;
			builder->SetInsertPoint(startBlock);
			if (imtPairs.size() == 0)
			{
				builder->CreateUnreachable();
			}
			else
			{
				auto givenKey = builder->CreatePtrToInt(id_and_dynhandler, INTTYPE, "key");

				int cases = imtPairs.size();
				BasicBlock* currentBlock;
				BasicBlock* nextBlock = startBlock;
				for (int i = 0; i < cases; i++)
				{
					currentBlock = nextBlock;
					auto& pair = imtPairs[i];

					if (i < cases - 1)
					{
						currentBlock = BasicBlock::Create(LLVMCONTEXT, "", fun);
						nextBlock = BasicBlock::Create(LLVMCONTEXT, "", fun);

						auto keyMatch = builder->CreateICmpEQ(givenKey, ConstantExpr::getPtrToInt(pair.first->GetLLVMElement(*mod), INTTYPE));
						builder->CreateCondBr(keyMatch, currentBlock, nextBlock);

					}

					builder->SetInsertPoint(currentBlock);

					argBuf.clear();

					Function* callFun = pair.second;
					int argpos = 0;

					int argcount = callFun->arg_size();
					for (auto &argSpec : callFun->args())
					{
						if (argpos > 2 && argcount > 4)
						{
							argBuf.push_back(MakeLoad(builder, builder->CreateGEP(builder->CreatePointerCast(args[3], args[3]->getType()->getPointerTo()), MakeInt32(argpos - 3))));
						}
						else
						{
							argBuf.push_back(args[argpos]);
						}
						if (argSpec.getType()->isPointerTy())
						{
							argBuf[argpos] = builder->CreatePointerCast(argBuf[argpos], argSpec.getType());
						}
						if (argSpec.getType()->isIntegerTy(INTTYPE->getIntegerBitWidth()))
						{
							argBuf[argpos] = builder->CreatePtrToInt(argBuf[argpos], INTTYPE);
						}
						else if (argSpec.getType()->isDoubleTy())
						{
							argBuf[argpos] = builder->CreateBitCast(builder->CreatePtrToInt(argBuf[argpos], INTTYPE), FLOATTYPE);
						}
						else if (argSpec.getType()->isIntegerTy(BOOLTYPE->getIntegerBitWidth()))
						{
							argBuf[argpos] = builder->CreatePtrToInt(argBuf[argpos], BOOLTYPE);
						}
						argpos++;
					}
					auto callResult = builder->CreateCall(callFun, argBuf);
					callResult->setCallingConv(NOMCC);
					
					llvm::Value* result = callResult;
					if (result->getType()->isPointerTy())
					{
						if (result->getType() != POINTERTYPE)
						{
							result = builder->CreatePointerCast(callResult, POINTERTYPE);
						}
					}
					else
					{
						if (result->getType()->isIntegerTy())
						{
							result = builder->CreateIntToPtr(result, POINTERTYPE);
						}
						else if (result->getType()->isDoubleTy())
						{
							result = builder->CreateIntToPtr(builder->CreateBitCast(result, INTTYPE), POINTERTYPE);
						}
						else
						{
							throw new std::exception();
						}
					}
					
					builder->CreateRet(result);
				}

			}
			llvm::raw_os_ostream out(std::cout);

			if (verifyFunction(*fun, &out))
			{
				out.flush();
				std::cout << "Could not verify IMT method!";
				fun->print(out);
				out.flush();
				std::cout.flush();
				throw name;
			}
			return fun;
		}
	}
}