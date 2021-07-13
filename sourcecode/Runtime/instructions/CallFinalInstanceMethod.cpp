//#include "CallFinalInstanceMethod.h"
//#include "../NomConstants.h"
//#include "../NomInstantiationRef.h"
//#include "llvm/IR/Value.h"
//#include "llvm/IR/IRBuilder.h"
//#include "../NomAlloc.h"
//#include "../NomMethod.h"
//#include <iostream>
//
//using namespace std;
//namespace Nom
//{
//	namespace Runtime
//	{
//		using namespace llvm;
//		CallFinalInstanceMethod::CallFinalInstanceMethod(RegIndex reg, RegIndex receiver, ConstantID method) : NomValueInstruction(reg, OpCode::CallFinal), Receiver(receiver), Method(method)
//		{
//		}
//
//
//		CallFinalInstanceMethod::~CallFinalInstanceMethod()
//		{
//			nfree(argarr);
//		}
//		void CallFinalInstanceMethod::Compile(llvm::IRBuilder<>& builder, CompileEnv* env, int lineno)
//		{
//			env->basicBlockTerminated = false;
//			NomSubstitutionContextMemberContext nscmc(env->Context);
//			NomInstantiationRef<const NomMethod> method = NomConstants::GetMethod(Method)->GetMethod(&nscmc);
//			NomSubstitutionContextList nscl(method.TypeArgs);
//			if (argarr != nullptr)
//			{
//				nfree(argarr);
//			}
//			argarr = (Value * *)nalloc(sizeof(Value*)*(method.Elem->GetArgumentCount() + 1));
//			argarr[0] = (*env)[Receiver];
//			for (int i = env->GetArgCount() - 1; i >= 0; i--)
//			{
//				argarr[i] = env->GetArgument(i);
//			}
//			env->ClearArguments();
//			RegisterValue(env, NomValue(builder.CreateCall(method.Elem->GetLLVMFunction(env->Module), llvm::ArrayRef<llvm::Value *>(argarr, method.Elem->GetArgumentCount() + 1)), method.Elem->GetReturnType(&nscl)));
//		}
//		void CallFinalInstanceMethod::Print(bool resolve)
//		{
//			cout << "FCall #" << std::dec << Receiver;
//			cout << ".";
//			NomConstants::PrintConstant(Method, resolve);
//			cout << " -> #" << std::dec << WriteRegister;
//			cout << "\n";
//		}
//	}
//}