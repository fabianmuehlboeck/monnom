#pragma once
#include "../NomInstruction.h"

namespace Nom
{
	namespace Runtime
	{
		class ReturnVoidInstruction : public NomInstruction
		{
		public:
			ReturnVoidInstruction();
			virtual ~ReturnVoidInstruction() override;
			virtual void Compile(NomBuilder &builder, CompileEnv* env, int lineno) override;

			// Inherited via NomInstruction
			virtual void Print(bool resolve = false) override;

			virtual void FillConstantDependencies(NOM_CONSTANT_DEPENCENCY_CONTAINER& result) override;
		};
	}
}
	