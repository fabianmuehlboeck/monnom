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
			const size_t Target;
			BranchInstruction(size_t target):NomInstruction(OpCode::Branch), Target(target)
			{
			}
			~BranchInstruction() override;

			void AddIncoming(RegIndex to, RegIndex from)
			{
				Incomings.push_back(std::make_pair(to, from));
			}

			// Inherited via NomInstruction
			virtual void Compile(NomBuilder& builder, CompileEnv* env, size_t lineno) override;

			// Inherited via NomInstruction
			virtual void Print(bool resolve = false) override; 
			
			virtual void FillConstantDependencies(NOM_CONSTANT_DEPENCENCY_CONTAINER& result) override;
		};
	}
}
