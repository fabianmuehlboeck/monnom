#pragma once
#include "../../Runtime_Data/ARTRep.h"
#include "../../Intermediate_Representation/Types/NomMaybeType.h"
#include "../../Common/NomBuilder.h"

namespace Nom
{
	namespace Runtime
	{
		enum class RTMaybeTypeFields : unsigned char { Head = 0, PotentialType = 1 };
		class RTMaybeType : public ARTRep<RTMaybeType,RTMaybeTypeFields>
		{
		public:
			static llvm::StructType* GetLLVMType();
			static llvm::Constant* GetConstant(llvm::Module& mod, const NomMaybeType* mbt);

			static uint64_t HeadOffset();
			static llvm::Value* GenerateReadPotentialType(NomBuilder& builder, llvm::Value* type);
		};
	}
}