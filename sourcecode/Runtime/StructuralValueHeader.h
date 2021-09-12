#pragma once
#include "llvm/IR/Type.h"
#include "ARTRep.h"
#include "NomBuilder.h"

namespace Nom
{
	namespace Runtime
	{
		class NomRecord;
		class NomInstruction;
		class CompileEnv;
		enum class StructuralValueHeaderFields : unsigned char {
			TypeArgs = 0, RefValueHeader = 1, RawInvokeSpace = 2, CastData = 3
		};
		class StructuralValueHeader : public ARTRep<StructuralValueHeader, StructuralValueHeaderFields>
		{
		public:
			static llvm::StructType* GetLLVMType();
			static void GenerateInitializationCode(NomBuilder& builder, llvm::Value* refValue, llvm::ArrayRef<llvm::Value*> typeArguments, llvm::Constant* vTablePtr, llvm::Constant* rawInvokePointer=nullptr);
			static llvm::Value* GenerateReadTypeArgsPtr(NomBuilder& builder, llvm::Value* refValue);
			static llvm::Value* GenerateReadCastData(NomBuilder& builder, llvm::Value* refValue);
			static llvm::Value* GenerateReadTypeArgument(NomBuilder& builder, llvm::Value* refValue, llvm::Value* index);
			static llvm::Value* GenerateWriteCastTypePointerCMPXCHG(NomBuilder& builder, llvm::Value* refValue, llvm::Value* newPtr, llvm::Value* oldPtr);
			static void GenerateMonotonicStructuralCast(NomBuilder& builder, llvm::Function* fun, llvm::BasicBlock* successBlock, llvm::BasicBlock* failBlock, llvm::Value* value, NomClassTypeRef rightType, llvm::Value* outerStack);
			static void GenerateMonotonicStructuralCast(NomBuilder& builder, llvm::Function* fun, llvm::BasicBlock* successBlock, llvm::BasicBlock* failBlock, llvm::Value* value, llvm::Value* rightType, llvm::Value* rightIface, llvm::Value* rightTypeArgs, llvm::Value* outerStack);
		};
	}
}