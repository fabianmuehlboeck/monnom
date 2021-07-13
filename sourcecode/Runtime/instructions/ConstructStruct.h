#pragma once
#include "../NomValueInstruction.h"

namespace Nom
{
	namespace Runtime
	{

		class ConstructStructInstruction : public NomValueInstruction
		{
		public:
			const ConstantID StructureID;
			const ConstantID TypeArgsID;
			ConstructStructInstruction(RegIndex reg, ConstantID structure, ConstantID typeArgs);
			// Inherited via NomValueInstruction
			virtual void Compile(NomBuilder& builder, CompileEnv* env, int lineno) override;
			virtual void Print(bool resolve = false) override;


			virtual void FillConstantDependencies(NOM_CONSTANT_DEPENCENCY_CONTAINER& result) override;
		};
	}
}
