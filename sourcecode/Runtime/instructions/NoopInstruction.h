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
			virtual ~NoopInstruction()
			{

			}

			virtual void Compile(NomBuilder &builder, CompileEnv* env, int lineno) override
			{

			}

			// Inherited via NomInstruction
			virtual void Print(bool resolve = false) override;

			virtual void FillConstantDependencies(NOM_CONSTANT_DEPENCENCY_CONTAINER& result) override;
		};

	}
}

