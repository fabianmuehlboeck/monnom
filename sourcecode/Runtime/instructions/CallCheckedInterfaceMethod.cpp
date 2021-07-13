//#include "CallCheckedInterfaceMethod.h"
//#include "../NomConstants.h"
//#include "../ObjectHeader.h"
//#include "../NomAlloc.h"
//#include "../NomMethod.h"
//#include "../CompileHelpers.h"
//#include <iostream>
//
//using namespace std;
//namespace Nom
//{
//	namespace Runtime
//	{
//		using namespace llvm;
//		CallCheckedInterfaceMethod::CallCheckedInterfaceMethod(RegIndex reg, RegIndex receiver, InterfaceID iface, ConstantID method) : NomValueInstruction(reg, OpCode::CallCheckedInterface), Receiver(receiver), Interface(iface), Method(method)
//		{
//		}
//
//
//		CallCheckedInterfaceMethod::~CallCheckedInterfaceMethod()
//		{
//			nfree(argarr);
//		}
//
//		void CallCheckedInterfaceMethod::Compile(llvm::IRBuilder<>& builder, CompileEnv* env, int lineno)
//		{
//			env->basicBlockTerminated = false;
//			NomSubstitutionContextMemberContext nscmc(env->Context);
//			NomInstantiationRef<const NomMethod> method = NomConstants::GetMethod(Method)->GetMethod(&nscmc);
//			NomSubstitutionContextList nscl(method.TypeArgs);
//			Value *methodptraddress = ObjectHeader::GetInterfaceMethodPointer(builder, env, Receiver, lineno, Interface, method.Elem->GetOffset());
//			Value *methodptrptr = builder.CreatePointerCast(methodptraddress, method.Elem->GetLLVMFunctionType()->getPointerTo()->getPointerTo());
//			Value *methodptr = MakeLoad(builder,*(env->Module),methodptrptr);
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
//			RegisterValue(env, NomValue(builder.CreateCall(method.Elem->GetLLVMFunctionType(), methodptr, llvm::ArrayRef<llvm::Value *>(argarr, method.Elem->GetArgumentCount() + 1)), method.Elem->GetReturnType(&nscl)));
//		}
//		void CallCheckedInterfaceMethod::Print(bool resolve)
//		{
//			cout << "ICall #" << std::dec << Receiver;
//			cout << " ";
//			NomConstants::PrintConstant(Interface, resolve);
//			cout << ".";
//			NomConstants::PrintConstant(Method, resolve);
//			cout << " -> #" << std::dec << WriteRegister;
//			cout << "\n";
//		}
//	}
//}