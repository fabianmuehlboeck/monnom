#pragma once

#include "llvm/IR/IRBuilder.h"
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
		public:
			NomBuilder() : builder(LLVMCONTEXT), pos(nullptr) {} //TODO: remove and fix call sites once position information is implemented
			NomBuilder(SourcePosRef pos) : builder(LLVMCONTEXT), pos(pos) {}
			~NomBuilder() {}

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

			operator llvm::IRBuilder<>& ()
			{
				return builder;
			}
		};
	}
}