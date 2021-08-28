#pragma once
#include "ARTRep.h"
#include "llvm/IR/IRBuilder.h"
#include "CompileEnv.h"
#include "NomBuilder.h"

namespace Nom
{
	namespace Runtime
	{
		class NomStruct;
		class NomInstruction;
		enum class StructHeaderFields : unsigned char {
			StructValueHeader = 0, InstanceDictionary = 1, Fields = 2
		};
		class StructHeader : public ARTRep<StructHeader, StructHeaderFields>
		{
		public:
			static llvm::StructType* GetLLVMType();

			static llvm::Value* GenerateReadStructDescriptor(NomBuilder& builder, llvm::Value* objPointer);
			static llvm::Value* GenerateReadStructDescriptorPtr(NomBuilder& builder, llvm::Value* objPointer);
			static llvm::Value* GenerateReadStructDictionary(NomBuilder& builder, llvm::Value* objPointer);
			static llvm::Value* GenerateReadAndLockField(NomBuilder& builder, llvm::Value* thisObj, int32_t fieldindex, bool hasRawInvoke);
			static llvm::Value* GenerateWriteField(NomBuilder& builder, llvm::Value* thisObj, int32_t fieldindex, llvm::Value* value, bool hasRawInvoke);
			static llvm::Value* GenerateWriteField(NomBuilder& builder, llvm::Value* thisObj, llvm::Value* fieldindex, llvm::Value* value, bool hasRawInvoke);
			static llvm::Value* GenerateReadField(NomBuilder& builder, llvm::Value* thisObj, int32_t fieldindex, bool hasRawInvoke);
			static llvm::Value* GetFieldPointer(NomBuilder& builder, llvm::Value* thisObj, llvm::Value* fieldindex, bool hasRawInvoke);
			static llvm::Value* GenerateReadField(NomBuilder& builder, llvm::Value* thisObj, llvm::Value* fieldindex, bool hasRawInvoke);
			static llvm::Value* GenerateReadTypeArgument(NomBuilder& builder, llvm::Value* thisObj, int32_t argindex, bool hasRawInvoke);
			static llvm::Value* GenerateWriteTypeArgument(NomBuilder& builder, llvm::Value* thisObj, int32_t argindex, llvm::Value* targ, bool hasRawInvoke);
			static llvm::Value* GeneratePointerToTypeArguments(NomBuilder& builder, llvm::Value* thisObj, bool hasRawInvoke);
			//static llvm::Value* GenerateWriteCastTypeArgument(NomBuilder& builder, llvm::Value* thisObj, llvm::Value* argindex, llvm::Value* targ);
			//static llvm::Value* GenerateWriteCastTypeArgumentCMPXCHG(NomBuilder& builder, llvm::Value* thisObj, llvm::Value* argindex, llvm::Value* targ);
			//static llvm::Value* GeneratePointerToCastTypeArguments(NomBuilder& builder, llvm::Value* thisObj);
			static void GenerateConstructorCode(NomBuilder& builder, llvm::ArrayRef<llvm::Value*> typeArguments, StructInstantiationCompileEnv* env, llvm::Value* descriptorRef, const std::vector<NomInstruction*>* instructions);
		};
	}
}