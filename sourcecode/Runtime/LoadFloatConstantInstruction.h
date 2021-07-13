#pragma once
#include "NomValueInstruction.h"

namespace Nom
{
	namespace Runtime
	{
		class LoadFloatConstantInstruction : public NomValueInstruction
		{
		public:
			const double Value;
			LoadFloatConstantInstruction(const double value, const RegIndex reg);
			virtual ~LoadFloatConstantInstruction();
			virtual void Compile(NomBuilder& builder, CompileEnv* env, int lineno) override;

			// Inherited via NomValueInstruction
			virtual void Print(bool resolve = false) override;

			virtual void FillConstantDependencies(NOM_CONSTANT_DEPENCENCY_CONTAINER& result) override;
		};

	}
}