#include "RTSubstStack.h"
#include "RTSubtyping.h"
#include "CompileHelpers.h"
#include "RTTypeVar.h"
#include "PWSubstStack.h"
#include "PWTypeVar.h"
#include "llvm/Support/AtomicOrdering.h"

using namespace llvm;
using namespace std;
namespace Nom
{
	namespace Runtime
	{
		llvm::StructType* RTSubstStack::GetLLVMType()
		{
			return RTSubtyping::TypeArgumentListStackType();
		}
		llvm::Value* RTSubstStack::Pop(NomBuilder& builder, llvm::Value* substStack, llvm::Value* typeVar, llvm::Value** newStack)
		{
			auto ss = PWSubstStack(substStack);
			PWType ret = PWSubstStack(substStack).Pop(builder, typeVar, newStack!=nullptr?&ss:nullptr);
			if (newStack != nullptr)
			{
				*newStack = ss.wrapped;
			}
			return ret;
		}
		RTSubstStackValue::RTSubstStackValue(NomBuilder& builder, llvm::Value* typelist, llvm::Value* previousStack, llvm::ConstantInt* typeListSize, llvm::Value* typeListOrigPtr) : typeList(typeListOrigPtr), typeListSize(typeListSize)
		{
			stack = builder->CreateAlloca(RTSubstStack::GetLLVMType(), MakeInt32(1), "substStack");
			builder->CreateIntrinsic(llvm::Intrinsic::lifetime_start, { POINTERTYPE }, { MakeInt<int64_t>(GetNomJITDataLayout().getTypeAllocSize(RTSubstStack::GetLLVMType())), builder->CreatePointerCast(stack, POINTERTYPE) });
			MakeInvariantStore(builder, previousStack==nullptr?ConstantPointerNull::get(RTSubstStack::GetLLVMType()->getPointerTo()):previousStack, RTSubstStack::GetLLVMType(), stack, MakeInt32(TypeArgumentListStackFields::Next));
			MakeInvariantStore(builder, typelist, RTSubstStack::GetLLVMType(), stack, MakeInt32(TypeArgumentListStackFields::Types));
			invariantID=builder->CreateIntrinsic(llvm::Intrinsic::invariant_start, {POINTERTYPE}, { MakeInt<int64_t>(GetNomJITDataLayout().getTypeAllocSize(RTSubstStack::GetLLVMType())), builder->CreatePointerCast(stack, POINTERTYPE) });
			if (typeListSize != nullptr)
			{
				builder->CreateIntrinsic(llvm::Intrinsic::lifetime_start, { POINTERTYPE }, { builder->CreateZExt(typeListSize, inttype(64)), builder->CreatePointerCast(typeListOrigPtr, POINTERTYPE) });
			}
		}



		RTSubstStackValue::~RTSubstStackValue()
		{
			if (!released)
			{
				throw new std::exception();
			}
		}
		void RTSubstStackValue::MakeTypeListInvariant(NomBuilder& builder)
		{
			TLinvariantID = builder->CreateIntrinsic(llvm::Intrinsic::invariant_start, { POINTERTYPE }, { builder->CreateZExt(typeListSize, inttype(64)), builder->CreatePointerCast(typeList, POINTERTYPE) });
		}
		void RTSubstStackValue::MakeReleaseBlocks(NomBuilder& builder, BasicBlock* targetBlock1, BasicBlock** newTargetBlock1, BasicBlock* targetBlock2, BasicBlock** newTargetBlock2, BasicBlock* targetBlock3, BasicBlock** newTargetBlock3)
		{
			auto origBlock = builder->GetInsertBlock();
			
			StringRef tbn = targetBlock1->getName();

			BasicBlock* ntb1 = BasicBlock::Create(LLVMCONTEXT, tbn, origBlock->getParent());
			builder->SetInsertPoint(ntb1);
			MakeRelease(builder);
			builder->CreateBr(targetBlock1);
			*newTargetBlock1 = ntb1;

			if (targetBlock2 != nullptr)
			{
				BasicBlock* ntb2;
				if (targetBlock2 == targetBlock1)
				{
					ntb2 = ntb1;
				}
				else
				{
					tbn = targetBlock2->getName();
					ntb2 = BasicBlock::Create(LLVMCONTEXT, tbn, origBlock->getParent());
					builder->SetInsertPoint(ntb2);
					MakeRelease(builder);
					builder->CreateBr(targetBlock2);
				}
				*newTargetBlock2 = ntb2;
				if (targetBlock3 != nullptr)
				{
					if (targetBlock3 == targetBlock1)
					{
						*newTargetBlock3 = ntb1;
					}
					else if (targetBlock3 == targetBlock2)
					{
						*newTargetBlock3 = ntb2;
					}
					else
					{
						tbn = targetBlock3->getName();
						BasicBlock* ntb3 = BasicBlock::Create(LLVMCONTEXT, tbn, origBlock->getParent());
						builder->SetInsertPoint(ntb3);
						MakeRelease(builder);
						builder->CreateBr(targetBlock3);
						*newTargetBlock3 = ntb3;
					}
				}
			}

			if (origBlock != nullptr)
			{
				builder->SetInsertPoint(origBlock);
			}
			released = true;
		}
		void RTSubstStackValue::MakeRelease(NomBuilder& builder)
		{
			builder->CreateIntrinsic(llvm::Intrinsic::invariant_end, {POINTERTYPE}, { invariantID, MakeInt<int64_t>(GetNomJITDataLayout().getTypeAllocSize(RTSubstStack::GetLLVMType())), builder->CreatePointerCast(stack, POINTERTYPE) });
			builder->CreateIntrinsic(llvm::Intrinsic::lifetime_end, {POINTERTYPE}, { MakeInt<int64_t>(GetNomJITDataLayout().getTypeAllocSize(RTSubstStack::GetLLVMType())), builder->CreatePointerCast(stack, POINTERTYPE) });
			if (typeListSize != nullptr)
			{
				if (TLinvariantID != nullptr)
				{
					builder->CreateIntrinsic(llvm::Intrinsic::invariant_end, { POINTERTYPE }, { TLinvariantID, builder->CreateZExt(typeListSize, inttype(64)), builder->CreatePointerCast(typeList, POINTERTYPE) });
				}
				builder->CreateIntrinsic(llvm::Intrinsic::lifetime_end, { POINTERTYPE }, { builder->CreateZExt(typeListSize, inttype(64)), builder->CreatePointerCast(typeList, POINTERTYPE) });
			}
			released = true;
		}
	}
}