#pragma once
#include "PWrapper.h"
#include "NomBuilder.h"
#include "NomType.h"
#include "CompileHelpers.h"
#include "llvm/IR/Constants.h"

namespace Nom
{
	namespace Runtime
	{
		class PWNomType : public PWrapper
		{
		public:
			PWNomType(llvm::Value* _wrapped) : PWrapper(_wrapped)
			{

			}
			PWNomType(NomType* tp) : PWrapper(llvm::ConstantExpr::getIntToPtr(llvm::ConstantInt::get(numtype(intptr_t), reinterpret_cast<uintptr_t>(tp)), llvm::PointerType::get(LLVMCONTEXT, 0)))
			{

			}
		};
	}
}
