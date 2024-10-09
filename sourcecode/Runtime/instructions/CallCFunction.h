#pragma once
#include "../NomValueInstruction.h"
#include "../Defs.h"

namespace Nom
{
	namespace Runtime
	{
		class CallCFunction : public NomValueInstruction
		{
		public:
			const ConstantID Function;
			const ConstantID TypeArgs;
			CallCFunction(const ConstantID function, const ConstantID typeArgs, RegIndex reg);
			virtual ~CallCFunction();
			virtual void Compile(NomBuilder& builder, CompileEnv* env, int lineno) override;

			// Inherited via NomValueInstruction
			virtual void Print(bool resolve = false) override;


			virtual void FillConstantDependencies(NOM_CONSTANT_DEPENCENCY_CONTAINER& result) override;
		};
	}
}