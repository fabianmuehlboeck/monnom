#pragma once
#include "ARTRep.h"
#include "llvm/IR/Type.h"
#include "NomBuilder.h"
namespace Nom
{
	namespace Runtime
	{
		class NomStruct;
		enum class RTSTableFields : unsigned char { Signature = 0, LambdaMethod = 1, LambdaDispatcher = 2, TypeArgCount = 3, ArgCount = 4, NomIRLink = 5 };
		class RTSTable : public ARTRep<RTSTable, RTSTableFields>
		{
		public:
			static llvm::StructType* GetLLVMType();

			static llvm::Constant* GenerateConstant(llvm::Constant* signature, llvm::Constant* functionPtr, llvm::Constant* dispatcherPtr, llvm::Constant* lambdaTypeArgCount, llvm::Constant* lambdaArgCount, llvm::Constant* irlink);

			static llvm::Value* GenerateReadSignature(NomBuilder& builder, llvm::Value* descriptor);
			static llvm::Value* GenerateReadLambdaMethod(NomBuilder& builder, llvm::Value* descriptor);
			static llvm::Value* GenerateReadLambdaDispatcher(NomBuilder& builder, llvm::Value* descriptor);
			static llvm::Value* GenerateReadLambdaTypeArgCount(NomBuilder& builder, llvm::Value* descriptor);
			static llvm::Value* GenerateReadLambdaArgCount(NomBuilder& builder, llvm::Value* descriptor);
			static llvm::Value* GenerateReadNomIRLink(NomBuilder& builder, llvm::Value* descriptor);
		};
	}
}