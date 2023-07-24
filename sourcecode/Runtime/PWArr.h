#pragma once
PUSHDIAGSUPPRESSION
#include "llvm/IR/Constants.h"
#include "llvm/ADT/Twine.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/AtomicOrdering.h"
POPDIAGSUPPRESSION
#include "PWrapper.h"
#include "NomBuilder.h"
#include <inttypes.h>
#include "PWInt.h"
#include "Metadata.h"
#include "PWPtr.h"
#include <functional>
#include "PWPhi.h"

namespace Nom
{
	namespace Runtime
	{
		namespace ZZ
		{
			template <typename T, typename S, typename P> class PWArrTemplate : public P
			{
			public:
				const PWInt32 size;
				const PWCInt64 csize;
				mutable llvm::Value* invariantID = nullptr;
				static llvm::Type* GetLLVMType()
				{
					llvm::Value* sval = size;
					if (auto* ci = dyn_cast<llvm::ConstantInt>(sval))
					{
						return llvm::ArrayType::get(S::GetElemType(), ci->getValue());
					}
					return llvm::ArrayType::get(S::GetElemType(), 0);
				}
				static llvm::Type* GetWrappedLLVMType()
				{
					return NLLVMPointer(GetLLVMType());
				}
				static S Alloca(NomBuilder& builder, PWInt32 size, llvm::Twine name = "buf")
				{
					auto alloced = builder->CreateAlloca(S::GetElemType(), size, name);
					auto csize = MakePWInt64(-1);
					builder->CreateIntrinsic(llvm::Intrinsic::lifetime_start, { POINTERTYPE }, { csize, alloced });
					return S(alloced, size, csize);
				}
				static S Alloca(NomBuilder& builder, llvm::Twine name, size_t size)
				{
					auto pwsz = MakePWUInt32(size);
					auto alloced = builder->CreateAlloca(S::GetElemType(), size, name);
					auto csize = MakePWUInt64(GetNomJITDataLayout().getTypeAllocSize(S::GetElemType()) * size);
					builder->CreateIntrinsic(llvm::Intrinsic::lifetime_start, { POINTERTYPE }, { csize, alloced });
					return S(alloced, pwsz, csize);
				}
				PWArrTemplate(llvm::Value* _wrapped, PWInt32 _size = PWInt32(0, false), PWCInt64 _csize = MakePWInt64(-2)) : P(_wrapped), size(_size), csize(_csize)
				{
				}
				S WithSize(PWInt32 newsize) const
				{
					return S(this->wrapped, newsize);
				}
				S WithSizeFromEnd(NomBuilder& builder, PWInt32 newsize) const
				{
					return S(builder->CreateGEP(S::GetElemType(), this->wrapped, builder->CreateNeg(newsize), "arrayOrig"), newsize);
				}

				S SubArr(NomBuilder& builder, PWInt32 start, llvm::Twine name = "subArr") const
				{
					return S(builder->CreateGEP(S::GetElemType(), this->wrapped, { start }, name), size.Subtract(builder, start));
				}
				P ElemAt(NomBuilder& builder, PWInt32 index, llvm::Twine name = "elemAt") const
				{
					return P(builder->CreateGEP(S::GetElemType(), this->wrapped, { index }, name));
				}
				T LoadElemAt(NomBuilder& builder, PWInt32 index, llvm::Twine name = "elem") const
				{
					return S::LoadElement(builder, ElemAt(builder, index), name);
				}
				T InvariantLoadElemAt(NomBuilder& builder, PWInt32 index, llvm::Twine name = "elem") const
				{
					return S::InvariantLoadElement(builder, ElemAt(builder, index), name);
				}

				void IterateFromStart(NomBuilder& builder, llvm::BasicBlock* after, std::function<void(NomBuilder&, T, PWInt32)> makeBody, bool nonempty = false, llvm::Twine bodyname = "loopbody", llvm::MDNode* emptyMetadata = nullptr) const
				{
					llvm::BasicBlock* curblock = builder->GetInsertBlock();
					llvm::BasicBlock* bblock = llvm::BasicBlock::Create(builder->getContext(), bodyname, curblock->getParent());
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
					auto indexPHI = builder->CreatePHI(size->getType(), 2, "iterIndex");
					indexPHI->addIncoming(MakeIntLike(size, 0), curblock);
					auto curItem = LoadElemAt(builder, indexPHI, "iterElem");
					makeBody(builder, curItem, indexPHI);
					llvm::BasicBlock* cblock = builder->GetInsertBlock();
					PWInt32 nIndex = builder->CreateAdd(indexPHI, MakeIntLike(indexPHI, 1), "nextIndex");
					indexPHI->addIncoming(nIndex, cblock);
					PWBool isDone = builder->CreateICmpEQ(size, nIndex, "isdone");
					CreateExpect(builder, isDone, MakeUInt(1, 0));
					builder->CreateCondBr(isDone, after, bblock, GetLikelySecondBranchMetadata());
				}

				void IterateFromEnd(NomBuilder& builder, llvm::BasicBlock* after, std::function<void(NomBuilder&, T, PWInt32)> makeBody, bool nonempty = false, llvm::Twine bodyname = "loopbody", llvm::MDNode* emptyMetadata = nullptr) const
				{
					llvm::BasicBlock* curblock = builder->GetInsertBlock();
					llvm::BasicBlock* bblock = llvm::BasicBlock::Create(builder->getContext(), bodyname, curblock->getParent());
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
					makeBody(builder, LoadElemAt(builder, nsize, "iterElem"), nsize);
					llvm::BasicBlock* cblock = builder->GetInsertBlock();
					sizePHI->addIncoming(nsize, cblock);
					PWBool isDone = builder->CreateICmpEQ(nsize, MakeInt32(0), "isdone");
					CreateExpect(builder, isDone, MakeUInt(1, 0));
					builder->CreateCondBr(anyLeft, after, bblock, GetLikelySecondBranchMetadata());
				}

				void MakeInvariant(NomBuilder& builder) const
				{
					PWCInt64 sz64 = this->csize;
					if (sz64.wrapped != MakePWInt64(-2))
					{
						invariantID = builder->CreateIntrinsic(llvm::Intrinsic::invariant_start, { POINTERTYPE }, { sz64.wrapped, this->wrapped });
					}
				}
				void Release(NomBuilder& builder) const
				{
					PWCInt64 sz64 = this->csize;
					if (sz64.wrapped != MakePWInt64(-2))
					{
						if (invariantID != nullptr)
						{
							builder->CreateIntrinsic(llvm::Intrinsic::invariant_end, { POINTERTYPE }, { invariantID, sz64, this->wrapped });
						}
						builder->CreateIntrinsic(llvm::Intrinsic::lifetime_end, { POINTERTYPE }, { sz64, this->wrapped });
					}
				}
			};

			template <typename T, typename S, typename P> class PWInverseArrTemplate : public P
			{
			public:
				const PWInt32 size;
				const PWCInt64 csize;
				mutable llvm::Value* invariantID = nullptr;
				static llvm::Type* GetLLVMType()
				{
					return llvm::ArrayType::get(S::GetElemType(), 0);
				}
				static llvm::Type* GetWrappedLLVMType()
				{
					return NLLVMPointer(GetLLVMType());
				}
				static S Alloca(NomBuilder& builder, PWInt32 size, llvm::Twine name = "buf")
				{
					auto alloced = builder->CreateAlloca(S::GetElemType(), size);
					auto csize = MakePWInt64(-1);
					builder->CreateIntrinsic(llvm::Intrinsic::lifetime_start, { POINTERTYPE }, { csize, alloced });
					return S(builder->CreateGEP(S::GetElemType(), alloced, { size.operator llvm::Value * () }, name), size, csize);
				}
				static S Alloca(NomBuilder& builder, llvm::Twine name, size_t size)
				{
					auto pwsz = MakePWUInt32(size);
					auto alloced = builder->CreateAlloca(S::GetElemType(), size, name);
					auto csize = MakePWUInt64(GetNomJITDataLayout().getTypeAllocSize(S::GetElemType()) * size);
					builder->CreateIntrinsic(llvm::Intrinsic::lifetime_start, { POINTERTYPE }, { csize, alloced });
					return S(builder->CreateGEP(S::GetElemType(), alloced, { size.operator llvm::Value * () }, name), pwsz, csize);
				}
				PWInverseArrTemplate(llvm::Value* _wrapped, PWInt32 _size = PWInt32(0, false), PWCInt64 _csize = MakePWInt64(-2)) : P(_wrapped), size(_size), csize(_csize)
				{
				}
				S WithSize(PWInt32 newsize) const
				{
					return S(this->wrapped, newsize);
				}
				S WithSize(uint64_t newsize, bool issigned = false) const
				{
					return S(this->wrapped, PWInt32(newsize, issigned));
				}
				S WithSizeFromFront(NomBuilder& builder, PWInt32 newsize) const
				{
					return S(builder->CreateGEP(S::GetElemType(), this->wrapped, builder->CreateNeg(newsize), "arrayOrig"), newsize);
				}

				S SubArr(NomBuilder& builder, PWInt32 start, llvm::Twine name = "subArr") const
				{
					return S(builder->CreateGEP(S::GetElemType(), this->wrapped, { builder->CreateNeg(start) }, name), size.Subtract(builder, start));
				}
				P ElemAt(NomBuilder& builder, PWInt32 index, llvm::Twine name = "elemAt") const
				{
					return P(builder->CreateGEP(S::GetElemType(), this->wrapped, { builder->CreateSub(MakeInt32(-1),index) }, name));
				}
				T LoadElemAt(NomBuilder& builder, PWInt32 index, llvm::Twine name = "elem") const
				{
					return S::LoadElement(builder, ElemAt(builder, index), name);
				}
				T InvariantLoadElemAt(NomBuilder& builder, PWInt32 index, llvm::Twine name = "elem") const
				{
					return S::InvariantLoadElement(builder, ElemAt(builder, index), name);
				}

				void IterateFromStart(NomBuilder& builder, llvm::BasicBlock* after, std::function<void(NomBuilder&, T, PWInt32)> makeBody, bool nonempty = false, llvm::Twine bodyname = "loopbody", llvm::MDNode* emptyMetadata = nullptr) const
				{
					llvm::BasicBlock* curblock = builder->GetInsertBlock();
					llvm::BasicBlock* bblock = llvm::BasicBlock::Create(builder->getContext(), bodyname, curblock->getParent());
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
					auto indexPHI = builder->CreatePHI(size->getType(), 2, "iterIndex");
					indexPHI->addIncoming(MakeIntLike(size, 0), curblock);
					auto curItem = LoadElemAt(builder, indexPHI, "iterElem");
					makeBody(builder, curItem, indexPHI);
					llvm::BasicBlock* cblock = builder->GetInsertBlock();
					PWInt32 nIndex = builder->CreateAdd(indexPHI, MakeIntLike(indexPHI, 1), "nextIndex");
					indexPHI->addIncoming(nIndex, cblock);
					PWBool isDone = builder->CreateICmpEQ(size, nIndex, "isdone");
					CreateExpect(builder, isDone, MakeUInt(1, 0));
					builder->CreateCondBr(isDone, after, bblock, GetLikelySecondBranchMetadata());
				}

				void IterateFromEnd(NomBuilder& builder, llvm::BasicBlock* after, std::function<void(NomBuilder&, T, PWInt32)> makeBody, bool nonempty = false, llvm::Twine bodyname = "loopbody", llvm::MDNode* emptyMetadata = nullptr) const
				{
					llvm::BasicBlock* curblock = builder->GetInsertBlock();
					llvm::BasicBlock* bblock = llvm::BasicBlock::Create(builder->getContext(), bodyname, curblock->getParent());
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
					makeBody(builder, LoadElemAt(builder, nsize, "iterElem"), nsize);
					llvm::BasicBlock* cblock = builder->GetInsertBlock();
					sizePHI->addIncoming(nsize, cblock);
					PWBool isDone = builder->CreateICmpEQ(nsize, MakeInt32(0), "isdone");
					CreateExpect(builder, isDone, MakeUInt(1, 0));
					builder->CreateCondBr(anyLeft, after, bblock, GetLikelySecondBranchMetadata());
				}
				void MakeInvariant(NomBuilder& builder) const
				{
					PWCInt64 sz64 = this->csize;
					if (sz64.wrapped != MakePWInt64(-2))
					{
						invariantID = builder->CreateIntrinsic(llvm::Intrinsic::invariant_start, { POINTERTYPE }, { sz64, builder->CreateGEP(arrtype(S::GetElemType(), 0), this->wrapped, this->size.Add1(builder).Neg(builder)) });
					}
				}
				void Release(NomBuilder& builder) const
				{
					PWCInt64 sz64 = this->csize;
					if (sz64.wrapped != MakePWInt64(-2))
					{
						auto alloced = builder->CreateGEP(arrtype(S::GetElemType(), 0), this->wrapped, this->size.Add1(builder).Neg(builder));
						if (invariantID != nullptr)
						{
							builder->CreateIntrinsic(llvm::Intrinsic::invariant_end, { POINTERTYPE }, { invariantID, sz64, alloced });
						}
						builder->CreateIntrinsic(llvm::Intrinsic::lifetime_end, { POINTERTYPE }, { sz64, alloced });
					}
				}
			};
		}
		template <typename T> class PWFlatArr : public ZZ::PWArrTemplate<T, PWFlatArr<T>, T>
		{
		public:
			PWFlatArr(llvm::Value* _wrapped, PWInt32 _size = PWInt32(0, false)) : ZZ::PWArrTemplate<T, PWFlatArr<T>, T>(_wrapped, _size)
			{

			}
			static llvm::Type* GetElemType()
			{
				return T::GetLLVMType();
			}
			static T LoadElement(NomBuilder& builder, T t, llvm::Twine name = "ptr")
			{
				return t;
			}
			static T InvariantLoadElement(NomBuilder& builder, T pt, llvm::Twine name = "ptr")
			{
				return pt;
			}
		};
		template <typename T> class PWArr : public ZZ::PWArrTemplate<T, PWArr<T>, PWPtr<T> >
		{
		public:
			PWArr(llvm::Value* _wrapped, PWInt32 _size = PWInt32(0, false)) : ZZ::PWArrTemplate<T, PWArr<T>, PWPtr<T> >(_wrapped, _size)
			{

			}
			static llvm::Type* GetElemType()
			{
				return NLLVMPointer(T::GetLLVMType());
			}
			static T LoadElement(NomBuilder& builder, PWPtr<T> pt, llvm::Twine name = "ptr")
			{
				return pt.Load(builder, name);
			}
			static T InvariantLoadElement(NomBuilder& builder, PWPtr<T> pt, llvm::Twine name = "ptr")
			{
				return pt.InvariantLoad(builder, name);
			}
		};
		template <typename T> class PWFlatInverseArr : public ZZ::PWInverseArrTemplate<T, PWFlatInverseArr<T>, T>
		{
		public:
			PWFlatInverseArr(llvm::Value* _wrapped, PWInt32 _size = PWInt32(0, false), PWCInt64 _csize = MakePWInt64(-2)) : ZZ::PWInverseArrTemplate<T, PWFlatInverseArr<T>, T>(_wrapped, _size, _csize)
			{

			}
			static llvm::Type* GetElemType()
			{
				return T::GetLLVMType();
			}
			static T LoadElement(NomBuilder& builder, T t, llvm::Twine name = "ptr")
			{
				return t;
			}
			static T InvariantLoadElement(NomBuilder& builder, T pt, llvm::Twine name = "ptr")
			{
				return pt;
			}
		};
		template <typename T> class PWInverseArr : public ZZ::PWInverseArrTemplate<T, PWInverseArr<T>, PWPtr<T> >
		{
		public:
			PWInverseArr(llvm::Value* _wrapped, PWInt32 _size = PWInt32(0, false), PWCInt64 _csize = MakePWInt64(-2)) : ZZ::PWInverseArrTemplate<T, PWInverseArr<T>, PWPtr<T> >(_wrapped, _size, _csize)
			{

			}
			static llvm::Type* GetElemType()
			{
				return NLLVMPointer(T::GetLLVMType());
			}
			static T LoadElement(NomBuilder& builder, PWPtr<T> pt, llvm::Twine name = "ptr")
			{
				return pt.Load(builder, name);
			}
			static T InvariantLoadElement(NomBuilder& builder, PWPtr<T> pt, llvm::Twine name = "ptr")
			{
				return pt.InvariantLoad(builder, name);
			}
		};
	}
}
