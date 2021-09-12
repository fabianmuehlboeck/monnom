#pragma once
#include "ARTRep.h"
#include "NomBuilder.h"

namespace Nom
{
	namespace Runtime
	{
		class NomLambda;
		enum class LambdaHeaderFields :unsigned char { StructValueHeader = 0, Fields = 1 };
		class LambdaHeader : public ARTRep<LambdaHeader, LambdaHeaderFields>
		{
		public:
			static llvm::StructType* GetLLVMType();
			static llvm::Value* GenerateReadField(NomBuilder& builder, NomLambda* lambda, llvm::Value* thisObj, int32_t fieldindex);
			static llvm::Value* GenerateReadField(NomBuilder& builder, llvm::Value* thisObj, int32_t fieldIndex);
			static llvm::Value* GenerateWriteField(NomBuilder& builder, llvm::Value* thisObj, int32_t argindex, llvm::Value* value);

			static llvm::Value* GenerateReadTypeArgument(NomBuilder& builder, llvm::Value* thisObj, int32_t argindex, const NomLambda* lambda);
			static llvm::Value* GeneratePointerToTypeArguments(NomBuilder& builder, llvm::Value* thisObj, const NomLambda* lambda);

			static void GenerateConstructorCode(NomBuilder& builder, llvm::ArrayRef<llvm::Value*> typeArguments, llvm::ArrayRef<llvm::Value*> arguments, llvm::Constant* descriptorRef, const NomLambda* lambda);
		};
	}
}