#pragma once
#include "../NomInstruction.h"

namespace Nom
{
	namespace Runtime {

		class ReturnInstruction :
			public NomInstruction
		{
		private:
			const RegIndex reg;
		public:
			virtual void Compile(NomBuilder& builder, CompileEnv* env, int lineno) override;
			ReturnInstruction(int reg) : NomInstruction(OpCode::Return), reg(reg)
			{

			}
			virtual ~ReturnInstruction();

			// Inherited via NomInstruction
			virtual void Print(bool resolve = false) override;


			virtual void FillConstantDependencies(NOM_CONSTANT_DEPENCENCY_CONTAINER& result) override;
		};

	}
}