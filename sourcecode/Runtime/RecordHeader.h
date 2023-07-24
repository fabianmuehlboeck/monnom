#pragma once
#include "ARTRep.h"
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
			static llvm::StructType* GetFieldGroupType();
			static llvm::StructType* GetLLVMType();

			static llvm::StructType* GetLLVMType(size_t fieldCount);

			static llvm::Value* GenerateReadStructDictionary(NomBuilder& builder, llvm::Value* objPointer);
			static llvm::Value* GenerateWriteField(NomBuilder& builder, llvm::Value* thisObj, PWCInt32 fieldindex, llvm::Value* value, size_t fieldCount);
			static llvm::Value* GenerateWriteWrittenTag(NomBuilder& builder, llvm::Value* thisObj, PWCInt32 fieldindex, size_t fieldCount);
			static llvm::Value* GenerateReadField(NomBuilder& builder, llvm::Value* thisObj, PWCInt32 fieldindex, size_t fieldCount);
			static llvm::Value* GenerateReadWrittenTag(NomBuilder& builder, llvm::Value* thisObj, PWCInt32 fieldIndex);
			static llvm::Value* GenerateReadTypeArgument(NomBuilder& builder, llvm::Value* thisObj, PWCInt32 argindex, bool hasRawInvoke);
			static llvm::Value* GeneratePointerToTypeArguments(NomBuilder& builder, llvm::Value* thisObj, bool hasRawInvoke);
			static void GenerateConstructorCode(NomBuilder& builder, llvm::ArrayRef<llvm::Value*> typeArguments, StructInstantiationCompileEnv* env, llvm::Constant* descriptorRef, const std::vector<NomInstruction*>* instructions, llvm::Constant* rawInvokePtr=nullptr);
		};
	}
}
