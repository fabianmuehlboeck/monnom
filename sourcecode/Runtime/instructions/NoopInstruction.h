#pragma once

#include "../NomInstruction.h"

namespace Nom
{
	namespace Runtime
	{
		class NoopInstruction : public NomInstruction
		{
		public:
			NoopInstruction() : NomInstruction(OpCode::Noop)
			{

			}
			virtual ~NoopInstruction() override
			{

			}

			virtual void Compile([[maybe_unused]] NomBuilder &builder, [[maybe_unused]] CompileEnv* env, [[maybe_unused]] size_t lineno) override
			{

			}

			// Inherited via NomInstruction
			virtual void Print([[maybe_unused]] bool resolve = false) override;

			virtual void FillConstantDependencies([[maybe_unused]] NOM_CONSTANT_DEPENCENCY_CONTAINER& result) override;
		};

	}
}

