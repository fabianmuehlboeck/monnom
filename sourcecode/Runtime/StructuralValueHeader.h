#pragma once
#include "llvm/IR/Type.h"
#include "ARTRep.h"
#include "NomBuilder.h"

namespace Nom
{
	namespace Runtime
	{
		class NomStruct;
		class NomInstruction;
		class CompileEnv;
		enum class StructuralValueHeaderFields : unsigned char {
			TypeArgs = 0, RefValueHeader = 1, RawInvokeSpace = 2, STable = 3, CastData = 4
		};
		class StructuralValueHeader : public ARTRep<StructuralValueHeader, StructuralValueHeaderFields>
		{
		public:
			static llvm::StructType* GetLLVMType();
			static void GenerateInitializationCode(NomBuilder& builder, llvm::Value* descriptor, llvm::ArrayRef<llvm::Value*> typeArguments, llvm::Value* vTablePtr, llvm::Value* sTablePtr);
			static llvm::Value* GenerateReadTypeArgsPtr(NomBuilder& builder, llvm::Value* descriptor);
			static llvm::Value* GenerateReadCastData(NomBuilder& builder, llvm::Value* descriptor);
			static llvm::Value* GenerateReadSTablePtr(NomBuilder& builder, llvm::Value* descriptor);
			static llvm::Value* GenerateReadRawSTablePtr(NomBuilder& builder, llvm::Value* sValue);
			static llvm::Value* GenerateReadTypeArgument(NomBuilder& builder, llvm::Value* descriptor, llvm::Value* index);
			static llvm::Value* GenerateWriteCastTypePointerCMPXCHG(NomBuilder& builder, llvm::Value* thisObj, llvm::Value* newPtr, llvm::Value* oldPtr);
			static int GenerateStructuralValueKindCase(NomBuilder& builder, llvm::Value* sValue, llvm::Value** sTableVar, llvm::BasicBlock** lambdaBlock, llvm::BasicBlock** recordBlock, llvm::BasicBlock** partialAppBlock, llvm::BasicBlock** errorBlockVar);
			static void GenerateMonotonicStructuralCast(NomBuilder& builder, CompileEnv* env, llvm::Function* fun, llvm::BasicBlock* successBlock, llvm::BasicBlock* failBlock, llvm::Value* value, NomClassTypeRef rightType, llvm::Value* outerStack);
			static void GenerateMonotonicStructuralCast(NomBuilder& builder, CompileEnv* env, llvm::Function* fun, llvm::BasicBlock* successBlock, llvm::BasicBlock* failBlock, llvm::Value* value, llvm::Value* rightType, llvm::Value* rightIface, llvm::Value* rightTypeArgs, llvm::Value* outerStack);
		};
	}
}