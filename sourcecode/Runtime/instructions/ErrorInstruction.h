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
			~ErrorInstruction() override {}
			// Inherited via NomInstruction
			virtual void Compile(NomBuilder& builder, CompileEnv* env, size_t lineno) override;
			virtual void Print(bool resolve = false) override;


			virtual void FillConstantDependencies(NOM_CONSTANT_DEPENCENCY_CONTAINER& result) override;
		};
	}
}
