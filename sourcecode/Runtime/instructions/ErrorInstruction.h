#pragma once
#include "../NomInstruction.h"

namespace Nom
{
	namespace Runtime
	{

		class ErrorInstruction : public NomInstruction
		{
		public:
			const RegIndex Register;
			ErrorInstruction(RegIndex reg);
			// Inherited via NomInstruction
			virtual void Compile(NomBuilder& builder, CompileEnv* env, int lineno) override;
			virtual void Print(bool resolve = false) override;


			virtual void FillConstantDependencies(NOM_CONSTANT_DEPENCENCY_CONTAINER& result) override;
		};


	}
}