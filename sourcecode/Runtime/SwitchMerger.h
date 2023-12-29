#pragma once
#include "NomBuilder.h"
PUSHDIAGSUPPRESSION
#include "llvm/IR/BasicBlock.h"
#include "llvm/ADT/Twine.h"
#include "llvm/IR/Instructions.h"
POPDIAGSUPPRESSION
#include "PWPhi.h"

namespace Nom
{
	namespace Runtime
	{
		template<typename T, unsigned int size>
		class SwitchMerger
		{
			T results[size];
			llvm::BasicBlock* blocks[size];
			unsigned int count = 0;

		public:
			SwitchMerger() {

			}
			void AddResult(NomBuilder& builder, T val)
			{
				results[count] = val;
				blocks[count] = builder->GetInsertBlock();
				count++;
			}
			T Merge(NomBuilder& builder, llvm::Twine mergeName = "merge")
			{
				if (count == 0)
				{
					throw new std::exception();
				}
				if (count == 1)
				{
					builder->SetInsertPoint(blocks[0]);
					return results[0];
				}
				else
				{
					llvm::BasicBlock* mergeBlock = llvm::BasicBlock::Create(builder->getContext(), mergeName, builder.GetFunction());
					builder->SetInsertPoint(mergeBlock);
					PWPhi<T> phi = PWPhi<T>::Create(builder, count, mergeName.concat("Phi"));
					for (unsigned int i = 0; i < count; i++)
					{
						phi->addIncoming(results[i], blocks[i]);
						builder->SetInsertPoint(blocks[i]);
						builder->CreateBr(mergeBlock);
					}
					builder->SetInsertPoint(mergeBlock);
					return phi;
				}
			}
		};
	}
}