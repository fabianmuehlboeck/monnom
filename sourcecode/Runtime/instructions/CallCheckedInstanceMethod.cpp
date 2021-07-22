#include "CallCheckedInstanceMethod.h"
#include "../NomAlloc.h"
#include "../NomMethod.h"
#include "llvm/IR/BasicBlock.h"
#include "../NomInterface.h"
#include "../IntClass.h"
#include "../FloatClass.h"
#include "../NomClass.h"
#include "../NomClassType.h"
#include "../TypeOperations.h"
#include <iostream>
#include <cstdio>
#include "llvm/IR/Type.h"
#include "llvm/Support/raw_os_ostream.h"
#include "../CallingConvConf.h"
#include "../CompileHelpers.h"
#include "../IMT.h"
#include "../RTCompileConfig.h"
#include "../RefValueHeader.h"
#include "../NomMethodKey.h"
#include "../RTConfig.h"
#include "../CastStats.h"

using namespace llvm;
using namespace std;
namespace Nom
{
	namespace Runtime
	{
		CallCheckedInstanceMethod::~CallCheckedInstanceMethod()
		{
			nfree(argarr);
		}
		void CallCheckedInstanceMethod::Compile(NomBuilder& builder, CompileEnv* env, int lineno)
		{

			env->basicBlockTerminated = false;
			NomSubstitutionContextMemberContext nscmc(env->Context);
			
			NomInstantiationRef<const NomMethod> method = NomConstants::GetMethod(Method)->GetMethod(&nscmc);

			bool rawInvoke = false;
			if ((!method.Elem->IsFinal())&&method.Elem->GetName().empty()&&NomLambdaOptimizationLevel>0)
			{
				rawInvoke = true;
			}

			NomSubstitutionContextList nscl(method.TypeArgs);
			
			if (argarr != nullptr)
			{
				nfree(argarr);
			}

			int additionalArgs = 0;
			auto recContainer = method.Elem->GetContainer();
			if (rawInvoke)
			{
				additionalArgs += 1; //used to indicate which interface the raw invocation is based on
			}
			else if (recContainer->IsInterface())
			{
				additionalArgs += 1; //these are for calling the IMT method, no need for them when invoking directly
			}
			

			auto substitutedSig = method.Elem->Signature(&nscl);



			argarr = (Value * *)nalloc(sizeof(Value*)*(method.Elem->GetArgumentCount() + method.Elem->GetDirectTypeParametersCount() + 1 + additionalArgs+3));
			argarr += additionalArgs;

			for (int i = 0; i < 4; i++)
			{
				argarr[i] = ConstantPointerNull::get(REFTYPE);
			}


			argarr[0] = (*env)[Receiver];

			for (int i = 1; i <= method.Elem->GetDirectTypeParametersCount(); i++)
			{
				argarr[i] = method.TypeArgs[i]->GetLLVMElement(*env->Module);
			}

			for (int i = env->GetArgCount() - 1; i >= 0; i--)
			{
				argarr[i + 1 + method.Elem->GetDirectTypeParametersCount()] = env->GetArgument(i);
			}

			FunctionType *ftype = method.Elem->GetLLVMFunctionType();
			bool uncertainty = true;

			Type *pt;
			Type *argt;
			for (int i = env->GetArgCount(); i >= 0; i--)
			{
				if (i > 0 && i <= method.Elem->GetDirectTypeParametersCount())
				{
					continue;
				}
				pt = ftype->getParamType(i);
				argt = argarr[i]->getType();
				if (pt != argt)
				{
					if (pt->isIntegerTy() && argt->isPointerTy())
					{
						argarr[i] = UnpackInt(builder, argarr[i]);
						continue;
					}
					if (pt->isDoubleTy() && argt->isPointerTy())
					{
						argarr[i] = UnpackFloat(builder, argarr[i]);
						continue;
					}
					if (pt->isPointerTy() && argt->isIntegerTy())
					{
						argarr[i] = PackInt(builder, argarr[i]);
						continue;
					}
					if (pt->isPointerTy() && argt->isDoubleTy())
					{
						argarr[i] = PackFloat(builder, argarr[i]);
						continue;
					}
					llvm::raw_os_ostream out(std::cout);
					pt->print(out);
					std::cout << "\n";
					argt->print(out);
					out.flush();
					std::cout.flush();
					throw new std::exception();
				}
			}
			env->ClearArguments();

			if (method.Elem->IsFinal())
			{
				if (NomCastStats)
				{
					builder->CreateCall(GetIncFinalInstanceMethodCalls(*builder->GetInsertBlock()->getParent()->getParent()), {});
				}
				Function * fun = method.Elem->GetLLVMFunction(env->Module);
				auto call = builder->CreateCall(fun, llvm::ArrayRef<llvm::Value*>(argarr, method.Elem->GetArgumentCount() + method.Elem->GetDirectTypeParametersCount() + 1));
				call->setCallingConv(NOMCC);
				RegisterValue(env, NomValue(call, method.Elem->GetReturnType(&nscl), true));
				return;
			}

			NomClassTypeRef inttype = NomIntClass::GetInstance()->GetType();
			NomClassTypeRef floattype = NomFloatClass::GetInstance()->GetType();
			
			Value *receiver = (*env)[Receiver];

			if (rawInvoke)
			{
				if (NomCastStats)
				{
					builder->CreateCall(GetIncTypedRawInvokes(*builder->GetInsertBlock()->getParent()->getParent()), {});
				}
				auto recNV = (*env)[Receiver];
				llvm::Value* methodptr = builder->CreatePointerCast( RefValueHeader::GenerateReadRawInvoke(builder, recNV), method.Elem->GetRawInvokeLLVMFunctionType()->getPointerTo());
				argarr -= 1;
				argarr[0] = MakeInt<size_t>(method.Elem->GetContainer()->GetID());
				auto call = builder->CreateCall(method.Elem->GetRawInvokeLLVMFunctionType(), methodptr, llvm::ArrayRef<llvm::Value*>(argarr, method.Elem->GetArgumentCount() + 1 + 1 + method.Elem->GetDirectTypeParametersCount()), "rawInvoke_"+method.Elem->GetContainer()->GetName()->ToStdString());
				call->setCallingConv(NOMCC);
				RegisterValue(env, NomValue(call, method.Elem->GetReturnType(&nscl), true));
			}
			else if (method.Elem->GetContainer()->IsInterface())
			{
				if (method.Elem->GetArgumentCount() + method.Elem->GetDirectTypeParametersCount() > 3)
				{
					if (NomCastStats)
					{
						builder->CreateCall(GetIncExtendedInterfaceMethodCalls(*builder->GetInsertBlock()->getParent()->getParent()), {});
					}
					llvm::Value* argsasarr = builder->CreateAlloca(POINTERTYPE, MakeInt32(method.Elem->GetArgumentCount() + method.Elem->GetDirectTypeParametersCount() -2), "argarray");
					for (int j = method.Elem->GetArgumentCount() + method.Elem->GetDirectTypeParametersCount() -1; j >= 2; j--)
					{
						MakeStore(builder, WrapAsPointer(builder, argarr[j + 1]), builder->CreateGEP(argsasarr, MakeInt32(j), "arginarray"));
					}
					argarr[3] = builder->CreatePointerCast(argsasarr, POINTERTYPE);
				}
				else
				{
					if (NomCastStats)
					{
						builder->CreateCall(GetIncInterfaceMethodCalls(*builder->GetInsertBlock()->getParent()->getParent()), {});
					}
				}
				for (int j = 0; j < 4; j++)
				{
					argarr[j] = WrapAsPointer(builder, argarr[j]);
				}
				argarr -= 1;
				argarr[0] = NomMethodKey::GetMethodKey(method.Elem)->GetLLVMElement(*env->Module);

				Value* methodptr = ObjectHeader::GetInterfaceMethodTableFunction(builder, env, Receiver, MakeInt32(method.Elem->GetIMTIndex()), lineno);

				auto call = builder->CreateCall(GetIMTFunctionType(), methodptr, llvm::ArrayRef<llvm::Value*>(argarr, 5));
				call->setName("calling " + method.Elem->GetName());
				call->setCallingConv(NOMCC);
				auto rettype = method.Elem->GetLLVMFunctionType()->getReturnType();
				Value* result;
				if (rettype->isDoubleTy())
				{
					result = builder->CreateBitCast(builder->CreatePtrToInt(call, INTTYPE), FLOATTYPE);
				}
				else
				{
					result = builder->CreateBitOrPointerCast(call, rettype);
				}
				RegisterValue(env, NomValue(result, method.Elem->GetReturnType(&nscl), true));

			}
			else
			{
				if (NomCastStats)
				{
					builder->CreateCall(GetIncDirectClassMethodCalls(*builder->GetInsertBlock()->getParent()->getParent()), {});
				}
				Value* methodptr = ObjectHeader::GetDispatchMethodPointer(builder, env, Receiver, lineno, method);
				auto call = builder->CreateCall(method.Elem->GetLLVMFunctionType(), methodptr, llvm::ArrayRef<llvm::Value*>(argarr, method.Elem->GetArgumentCount() + 1));
				call->setName("calling " + method.Elem->GetName());
				call->setCallingConv(NOMCC);
				RegisterValue(env, NomValue(call, method.Elem->GetReturnType(&nscl), true));
			}
		}
		void CallCheckedInstanceMethod::Print(bool resolve)
		{
			cout << "Call #" << std::dec << Receiver;
			cout << ".";
			NomConstants::PrintConstant(Method, resolve);
			cout << " -> #" << std::dec << WriteRegister;
			cout << "\n";
		}
		void CallCheckedInstanceMethod::FillConstantDependencies(NOM_CONSTANT_DEPENCENCY_CONTAINER& result)
		{
			result.push_back(Method);
			result.push_back(TypeArgs);
		}
	}
}
