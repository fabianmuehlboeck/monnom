#include "RTTypeCheckGen.h"
#include "PWType.h"
#include "PWSubstStack.h"

using namespace std;
using namespace llvm;
namespace Nom
{
	namespace Runtime
	{
		PWBoolOrTri RTTypeCheckGen::CallTypeEq(NomBuilder& builder, PWType leftType, PWType rightType, PWSubstStack leftSubst, PWSubstStack rightSubst, bool optimistic, bool generatePtrEq)
		{
			BasicBlock* sourceBlock = builder->GetInsertBlock();
			if (TypeEqBlock == nullptr)
			{
				TypeEqBlock = BasicBlock::Create(builder->getContext(), "typeEqRec", builder.GetFunction());
				targets.push_back(TypeEqBlock);

				for (auto source : sources)
				{
					source->addDestination(TypeEqBlock);
				}

				builder->SetInsertPoint(sourceBlock);
			}
			auto br = builder->CreateBr(TypeEqBlock);
		}
		PWBoolOrTri RTTypeCheckGen::CallIsSubtype(NomBuilder& builder, PWType leftType, PWType rightType, PWSubstStack leftSubst, PWSubstStack rightSubst, bool optimistic, bool generatePtrEq)
		{
			return PWBoolOrTri();
		}
		PWBoolOrTri RTTypeCheckGen::CallIsSupertype(NomBuilder& builder, PWType leftType, PWType rightType, PWSubstStack leftSubst, PWSubstStack rightSubst, bool optimistic, bool generatePtrEq)
		{
			return PWBoolOrTri();
		}
		void RTTypeCheckGen::Return(NomBuilder& builder, PWBoolOrTri retVal)
		{
			BasicBlock* sourceBlock = builder->GetInsertBlock();
			auto br = builder->CreateIndirectBr();
			sources.push_back(br);
			for (auto target : targets)
			{
				br->addDestination(target);
			}
		}
	}
}
