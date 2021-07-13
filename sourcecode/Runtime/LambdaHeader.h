#pragma once
#include "ARTRep.h"
#include "NomBuilder.h"

namespace Nom
{
	namespace Runtime
	{
		class NomLambda;
		enum class LambdaHeaderFields :unsigned char { CastTypeArgs = 0, RefValueHeader = 1, RawInvokePointerSpace = 2, LambdaDescriptor = 3, TypeArgs = 4, Fields = 5 };
		class LambdaHeader : public ARTRep<LambdaHeader, LambdaHeaderFields>
		{
		public:
			static llvm::StructType* GetLLVMType();
			static llvm::Value* GenerateReadField(NomBuilder& builder, llvm::Value* thisObj, int32_t fieldIndex);
			static llvm::Value* GenerateWriteField(NomBuilder& builder, llvm::Value* thisObj, int32_t argindex, llvm::Value* value);
			static void GenerateConstructorCode(NomBuilder& builder, llvm::ArrayRef<llvm::Value*> typeArguments, llvm::ArrayRef<llvm::Value*> arguments, llvm::Value* descriptorRef, const NomLambda* lambda);

			static llvm::Value* GenerateReadField(NomBuilder& builder, NomLambda* lambda, llvm::Value* thisObj, int32_t fieldindex);

			static llvm::Value* GenerateReadTypeArgument(NomBuilder& builder, llvm::Value* thisObj, int32_t argindex, const NomLambda* lambda);
			static llvm::Value* GenerateWriteTypeArgument(NomBuilder& builder, llvm::Value* thisObj, int32_t argindex, llvm::Value* targ, const NomLambda* lambda);
			//static void GenerateWriteTypeArgument(NomBuilder& builder, llvm::Value* thisObj, llvm::Value* argindex, llvm::Value *type, llvm::Value* substitutions);
			static llvm::Value* GeneratePointerToTypeArguments(NomBuilder& builder, llvm::Value* thisObj, const NomLambda* lambda);
			static llvm::Value* GenerateReadCastTypeArgument(NomBuilder& builder, llvm::Value* thisObj, int32_t argindex);
			static llvm::Value* GeneratePointerToCastTypeArguments(NomBuilder& builder, llvm::Value* thisObj);
			static llvm::Value* GenerateWriteCastTypeArgument(NomBuilder& builder, llvm::Value* thisObj, llvm::Value* argindex, llvm::Value* targ);
			static llvm::Value* GenerateWriteCastTypeArgumentCMPXCHG(NomBuilder& builder, llvm::Value* thisObj, llvm::Value* argindex, llvm::Value* targ);
			static llvm::Value* GenerateReadLambdaMetadata(NomBuilder& builder, llvm::Value* thisObj);
		};
	}
}