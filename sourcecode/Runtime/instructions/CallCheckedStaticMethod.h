#pragma once
#include "../NomValueInstruction.h"
#include "../Defs.h"

namespace Nom
{
	namespace Runtime
	{
		class CallCheckedStaticMethod : public NomValueInstruction
		{
		public:
			const ConstantID Method;
			const ConstantID TypeArgs;
			CallCheckedStaticMethod(const ConstantID method, const ConstantID typeArgs, RegIndex reg);
			virtual ~CallCheckedStaticMethod() override;
			virtual void Compile(NomBuilder &builder, CompileEnv* env, size_t lineno) override;

			// Inherited via NomValueInstruction
			virtual void Print(bool resolve = false) override;


			virtual void FillConstantDependencies(NOM_CONSTANT_DEPENCENCY_CONTAINER& result) override;
		};
	}
}
