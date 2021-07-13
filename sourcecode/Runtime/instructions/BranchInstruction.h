#pragma once
#include <vector>
#include "../NomInstruction.h"

namespace Nom
{
	namespace Runtime
	{
		class BranchInstruction : public NomInstruction
		{
		private:
			std::vector<std::pair<RegIndex, RegIndex>> Incomings;
		public:
			const int Target;
			BranchInstruction(int target):NomInstruction(OpCode::Branch), Target(target)
			{
			}
			~BranchInstruction();

			void AddIncoming(RegIndex to, RegIndex from)
			{
				Incomings.push_back(std::make_pair(to, from));
			}

			// Inherited via NomInstruction
			virtual void Compile(NomBuilder& builder, CompileEnv* env, int lineno) override;

			// Inherited via NomInstruction
			virtual void Print(bool resolve = false) override; 
			
			virtual void FillConstantDependencies(NOM_CONSTANT_DEPENCENCY_CONTAINER& result) override;
		};

	}
}