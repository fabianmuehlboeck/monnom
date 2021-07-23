#pragma once
#include "../NomValueInstruction.h"
#include "../NomConstants.h"
#include "../ObjectHeader.h"
#include "llvm/IR/Value.h"

namespace Nom
{
	namespace Runtime
	{
		class CallCheckedInstanceMethod : public NomValueInstruction
		{
		public:
			const RegIndex Receiver;
			const ConstantID Method;
			const ConstantID TypeArgs;
			CallCheckedInstanceMethod(const RegIndex reg, const ConstantID method, const ConstantID typeArgs, const RegIndex receiver) : NomValueInstruction(reg, OpCode::InvokeCheckedInstance), Receiver(receiver), Method(method), TypeArgs(typeArgs)
			{

			}
			virtual ~CallCheckedInstanceMethod();
			virtual void Compile(NomBuilder &builder, CompileEnv* env, int lineno) override;

			// Inherited via NomValueInstruction
			virtual void Print(bool resolve = false) override;

			virtual void FillConstantDependencies(NOM_CONSTANT_DEPENCENCY_CONTAINER & result) override;
		};
	}
}

