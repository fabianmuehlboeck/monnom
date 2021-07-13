#pragma once
#include "../NomInstruction.h"
#include "../CompileEnv.h"
#include "llvm/IR/IRBuilder.h"

namespace Nom
{
	namespace Runtime
	{

		class ArgumentInstruction : public NomInstruction
		{
		public:
			const RegIndex Register;
			ArgumentInstruction(RegIndex reg);
			virtual ~ArgumentInstruction();
			virtual void Compile(NomBuilder &builder, CompileEnv* env, int lineno) override
			{
				env->PushArgument((*env)[Register]);
			}

			// Inherited via NomInstruction
			virtual void Print(bool resolve = false) override;

			virtual void FillConstantDependencies(NOM_CONSTANT_DEPENCENCY_CONTAINER & result) override;
		};
	}
}
