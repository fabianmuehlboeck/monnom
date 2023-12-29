#pragma once
#include "ARTRep.h"
#include "NomMaybeType.h"
#include "NomBuilder.h"

namespace Nom
{
	namespace Runtime
	{
		enum class XRTMaybeTypeFields : unsigned char { Head = 0, PotentialType = 1 };
		class XRTMaybeType : public ARTRep<XRTMaybeType,XRTMaybeTypeFields>
		{
		public:
			static llvm::StructType* GetLLVMType();
			static llvm::Constant* GetConstant(llvm::Module& mod, const NomMaybeType* mbt);

			static uint64_t HeadOffset();
			static llvm::Value* GenerateReadPotentialType(NomBuilder& builder, llvm::Value* type);
		};
	}
}
