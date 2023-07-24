#pragma once
PUSHDIAGSUPPRESSION
#include "llvm/IR/IRBuilder.h"
POPDIAGSUPPRESSION

#include "Context.h"
#include "SourcePos.h"

namespace Nom
{
	namespace Runtime
	{
		class NomBuilder
		{
		private:
			llvm::IRBuilder<> builder;
			SourcePosRef pos;
			char** firstpageptr = nullptr;
			char* curallocpos = nullptr;
			char* curallocend = nullptr;
			char** curallocstart = nullptr;

			void AllocNewPage();
		public:
			NomBuilder() : builder(LLVMCONTEXT), pos(nullptr) {
				
			} //TODO: remove and fix call sites once position information is implemented
			NomBuilder(SourcePosRef spos) : builder(LLVMCONTEXT), pos(spos) {}
			~NomBuilder();

			llvm::IRBuilder<>* operator*()
			{
				return &builder;
			}
			llvm::IRBuilder<>* operator->()
			{
				return &builder;
			}

			void AdjustPos(SourcePosRef newPos) { pos = newPos; }
			SourcePosRef GetPos() const { return pos; }

			char* Malloc(size_t size);
			template<typename T> T* Alloc()
			{
				return reinterpret_cast<T*>(Malloc(sizeof(T)));
			}

			operator llvm::IRBuilder<>& ()
			{
				return builder;
			}
		};
	}
}
