#include "IMT.h"
#include "Defs.h"
PUSHDIAGSUPPRESSION
#include "llvm/IR/IRBuilder.h"
#include "llvm/Support/raw_os_ostream.h"
#include "llvm/IR/Verifier.h"
POPDIAGSUPPRESSION
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
#include "RefValueHeader.h"
#include <stdarg.h>
#include "NomInterfaceCallTag.h"
#include "RTCompileConfig.h"

using namespace llvm;
using namespace std;
namespace Nom
{
	namespace Runtime
	{
		llvm::FunctionType* GetIMTCastFunctionType()
		{
			return GetIMTFunctionType(); //to make sure this can be tail called
		}
		llvm::FunctionType* GetFieldReadFunctionType()
		{
			static auto ft = FunctionType::get(RefValueHeader::GetUninitializedLLVMType()->getPointerTo(), { numtype(size_t), RefValueHeader::GetUninitializedLLVMType()->getPointerTo() }, false);
			return ft;
		}
		llvm::FunctionType* GetFieldWriteFunctionType()
		{
			static auto ft = FunctionType::get(Type::getVoidTy(LLVMCONTEXT), { numtype(size_t), RefValueHeader::GetUninitializedLLVMType()->getPointerTo(), RefValueHeader::GetUninitializedLLVMType()->getPointerTo() }, false);
			return ft;
		}

		llvm::FunctionType* GetIMTFunctionType()
		{
			static FunctionType* ft;
			static bool once = false;
			if (!once)
			{
				auto argtypes = makealloca(Type*, 2 + RTConfig_NumberOfVarargsArguments);
				argtypes[0] = POINTERTYPE; //GetIMTCastFunctionType()->getPointerTo();
				argtypes[1] = POINTERTYPE;
				for (decltype(RTConfig_NumberOfVarargsArguments) i = 0; i < RTConfig_NumberOfVarargsArguments; i++)
				{
					argtypes[i + 2] = POINTERTYPE;
				}
				ft = FunctionType::get(POINTERTYPE, ArrayRef<Type*>(argtypes, static_cast<size_t>(2 + RTConfig_NumberOfVarargsArguments)), false);
				once = true;
			}

			return ft;
		}

		llvm::StructType* GetDynamicDispatchListEntryType()
		{
			static StructType* st = StructType::create(LLVMCONTEXT, "MONNOM_RT_DynamicDispatchListEntry");
			static bool once = true;
			if (once)
			{
				once = false;
				st->setBody({
					inttype(64),							//key
					inttype(64),							//flags
					GetIMTFunctionType()->getPointerTo()	//dispatcher
					}, false);
			}
			return st;
		}

		llvm::Constant* GetDynamicDispatchListEntryConstant(llvm::Constant* key, llvm::Constant* flags, llvm::Constant* dispatcherPtr)
		{
			return ConstantStruct::get(GetDynamicDispatchListEntryType(), key, flags, dispatcherPtr);
		}


		llvm::StructType* GetDynamicDispatcherLookupResultType()
		{
			static llvm::StructType* st = StructType::create(LLVMCONTEXT, { GetIMTFunctionType()->getPointerTo(), POINTERTYPE }, "DynamicDispatcherResultPair");
			return st;
		}

		llvm::FunctionType* GetCheckReturnValueFunctionType()
		{
			//static auto ft = FunctionType::get(inttype(1), { GetIMTCastFunctionType()->getPointerTo(), TYPETYPE, POINTERTYPE, POINTERTYPE, POINTERTYPE, POINTERTYPE }, false);
			//return ft;
			static FunctionType* ft = nullptr;
			static bool once = false;
			if (!once)
			{
				auto argtypes = makealloca(Type*, 3 + RTConfig_NumberOfVarargsArguments);
				argtypes[0] = POINTERTYPE; //method key
				argtypes[1] = TYPETYPE->getPointerTo()->getPointerTo();
				argtypes[2] = REFTYPE; //return value; always packed because it comes from struct dispatcher
				for (decltype(RTConfig_NumberOfVarargsArguments) i = 0; i < RTConfig_NumberOfVarargsArguments; i++)
				{
					argtypes[i + 3] = POINTERTYPE;
				}
				ft = FunctionType::get(llvm::Type::getVoidTy(LLVMCONTEXT), ArrayRef<Type*>(argtypes, static_cast<size_t>(3 + RTConfig_NumberOfVarargsArguments)), false);
				once = true;
			}

			return ft;
		}

		llvm::Function* GenerateCheckReturnTypesFunction(Module* mod, GlobalValue::LinkageTypes linkage, const llvm::Twine name, SmallVector<tuple<NomInterfaceCallTag*, Function*, NomType*>, 8>& imtPairs)
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

				auto cases = imtPairs.size();
				BasicBlock* currentBlock;
				BasicBlock* nextBlock = startBlock;
				for (decltype(cases) i = 0; i < cases; i++)
				{
					currentBlock = nextBlock;
					auto& pair = imtPairs[i];

					if (i < cases - 1)
					{
						currentBlock = BasicBlock::Create(LLVMCONTEXT, "", fun);
						nextBlock = BasicBlock::Create(LLVMCONTEXT, "", fun);

						auto keyMatch = builder->CreateICmpEQ(givenKey, ConstantExpr::getPtrToInt(std::get<0>(pair)->GetLLVMElement(*mod), INTTYPE));
						builder->CreateCondBr(keyMatch, currentBlock, nextBlock);

					}

					builder->SetInsertPoint(currentBlock);

					argBuf.clear();

					Function* callFun = std::get<1>(pair);

					auto argcount = callFun->arg_size();
					decltype(argcount) argpos = 0;
					for (auto& argSpec : callFun->args())
					{
						if (argpos > 2 && argcount > 4)
						{
							argBuf.push_back(MakeLoad(builder, POINTERTYPE, builder->CreateGEP(args[3]->getType(), args[3], MakeInt32(argpos - 3))));
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
