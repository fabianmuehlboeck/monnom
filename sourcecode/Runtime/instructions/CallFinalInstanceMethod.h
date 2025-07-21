//#pragma once
//#include "../instructions/NomValueInstruction.h"
//#include "../Common/Defs.h"
//#include "llvm/IR/Value.h"
//
//namespace Nom
//{
//	namespace Runtime
//	{
//		class CallFinalInstanceMethod : public NomValueInstruction
//		{
//		private:
//			llvm::Value * * argarr = nullptr;
//		public:
//			const RegIndex Receiver;
//			const ConstantID Method;
//			CallFinalInstanceMethod(RegIndex reg, RegIndex receiver, ConstantID method);
//			virtual ~CallFinalInstanceMethod();
//			virtual void Compile(llvm::IRBuilder<> &builder, CompileEnv* env, int lineno) override;
//
//			// Inherited via NomValueInstruction
//			virtual void Print(bool resolve = false) override;
//		};
//	}
//}
