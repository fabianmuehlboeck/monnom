#pragma once
#include "PWType.h"
#include "NomBuilder.h"
#include "PWTypeArr.h"
namespace Nom
{
	namespace Runtime
	{
		class PWInterface;
		class PWNomType;
		class PWInstanceType : public PWType
		{
		public:
			static llvm::Type* GetLLVMType();
			static llvm::Type* GetWrappedLLVMType();
			PWInstanceType(llvm::Value* _wrapped) : PWType(_wrapped)
			{

			}
			void InitializeInstanceType(NomBuilder& builder, llvm::Value* hash, PWNomType nomtypeptr, PWInterface rtclassdesc, PWTypeArr ptrToTypeArgs);
			PWInterface ReadClassDescriptorLink(NomBuilder& builder);
			PWTypeArr TypeArgumentsPtr(NomBuilder& builder);
		};
	}
}
