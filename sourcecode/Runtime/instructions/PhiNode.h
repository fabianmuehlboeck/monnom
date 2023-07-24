#pragma once

#include "../Context.h"
#include "../NomInstruction.h"
PUSHDIAGSUPPRESSION
#include "llvm/ADT/Twine.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
POPDIAGSUPPRESSION
#include <vector>
#include <tuple>

namespace Nom
{
	namespace Runtime
	{
		class PhiNode : public NomInstruction
		{
		private:
			std::vector<std::pair<RegIndex, ConstantID>> mergeRegisters;
			llvm::BasicBlock *myBlock;
		public:
			const unsigned int IncomingCount;
			PhiNode(unsigned int incomingCount) : NomInstruction(OpCode::PhiNode), IncomingCount(incomingCount)
			{

			}
			~PhiNode() override
			{
			}

			void AddRegisterEntry(RegIndex reg, ConstantID type)
			{
				mergeRegisters.push_back(std::make_pair(reg, type));
			}

			void Initialize(NomBuilder &builder, llvm::Function * fun, CompileEnv* env);

			// Inherited via NomInstruction
			virtual void Compile(NomBuilder& builder, CompileEnv* env, size_t lineno) override;

			llvm::BasicBlock *getBlock() { return myBlock; }

			// Inherited via NomInstruction
			virtual void Print(bool resolve = false) override;

			virtual void FillConstantDependencies(NOM_CONSTANT_DEPENCENCY_CONTAINER& result) override;
		};
	}
}
