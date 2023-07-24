#pragma once

#include "../NomValueInstruction.h"
#include "../Context.h"
#include "../NomConstants.h"
#include "../NomVMInterface.h"
PUSHDIAGSUPPRESSION
#include "llvm/IR/Function.h"
POPDIAGSUPPRESSION
#include <iostream>

namespace Nom
{
	namespace Runtime
	{

		class LoadStringConstantInstruction : public NomValueInstruction
		{
		private:
			const ConstantID constantID;
		public:
			LoadStringConstantInstruction(const RegIndex _reg, const ConstantID _constantID) : NomValueInstruction(_reg, OpCode::LoadStringConstant), constantID(_constantID)
			{

			}
			virtual ~LoadStringConstantInstruction() override
			{
				
			}
			virtual void Compile(NomBuilder &builder, CompileEnv* env, size_t lineno) override;

			virtual void Print(bool resolve = false) override;


			virtual void FillConstantDependencies(NOM_CONSTANT_DEPENCENCY_CONTAINER& result) override;
		};

	}
}
