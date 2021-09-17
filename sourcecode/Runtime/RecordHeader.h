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
			StructValueHeader = 0, InstanceDictionary = 1, WrittenTags = 2, Fields = 3
		};
		class RecordHeader : public ARTRep<RecordHeader, StructHeaderFields>
		{
		public:
			static llvm::StructType* GetLLVMType();

			static llvm::StructType* GetLLVMType(size_t fieldCount);

			static llvm::Value* GenerateReadStructDictionary(NomBuilder& builder, llvm::Value* objPointer);
			static llvm::Value* GenerateWriteField(NomBuilder& builder, llvm::Value* thisObj, int32_t fieldindex, llvm::Value* value, size_t fieldCount);
			static llvm::Value* GenerateWriteWrittenTag(NomBuilder& builder, llvm::Value* thisObj, int32_t fieldindex, size_t fieldCount);
			static llvm::Value* GenerateReadField(NomBuilder& builder, llvm::Value* thisObj, int32_t fieldindex, size_t fieldCount);
			static llvm::Value* GenerateReadField(NomBuilder& builder, llvm::Value* thisObj, llvm::Value* fieldindex);
			static llvm::Value* GenerateReadWrittenTag(NomBuilder& builder, llvm::Value* thisObj, llvm::Value* fieldIndex);
			static llvm::Value* GenerateReadTypeArgument(NomBuilder& builder, llvm::Value* thisObj, int32_t argindex, bool hasRawInvoke);
			static llvm::Value* GeneratePointerToTypeArguments(NomBuilder& builder, llvm::Value* thisObj, bool hasRawInvoke);
			static void GenerateConstructorCode(NomBuilder& builder, llvm::ArrayRef<llvm::Value*> typeArguments, StructInstantiationCompileEnv* env, llvm::Constant* descriptorRef, const std::vector<NomInstruction*>* instructions, llvm::Constant* rawInvokePtr=nullptr);
		};
	}
}