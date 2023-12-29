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
		enum class XRTTypeVarFields : unsigned char { Head = 0, Index = 1, LowerBound = 2, UpperBound = 3 };
		class XRTTypeVar : public RTConcreteType, public ARTRep<XRTTypeVar, XRTTypeVarFields>
		{
		public:
			static llvm::StructType *GetLLVMType();
			XRTTypeVar(const char * entry) : ARTRep<XRTTypeVar, XRTTypeVarFields>(entry)
			{
			}
			XRTTypeVar(const void * entry) : ARTRep<XRTTypeVar, XRTTypeVarFields>(entry)
			{
			}
			static llvm::Constant *GetConstant(size_t index, llvm::Constant *lowerBound, llvm::Constant *upperBound, const NomTypeVar *type);

			static llvm::Value* GenerateLoadIndex(NomBuilder& builder, llvm::Value* type);

		};
	}
}
