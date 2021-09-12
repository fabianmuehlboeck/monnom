#pragma once
#include "ARTRep.h"
#include "llvm/IR/IRBuilder.h"
#include "CompileEnv.h"
#include "NomBuilder.h"

namespace Nom
{
	namespace Runtime
	{
		class NomRecord;
		class NomInstruction;
		enum class StructHeaderFields : unsigned char {
			StructValueHeader = 0, InstanceDictionary = 1, Fields = 2
		};
		class RecordHeader : public ARTRep<RecordHeader, StructHeaderFields>
		{
		public:
			static llvm::StructType* GetLLVMType();

			static llvm::Value* GenerateReadStructDictionary(NomBuilder& builder, llvm::Value* objPointer);
			static llvm::Value* GenerateReadAndLockField(NomBuilder& builder, llvm::Value* thisObj, llvm::Value* fieldindex, bool hasRawInvoke);
			static llvm::Value* GenerateWriteField(NomBuilder& builder, llvm::Value* thisObj, int32_t fieldindex, llvm::Value* value, bool hasRawInvoke);
			static llvm::Value* GenerateWriteField(NomBuilder& builder, llvm::Value* thisObj, llvm::Value* fieldindex, llvm::Value* value, bool hasRawInvoke);
			static llvm::Value* GenerateReadField(NomBuilder& builder, llvm::Value* thisObj, int32_t fieldindex, bool hasRawInvoke);
			static llvm::Value* GetFieldPointer(NomBuilder& builder, llvm::Value* thisObj, llvm::Value* fieldindex, bool hasRawInvoke);
			static llvm::Value* GenerateReadField(NomBuilder& builder, llvm::Value* thisObj, llvm::Value* fieldindex, bool hasRawInvoke);
			static llvm::Value* GenerateReadTypeArgument(NomBuilder& builder, llvm::Value* thisObj, int32_t argindex, bool hasRawInvoke);
			static llvm::Value* GeneratePointerToTypeArguments(NomBuilder& builder, llvm::Value* thisObj, bool hasRawInvoke);
			static void GenerateConstructorCode(NomBuilder& builder, llvm::ArrayRef<llvm::Value*> typeArguments, StructInstantiationCompileEnv* env, llvm::Constant* descriptorRef, const std::vector<NomInstruction*>* instructions, llvm::Constant* rawInvokePtr=nullptr);
		};
	}
}