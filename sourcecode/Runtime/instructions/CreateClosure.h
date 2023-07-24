#pragma once
#include "../NomValueInstruction.h"

namespace Nom
{
	namespace Runtime
	{
		class CreateClosure : public NomValueInstruction
		{
		public:
			const ConstantID Lambda;
			const ConstantID TypeArgsID;
			CreateClosure(RegIndex reg, ConstantID lambda, ConstantID typeArgs);
			// Inherited via NomValueInstruction
			virtual void Compile(NomBuilder& builder, CompileEnv* env, size_t lineno) override;

			// Inherited via NomValueInstruction
			virtual void Print(bool resolve = false) override;


			virtual void FillConstantDependencies(NOM_CONSTANT_DEPENCENCY_CONTAINER& result) override;
		};
	}
}
