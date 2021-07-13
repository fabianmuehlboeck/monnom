#pragma once
#include "ARTRep.h"
#include "llvm/IR/Function.h"
#include "NomBuilder.h"

namespace Nom
{
	namespace Runtime
	{
		class NomLambda;
		enum class RTLambdaFields :unsigned char { NomLink = 0, ArgCounts = 1, Signature=2, Function = 3, Dispatcher = 4, PreallocatedSlots = 5, SpecializedVTableCastID = 6, SpecializedVTable = 7 };
		class RTLambda : public ARTRep<RTLambda, RTLambdaFields>
		{
		public:
			static llvm::StructType* GetLLVMType();
			static llvm::Constant* CreateConstant(const NomLambda* lambda, int typeArgCount, int argCount, llvm::Constant* signature, llvm::Function* fun, llvm::Function* dispatcher);
			static llvm::Value* GenerateReadIRPointer(NomBuilder& builder, llvm::Value* descriptor);
			static llvm::Value* GenerateReadPreallocatedSlots(NomBuilder &builder, llvm::Value* descriptor);
			static llvm::Instruction* GenerateWritePreallocatedSlots(NomBuilder& builder, llvm::Value* descriptor, llvm::Value* newValue);
			static llvm::Value* GenerateReadSignature(NomBuilder& builder, llvm::Value* descriptor);
			static llvm::Value* GenerateReadDispatcherPointer(NomBuilder &builder, llvm::Value* descriptor);
			static llvm::Value* GenerateReadFunctionPointer(NomBuilder& builder, llvm::Value* descriptor);
			static llvm::Value* GenerateCheckArgCountsMatch(NomBuilder &builder, llvm::Value* descriptor, llvm::Value* typeArgCount, llvm::Value* argCount);
			static llvm::Value* GenerateReadSpecializedVTableCastID(NomBuilder& builder, llvm::Value* descriptor);
			static llvm::Value* GenerateReadSpecializedVTable(NomBuilder& builder, llvm::Value* descriptor);
			static void GenerateWriteSpecializedVTable(NomBuilder& builder, llvm::Value* descriptor, llvm::Value *type, llvm::Value *vtable);
		};
	}
}