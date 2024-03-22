#pragma once
#include "PWInt.h"

namespace Nom
{
	namespace Runtime
	{
		class PWSubstStack;
		class PWType;
		class RTTypeCheckGen
		{
		private:
			llvm::BasicBlock* TypeEqBlock;
			llvm::BasicBlock* SubtypeBlock;
			llvm::BasicBlock* SupertypeBlock;
			std::vector<llvm::BasicBlock*> targets;
			std::vector<llvm::IndirectBrInst*> sources;
		public:
			PWBoolOrTri CallTypeEq(NomBuilder& builder, PWType leftType, PWType rightType, PWSubstStack leftSubst, PWSubstStack rightSubst, bool optimistic = false, bool generatePtrEq = true);
			PWBoolOrTri CallIsSubtype(NomBuilder& builder, PWType leftType, PWType rightType, PWSubstStack leftSubst, PWSubstStack rightSubst, bool optimistic = false, bool generatePtrEq = true);
			PWBoolOrTri CallIsSupertype(NomBuilder& builder, PWType leftType, PWType rightType, PWSubstStack leftSubst, PWSubstStack rightSubst, bool optimistic = false, bool generatePtrEq = true);
			void Return(NomBuilder& builder, PWBoolOrTri retVal);
		};
	}
}
