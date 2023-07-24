#pragma once
#include "RTTypeHead.h"
#include "RTConcreteType.h"
PUSHDIAGSUPPRESSION
#include "llvm/IR/IRBuilder.h"
POPDIAGSUPPRESSION
#include "CompileEnv.h"
#include "ARTRep.h"

namespace Nom
{
	namespace Runtime
	{
		class NomTypeVar;
		enum class RTTypeVarFields : unsigned char { Head = 0, Index = 1, LowerBound = 2, UpperBound = 3 };
		class RTTypeVar : public RTConcreteType, public ARTRep<RTTypeVar, RTTypeVarFields>
		{
		public:
			static llvm::StructType *GetLLVMType();
			RTTypeVar(const char * entry) : ARTRep<RTTypeVar, RTTypeVarFields>(entry)
			{
			}
			RTTypeVar(const void * entry) : ARTRep<RTTypeVar, RTTypeVarFields>(entry)
			{
			}
			static llvm::Constant *GetConstant(size_t index, llvm::Constant *lowerBound, llvm::Constant *upperBound, const NomTypeVar *type);

			static llvm::Value* GenerateLoadIndex(NomBuilder& builder, llvm::Value* type);

		};
	}
}
