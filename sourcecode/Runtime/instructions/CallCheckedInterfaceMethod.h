//#pragma once
//#include "../NomValueInstruction.h"
//#include "../Defs.h"
//#include "llvm/IR/Value.h"
//
//namespace Nom
//{
//	namespace Runtime
//	{
//		class CallCheckedInterfaceMethod : public NomValueInstruction
//		{
//		private:
//			llvm::Value * * argarr = nullptr;
//		public:
//			const RegIndex Receiver;
//			const InterfaceID Interface;
//			const ConstantID Method;
//			CallCheckedInterfaceMethod(RegIndex reg, RegIndex receiver, InterfaceID iface, ConstantID method);
//			virtual ~CallCheckedInterfaceMethod();
//			virtual void Compile(llvm::IRBuilder<> &builder, CompileEnv* env, int lineno) override;
//
//			// Inherited via NomValueInstruction
//			virtual void Print(bool resolve = false) override;
//		};
//	}
//}