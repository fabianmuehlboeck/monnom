#pragma once

#include <vector>
PUSHDIAGSUPPRESSION
#include "llvm/ADT/TinyPtrVector.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/DerivedTypes.h"
POPDIAGSUPPRESSION
#include "Defs.h"
#include <list>
#include <tuple>
#include "ARTRep.h"
#include "NomBuilder.h"

namespace Nom
{
	namespace Runtime
	{
		class RTConcreteType;
		class RTUnionType;
		class RTIsectType;
		class XRTClassType;
		class XRTTypeVar;
		class NomType;
		enum class RTTypeHeadFields : unsigned char { Kind = 0, Hash = 1, NomType = 2, CastFun = 3 };

		llvm::FunctionType* GetCastFunctionType();
		llvm::FunctionType* GetTypeEqFunctionType();
		llvm::FunctionType* GetSubtypeFunctionType();

		class RTTypeHead : public ARTRep<RTTypeHead, RTTypeHeadFields>
		{
		public:
			static ::llvm::StructType* GetLLVMType();
			static llvm::Constant* GetConstant(TypeKind kind, llvm::Constant* hash, const NomType* type, llvm::Constant* castFun);
			static void CreateInitialization(NomBuilder& builder, llvm::Module& mod, llvm::Value* ptr, TypeKind kind, llvm::Value* hash, llvm::Value* nomtypeptr, llvm::Value* castFun);
			static llvm::Constant* GetVariable(const int index);

			static llvm::Value* GenerateReadTypeKind(NomBuilder& builder, llvm::Value* type);
			static llvm::Value* GenerateReadTypeHash(NomBuilder& builder, llvm::Value* type);
			static llvm::Value* GenerateReadCastFun(NomBuilder& builder, llvm::Value* type);

			static int GenerateTypeKindSwitchRecurse(NomBuilder& builder, llvm::Value* type, llvm::Value* substStack, llvm::Value** innerTypeVar, llvm::Value** innerSubstStackVar, llvm::BasicBlock** classTypeBlock, llvm::BasicBlock** topTypeBlock, llvm::BasicBlock** typeVarBlock, llvm::BasicBlock** bottomTypeBlock, llvm::BasicBlock** instanceTypeBlock, llvm::BasicBlock** dynamicTypeBlock, llvm::BasicBlock** maybeTypeBlock, llvm::BasicBlock* failBlock);

			static int GenerateTypeKindSwitch(NomBuilder& builder, llvm::Value* type, llvm::BasicBlock** classTypeBlock, llvm::BasicBlock** topTypeBlock, llvm::BasicBlock** typeVarBlock, llvm::BasicBlock** bottomTypeBlock, llvm::BasicBlock** instanceTypeBlock, llvm::BasicBlock** structTypeBlock, llvm::BasicBlock** partialAppTypeBlock, llvm::BasicBlock** dynamicTypeBlock, llvm::BasicBlock** maybeTypeBlock, llvm::BasicBlock* failBlock = nullptr);

			RTTypeHead(const char* entry = nullptr);
			RTTypeHead(const void* entry);

		};


	}
}
