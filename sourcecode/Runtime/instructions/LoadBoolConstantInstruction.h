#pragma once
#include "../NomValueInstruction.h"

namespace Nom
{
	namespace Runtime
	{

		class LoadBoolConstantInstruction : public NomValueInstruction
		{
		public:
			const bool Value;
			LoadBoolConstantInstruction(RegIndex reg, bool value);

			// Inherited via NomValueInstruction
			virtual void Compile(NomBuilder& builder, CompileEnv* env, int lineno) override;
			virtual void Print(bool resolve = false) override;

			virtual void FillConstantDependencies(NOM_CONSTANT_DEPENCENCY_CONTAINER& result) override;
		};

	}
}

