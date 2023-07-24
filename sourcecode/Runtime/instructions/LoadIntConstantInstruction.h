#pragma once
#include "../NomValueInstruction.h"

namespace Nom
{
	namespace Runtime
	{
		class LoadIntConstantInstruction : public NomValueInstruction
		{
		public:
			const uint64_t Value;
			LoadIntConstantInstruction(const uint64_t value, const RegIndex reg);
			virtual ~LoadIntConstantInstruction() override;
			virtual void Compile(NomBuilder &builder, CompileEnv* env, size_t lineno) override;

			// Inherited via NomValueInstruction
			virtual void Print(bool resolve = false) override;


			virtual void FillConstantDependencies(NOM_CONSTANT_DEPENCENCY_CONTAINER& result) override;
		};

	}
}
