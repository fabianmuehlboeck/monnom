#pragma once
#include "PWType.h"
#include "NomBuilder.h"
namespace Nom
{
	namespace Runtime
	{
		class PWInterface;
		class PWTypeArr;
		class PWNomType;
		class PWInstanceType : public PWType
		{
		public:
			static llvm::Type* GetLLVMType();
			PWInstanceType(llvm::Value* wrapped) : PWType(wrapped)
			{

			}
			void InitializeInstanceType(NomBuilder& builder, llvm::Value* hash, PWNomType nomtypeptr, PWInterface rtclassdesc, PWTypeArr ptrToTypeArgs);
			PWInterface ReadClassDescriptorLink(NomBuilder& builder);
			PWTypeArr TypeArgumentsPtr(NomBuilder& builder);
		};
	}
}
