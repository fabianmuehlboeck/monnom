#pragma once
#include "llvm/IR/Type.h"
#include "llvm/IR/Value.h"
#include "NomBuilder.h"

namespace Nom
{
	namespace Runtime
	{
		enum class TypeArgumentListStackFields :unsigned char { Next = 0, Types = 1 };

		class RTSubstStackValue
		{
		public:
			llvm::Value* stack = nullptr;
			llvm::Value* invariantID = nullptr;
			llvm::Value* TLinvariantID = nullptr;
			llvm::Value* typeList = nullptr;
			llvm::ConstantInt* typeListSize = nullptr;
			bool released = false;
			RTSubstStackValue(NomBuilder& builder, llvm::Value* typelist, llvm::Value* previousStack = nullptr, llvm::ConstantInt* typeListSize = nullptr, llvm::Value* typeListOrigPtr = nullptr);
			RTSubstStackValue(RTSubstStackValue& other) = delete;
			RTSubstStackValue(RTSubstStackValue&& other) = default;
			~RTSubstStackValue();
			llvm::Value* operator*()
			{
				return stack;
			}
			llvm::Value* operator->()
			{
				return stack;
			}

			void MakeTypeListInvariant(NomBuilder& builder);

			operator llvm::Value* ()
			{
				return stack;
			}

			void MakeReleaseBlocks(NomBuilder& builder, llvm::BasicBlock* targetBlock1, llvm::BasicBlock** newTargetBlock1, llvm::BasicBlock* targetBlock2 = nullptr, llvm::BasicBlock** newTargetBlock2 = nullptr, llvm::BasicBlock* targetBlock3 = nullptr, llvm::BasicBlock** newTargetBlock3 = nullptr);
			void MakeRelease(NomBuilder& builder);
		};

		class RTSubstStack
		{
		public:
			static llvm::StructType* GetLLVMType();

			//static RTSubstStackValue Push(NomBuilder& builder, llvm::Value* substStack, llvm::Value* typelist);
			static llvm::Value* Pop(NomBuilder& builder, llvm::Value* substStack, llvm::Value* typeVar, llvm::Value** newStack);
		};
	}
}