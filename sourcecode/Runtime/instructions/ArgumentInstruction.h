#pragma once
#include "../NomInstruction.h"
#include "../CompileEnv.h"

namespace Nom
{
	namespace Runtime
	{

		class ArgumentInstruction : public NomInstruction
		{
		public:
			const RegIndex Register;
			ArgumentInstruction(RegIndex reg);
			virtual ~ArgumentInstruction() override;
			virtual void Compile([[maybe_unused]] NomBuilder &builder, CompileEnv* env, [[maybe_unused]] size_t lineno) override
			{
				env->PushArgument((*env)[Register]);
			}

			// Inherited via NomInstruction
			virtual void Print(bool resolve = false) override;

			virtual void FillConstantDependencies(NOM_CONSTANT_DEPENCENCY_CONTAINER & result) override;
		};
	}
}
