#pragma once
#include "PWrapper.h"
#include "NomBuilder.h"
#include <inttypes.h>
#include "CompileHelpers.h"
#include "PWInt.h"
#include "Metadata.h"
#include "PWPtr.h"

namespace Nom
{
	namespace Runtime
	{
		template <typename T> class PWRawArr : public PWPtr<T>
		{
		public:
			const PWInt32 size;
			static PWRawArr<T> Alloca(NomBuilder& builder, PWInt32 size, llvm::Twine name="buf")
			{
				return PWRawArr<T>(builder->CreateAlloca(T::GetLLVMType(), size, name), size);
			}

			PWRawArr(llvm::Value* wrapped, PWInt32 size=PWInt32(0,false)) : PWPtr(wrapped)
			{
			}

			PWRawArr<T> WithSize(PWInt32 newsize)
			{
				return PWRawArr<T>(wrapped, newsize);
			}

			PWRawArr<T> SubArr(NomBuilder& builder, PWInt32 start, llvm::Twine name = "subArr")
			{
				return PWRawArr<T>(builder->CreateGEP(T::GetLLVMType(), wrapped, { start }, name));
			}

			void IterateFromStart(NomBuilder& builder, llvm::BasicBlock* after, void (*makeBody)(NomBuilder&, T), bool nonempty=false, llvm::Twine bodyname="loopbody", llvm::MDNode* emptyMetadata=nullptr)
			{
				llvm::BasicBlock* curblock = builder->GetInsertBlock();
				llvm::BasicBlock* bblock = *body;
				if (bblock == nullptr)
				{
					bblock = llvm::BasicBlock::Create(builder->getContext(), bodyname, curblock->getParent());
					*body = bblock;
				}
				if (!nonempty)
				{
					PWBool isEmpty = builder->CreateICmpEQ(size, MakeInt32(0), "isempty");
					builder->CreateCondBr(isEmpty, after, bblock, emptyMetadata==nullptr? GetLikelySecondBranchMetadata() : emptyMetadata);
				}
				else
				{
					builder->CreateBr(bblock);
				}
				builder->SetInsertPoint(bblock);
				auto curPHI = builder->CreatePHI(wrapped->getType(), 2, "current");
				curPHI->addIncoming(wrapped, curblock);
				auto sizePHI = builder->CreatePHI(size->getType(), 2, "itemsLeft");
				sizePHI->addIncoming(size, curblock);
				makeBody(builder, T(curPHI));
				llvm::BasicBlock* cblock = builder->GetInsertBlock();
				PWInt32 nsize = builder->CreateSub(sizePHI, MakeIntLike(sizePHI, 1), "nextSize");
				sizePHI->addIncoming(nsize, cblock);
				PWArr narr = Get(curPHI, 1, "nextElem");
				curPHI->addIncoming(narr, cblock);
				PWBool anyLeft = builder->CreateICmpEQ(nsize, MakeInt32(0), "isdone");
				builder->CreateCondBr(anyLeft, bblock, after, GetLikelyFirstBranchMetadata());
			}

			void IterateFromEnd(NomBuilder& builder, PWInt32 size, llvm::BasicBlock* after, llvm::BasicBlock** body, T* curvar = nullptr, bool nonempty=false, llvm::Twine bodyname = "loopbody", llvm::MDNode* emptyMetadata = nullptr)
			{
				llvm::BasicBlock* curblock = builder->GetInsertBlock();
				llvm::BasicBlock* bblock = *body;
				if (bblock == nullptr)
				{
					bblock = llvm::BasicBlock::Create(builder->getContext(), bodyname, curblock->getParent());
					*body = bblock;
				}
				if (!nonempty)
				{
					PWBool isEmpty = builder->CreateICmpEQ(size, MakeInt32(0), "isempty");
					builder->CreateCondBr(isEmpty, after, bblock, emptyMetadata == nullptr ? GetLikelySecondBranchMetadata() : emptyMetadata);
				}
				else
				{
					builder->CreateBr(bblock);
				}
				builder->SetInsertPoint(bblock);
				auto sizePHI = builder->CreatePHI(size->getType(), 2, "itemsLeft");
				sizePHI->addIncoming(size, curblock);
				PWInt32 nsize = builder->CreateSub(sizePHI, MakeIntLike(sizePHI, 1), "nextSize");
				makeBody(builder, Get(builder, nsize, "current"));
				llvm::BasicBlock * cblock = builder->GetInsertBlock();
				sizePHI->addIncoming(nsize, cblock);
				PWBool anyLeft = builder->CreateICmpEQ(nsize, MakeInt32(0), "isdone");
				builder->CreateCondBr(anyLeft, bblock, after, GetLikelyFirstBranchMetadata());
			}
		};
		template<typename T> using PWArr = PWRawArr<T>;
	}
}