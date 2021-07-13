#pragma once
#include "../NomInstruction.h"

namespace Nom
{
	namespace Runtime
	{
		class CondBranchInstruction : public NomInstruction
		{
		private:
			std::vector<std::pair<RegIndex, RegIndex>> ThenIncomings;
			std::vector<std::pair<RegIndex, RegIndex>> ElseIncomings;
		public:
			const RegIndex Condition;
			const int ThenTarget;
			const int ElseTarget;
			CondBranchInstruction(RegIndex condition, int thenTarget, int elseTarget);
			~CondBranchInstruction();

			void AddThenIncoming(RegIndex to, RegIndex from)
			{
				ThenIncomings.push_back(std::make_pair(to, from));
			}
			void AddElseIncoming(RegIndex to, RegIndex from)
			{
				ElseIncomings.push_back(std::make_pair(to, from));
			}

			// Inherited via NomInstruction
			virtual void Compile(NomBuilder& builder, CompileEnv* env, int lineno) override;

			// Inherited via NomInstruction
			virtual void Print(bool resolve = false) override;


			virtual void FillConstantDependencies(NOM_CONSTANT_DEPENCENCY_CONTAINER& result) override;
		};


	}
}