#pragma once
#include "NomBuilder.h"
PUSHDIAGSUPPRESSION
#include "llvm/IR/BasicBlock.h"
#include "llvm/ADT/Twine.h"
#include "llvm/IR/Instructions.h"
#include "llvm/ADT/SmallVector.h"
POPDIAGSUPPRESSION
#include "PWPhi.h"

namespace Nom
{
	namespace Runtime
	{
		template<typename T, unsigned int size>
		class SwitchMerger
		{
			llvm::SmallVector<std::pair<T, llvm::BasicBlock*>, size> data;

		public:
			SwitchMerger() {

			}
			void AddResult(NomBuilder& builder, T val)
			{
				data.push_back(std::make_pair(val, builder->GetInsertBlock()));
			}
			T Merge(NomBuilder& builder, llvm::Twine mergeName = "merge")
			{
				if (data.size() == 0)
				{
					throw new std::exception();
				}
				if (data.size() == 1)
				{
					builder->SetInsertPoint(data[0].second);
					return data[0].first;
				}
				else
				{
					llvm::BasicBlock* mergeBlock = llvm::BasicBlock::Create(builder->getContext(), mergeName, builder.GetFunction());
					builder->SetInsertPoint(mergeBlock);
					PWPhi<T> phi = PWPhi<T>::Create(builder, static_cast<unsigned int>(data.size()), mergeName.concat("Phi"));
					for (size_t i = 0; i < data.size(); i++)
					{
						phi->addIncoming(data[i].first, data[i].second);
						builder->SetInsertPoint(data[i].second);
						builder->CreateBr(mergeBlock);
					}
					builder->SetInsertPoint(mergeBlock);
					return phi;
				}
			}
		};
	}
}
