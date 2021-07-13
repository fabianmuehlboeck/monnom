#pragma once
#include "NomValueInstruction.h"

namespace Nom
{
	namespace Runtime
	{
		class LoadNullConstantInstruction : public NomValueInstruction
		{
		public:
			LoadNullConstantInstruction(RegIndex reg) : NomValueInstruction(reg, OpCode::LoadNullConstant) {}
			~LoadNullConstantInstruction() = default;
			// Inherited via NomValueInstruction
			virtual void Compile(NomBuilder& builder, CompileEnv* env, int lineno) override;
			virtual void Print(bool resolve = false) override;
			virtual void FillConstantDependencies(NOM_CONSTANT_DEPENCENCY_CONTAINER& result) override;
		};
	}
}