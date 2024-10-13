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
#include "../NomInterfaceCallTag.h"
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
		}
		void CallCheckedInstanceMethod::Compile(NomBuilder& builder, CompileEnv* env, int lineno)
		{
			NomSubstitutionContextMemberContext nscmc(env->Context);
			NomInstantiationRef<const NomMethod> method = NomConstants::GetMethod(Method)->GetMethod(&nscmc);
			CompileActual(method,builder,env,lineno);
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
		
		void CallCheckedInstanceMethod::CompileActual(NomInstantiationRef<const NomMethod> method, NomBuilder& builder, CompileEnv* env, int lineno)
		{

			env->basicBlockTerminated = false;

			/*
			Final -> Cannot be overriden in sub-class
			GetName Empty -> No method name (Interfaces, lambdas with only one method. Class behaves just like lambda)
			(Special pointer in the object itself to avoid hops in method table)
			*/

			bool rawInvoke = false;
			if ((!method.Elem->IsFinal()) && method.Elem->GetName().empty() && NomLambdaOptimizationLevel > 0)
			{
				rawInvoke = true;
			}

			NomSubstitutionContextList nscl(method.TypeArgs);

			/*
			Interface method table (interface method calls)
			Class methods -> each object has pointer going to method table (starting from parent methods to its own methods)
			Index in parent class same as subclass

			Can implement multiple interfaces -> have list of all interfaces, each of those have identifiers, pointer to a sepearate table.
			If matches interface, has table of corresponding method in own class.

			Hashtable (fixed size) of interface method dispatcher methods, constant is 13 (total dispatcher methods)
			Each method has additional argument telling which interface and method pair calling

			Dispatcher method -> Either one method that dispatcher handles (ignore identifier)
								-> Multiple method, has switch table and maps pair to class (call corresponding method).
			Identifier can be a pointer to another method, that handles the case that you call an interface method on structural object.

			new Record (field x (lambda))
			has not type (dynamic object). Can call x, retrieve x.
			Interact with typed code. Didn't declare that it was an instance of an interface. Cast it to an interface (method X)

			Takes identifier (method pointer) and calls that.
			*/

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



			auto argarr = makealloca(Value*, max(method.Elem->GetArgumentCount() + method.Elem->GetDirectTypeParametersCount() + 1 + additionalArgs, (size_t)RTConfig_NumberOfVarargsArguments + 2));
			argarr += additionalArgs;

			for (decltype(RTConfig_NumberOfVarargsArguments) i = 0; i <= RTConfig_NumberOfVarargsArguments; i++)
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

			FunctionType* ftype = method.Elem->GetLLVMFunctionType();
			bool uncertainty = true;

			Type* pt;
			Type* argt;
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
				Function* fun = method.Elem->GetLLVMFunction(env->Module);
				auto call = builder->CreateCall(fun, llvm::ArrayRef<llvm::Value*>(argarr, method.Elem->GetArgumentCount() + method.Elem->GetDirectTypeParametersCount() + 1));
				call->setCallingConv(NOMCC);
				RegisterValue(env, NomValue(call, method.Elem->GetReturnType(&nscl), true));
				return;
			}

			NomClassTypeRef inttype = NomIntClass::GetInstance()->GetType();
			NomClassTypeRef floattype = NomFloatClass::GetInstance()->GetType();

			Value* receiver = (*env)[Receiver];

			if (rawInvoke)
			{
				if (NomCastStats)
				{
					builder->CreateCall(GetIncTypedRawInvokes(*builder->GetInsertBlock()->getParent()->getParent()), {});
				}
				auto recNV = (*env)[Receiver];
				llvm::Value* methodptr = builder->CreatePointerCast(RefValueHeader::GenerateReadRawInvoke(builder, recNV), GetIMTFunctionType()->getPointerTo());
				auto argsArrSize = method.Elem->GetArgumentCount() + method.Elem->GetDirectTypeParametersCount() - RTConfig_NumberOfVarargsArguments;

				if (method.Elem->GetArgumentCount() + method.Elem->GetDirectTypeParametersCount() > RTConfig_NumberOfVarargsArguments)
				{
					llvm::Value* argsasarr = builder->CreateAlloca(POINTERTYPE, MakeInt32(argsArrSize), "argarray");

					builder->CreateIntrinsic(llvm::Intrinsic::lifetime_start, { POINTERTYPE }, { MakeInt<int64_t>(GetNomJITDataLayout().getTypeAllocSize(POINTERTYPE) * argsArrSize), builder->CreatePointerCast(argsasarr, POINTERTYPE) });
					for (int j = method.Elem->GetArgumentCount() + method.Elem->GetDirectTypeParametersCount() - 1; j >= 2; j--)
					{
						MakeStore(builder, WrapAsPointer(builder, argarr[j + 1]), builder->CreateGEP(argsasarr, MakeInt32(j), "arginarray"), AtomicOrdering::NotAtomic);
					}
					//invariantID = builder->CreateIntrinsic(llvm::Intrinsic::invariant_start, { POINTERTYPE }, { MakeInt<int64_t>(GetNomJITDataLayout().getTypeAllocSize(POINTERTYPE) * argsArrSize), builder->CreatePointerCast(argsasarr, POINTERTYPE) });
					argarr[RTConfig_NumberOfVarargsArguments] = builder->CreatePointerCast(argsasarr, POINTERTYPE);
				}
				for (int j = 0; j <= RTConfig_NumberOfVarargsArguments; j++)
				{
					argarr[j] = WrapAsPointer(builder, argarr[j]);
				}
				argarr -= 1;
				argarr[0] = builder->CreatePointerCast(NomInterfaceCallTag::GetMethodKey(method.Elem)->GetLLVMElement(*env->Module), POINTERTYPE);
				auto call = builder->CreateCall(GetIMTFunctionType(), methodptr, llvm::ArrayRef<llvm::Value*>(argarr, 2 + RTConfig_NumberOfVarargsArguments), "rawInvoke_" + method.Elem->GetContainer()->GetName()->ToStdString());
				call->setCallingConv(NOMCC);
				auto result = EnsurePackedUnpacked(builder, call, method.Elem->GetLLVMFunctionType()->getReturnType());
				RegisterValue(env, NomValue(result, method.Elem->GetReturnType(&nscl), true));
			}
			else if (method.Elem->GetContainer()->IsInterface())
			{
				Value* invariantID = nullptr;
				auto argsArrSize = method.Elem->GetArgumentCount() + method.Elem->GetDirectTypeParametersCount() - RTConfig_NumberOfVarargsArguments;
				if (method.Elem->GetArgumentCount() + method.Elem->GetDirectTypeParametersCount() > RTConfig_NumberOfVarargsArguments)
				{
					if (NomCastStats)
					{
						builder->CreateCall(GetIncExtendedInterfaceMethodCalls(*builder->GetInsertBlock()->getParent()->getParent()), {});
					}
					llvm::Value* argsasarr = builder->CreateAlloca(POINTERTYPE, MakeInt32(argsArrSize), "argarray");

					builder->CreateIntrinsic(llvm::Intrinsic::lifetime_start, { POINTERTYPE }, { MakeInt<int64_t>(GetNomJITDataLayout().getTypeAllocSize(POINTERTYPE) * argsArrSize), builder->CreatePointerCast(argsasarr, POINTERTYPE) });
					for (int j = method.Elem->GetArgumentCount() + method.Elem->GetDirectTypeParametersCount() - 1; j >= 2; j--)
					{
						MakeStore(builder, WrapAsPointer(builder, argarr[j + 1]), builder->CreateGEP(argsasarr, MakeInt32(j), "arginarray"), AtomicOrdering::NotAtomic);
					}
					//invariantID = builder->CreateIntrinsic(llvm::Intrinsic::invariant_start, { POINTERTYPE }, { MakeInt<int64_t>(GetNomJITDataLayout().getTypeAllocSize(POINTERTYPE) * argsArrSize), builder->CreatePointerCast(argsasarr, POINTERTYPE) });
					argarr[RTConfig_NumberOfVarargsArguments] = builder->CreatePointerCast(argsasarr, POINTERTYPE);
				}
				else
				{
					if (NomCastStats)
					{
						builder->CreateCall(GetIncInterfaceMethodCalls(*builder->GetInsertBlock()->getParent()->getParent()), {});
					}
				}
				for (int j = 0; j <= RTConfig_NumberOfVarargsArguments; j++)
				{
					argarr[j] = WrapAsPointer(builder, argarr[j]);
				}
				argarr -= 1;
				argarr[0] = builder->CreatePointerCast(NomInterfaceCallTag::GetMethodKey(method.Elem)->GetLLVMElement(*env->Module), POINTERTYPE);

				Value* methodptr = ObjectHeader::GetInterfaceMethodTableFunction(builder, env, Receiver, MakeInt32(method.Elem->GetIMTIndex()), lineno);

				auto call = builder->CreateCall(GetIMTFunctionType(), methodptr, llvm::ArrayRef<llvm::Value*>(argarr, RTConfig_NumberOfVarargsArguments + 2));
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
				if (invariantID != nullptr)
				{
					builder->CreateIntrinsic(llvm::Intrinsic::invariant_end, { POINTERTYPE }, { invariantID, MakeInt<int64_t>(GetNomJITDataLayout().getTypeAllocSize(POINTERTYPE) * argsArrSize), argarr[RTConfig_NumberOfVarargsArguments + 1] });
					//builder->CreateIntrinsic(llvm::Intrinsic::lifetime_end, { POINTERTYPE }, { MakeInt<int64_t>(GetNomJITDataLayout().getTypeAllocSize(POINTERTYPE) * argsArrSize), argarr[RTConfig_NumberOfVarargsArguments + 1] });
				}
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
	}
}
