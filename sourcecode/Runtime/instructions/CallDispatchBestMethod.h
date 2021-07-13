#pragma once
#include "../NomValueInstruction.h"
#include "../Defs.h"
#include "../NomValue.h"

namespace Nom
{
	namespace Runtime
	{
		class CallDispatchBestMethod : public NomValueInstruction
		{
		public:
			const RegIndex Receiver;
			const ConstantID MethodName;
			const ConstantID TypeArguments;
			CallDispatchBestMethod(RegIndex reg, RegIndex receiver, ConstantID methodName, ConstantID typeArgs);
			virtual ~CallDispatchBestMethod();
			static llvm::Value* GenerateGetBestInvokeDispatcherDyn(NomBuilder& builder, NomValue receiver, llvm::Value* typeargcount, llvm::Value* argcount);
			static llvm::Value* GenerateBestInvoke(NomBuilder& builder, llvm::Value* receiver, uint32_t typeargcount, uint32_t argCount, llvm::ArrayRef<llvm::Value*> args);
			virtual void Compile(NomBuilder &builder, CompileEnv* env, int lineno) override;

			// Inherited via NomValueInstruction
			virtual void Print(bool resolve = false) override;

			virtual void FillConstantDependencies(NOM_CONSTANT_DEPENCENCY_CONTAINER& result) override;
		};

	}
}
