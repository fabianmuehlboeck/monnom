#pragma once
#include "RTTypeHead.h"
#include "RTClass.h"

namespace Nom
{
	namespace Runtime
	{
		enum class RTInstanceTypeFields : unsigned char { Head = 0, Class = 1, TypeArgs = 2 };
		class RTInstanceType : public ARTRep<RTInstanceType, RTInstanceTypeFields>
		{
		private:

		public:
			static llvm::StructType* GetLLVMType();
			static void CreateInitialization(NomBuilder &builder, llvm::Module &mod, llvm::Value* ptr, llvm::Value* hash, llvm::Value* nomtypeptr, llvm::Value* rtclassdesc, llvm::Value* ptrToTypeArg);

			static llvm::Value* GenerateReadClassDescriptorLink(NomBuilder& builder, llvm::Value* type);
			static llvm::Value* GetTypeArgumentsPtr(NomBuilder& builder, llvm::Value* type);
		};
	}
}
