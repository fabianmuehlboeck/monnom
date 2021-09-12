#pragma once

#include <vector>
#include "llvm/ADT/TinyPtrVector.h"
#include "llvm/IR/DerivedTypes.h"
#include "Defs.h"
#include <list>
#include <tuple>
#include "llvm/IR/DerivedTypes.h"
#include "ARTRep.h"
#include "NomBuilder.h"

namespace Nom
{
	namespace Runtime
	{
		class RTConcreteType;
		class RTUnionType;
		class RTIsectType;
		class RTClassType;
		class RTTypeVar;
		class NomType;
		enum class RTTypeHeadFields : unsigned char { Kind = 0, Hash = 1, NomType = 2, /*SubtypingFun = 3, TypeEqFun = 4,*/ CastFun = 3/*, IsDisjointFun = 6*/ };

		llvm::FunctionType* GetSubtypingFunctionType();
		llvm::FunctionType* GetTypeEqFunctionType();
		llvm::FunctionType* GetCastFunctionType();
		llvm::FunctionType* GetIsDisjointFunctionType();

		class RTTypeHead : public ARTRep<RTTypeHead, RTTypeHeadFields>
		{
		public:
			static ::llvm::StructType* GetLLVMType();
			static llvm::Constant* GetConstant(TypeKind kind, llvm::Constant* hash, const NomType* type, /*llvm::Constant* subtypingFun, llvm::Constant* typeEqFun,*/ llvm::Constant* castFun/*, llvm::Constant* dsjFun*/);
			static void CreateInitialization(NomBuilder& builder, llvm::Module& mod, llvm::Value* ptr, TypeKind kind, llvm::Value* hash, llvm::Value* nomtypeptr,/* llvm::Value* subtypingFun, llvm::Value* typeEqFun,*/ llvm::Value* castFun/*, llvm::Value* dsjFun*/);
			static llvm::Constant* GetVariable(const int index);

			static llvm::Value* GenerateReadTypeKind(NomBuilder& builder, llvm::Value* type);
			static llvm::Value* GenerateReadTypeHash(NomBuilder& builder, llvm::Value* type);
			static llvm::Value* GenerateReadSubtypingFun(NomBuilder& builder, llvm::Value* type);
			static llvm::Value* GenerateReadTypeEqFun(NomBuilder& builder, llvm::Value* type);
			static llvm::Value* GenerateReadCastFun(NomBuilder& builder, llvm::Value* type);
			static llvm::Value* GenerateReadIsDisjointFun(NomBuilder& builder, llvm::Value* type);

			static int GenerateTypeKindSwitchRecurse(NomBuilder& builder, llvm::Value* type, llvm::Value* substStack, llvm::Value** innerTypeVar, llvm::Value** innerSubstStackVar, llvm::BasicBlock** classTypeBlock, llvm::BasicBlock** topTypeBlock, llvm::BasicBlock** typeVarBlock, llvm::BasicBlock** bottomTypeBlock, llvm::BasicBlock** instanceTypeBlock, llvm::BasicBlock** dynamicTypeBlock, llvm::BasicBlock** maybeTypeBlock, llvm::BasicBlock* failBlock);

			static int GenerateTypeKindSwitch(NomBuilder& builder, llvm::Value* type, llvm::BasicBlock** classTypeBlock, llvm::BasicBlock** topTypeBlock, llvm::BasicBlock** typeVarBlock, llvm::BasicBlock** bottomTypeBlock, llvm::BasicBlock** instanceTypeBlock, llvm::BasicBlock** structTypeBlock, llvm::BasicBlock** partialAppTypeBlock, llvm::BasicBlock** dynamicTypeBlock, llvm::BasicBlock** maybeTypeBlock, llvm::BasicBlock* failBlock = nullptr);

			RTTypeHead(const char* entry = nullptr);
			RTTypeHead(const void* entry);

			~RTTypeHead()
			{

			}
		};


	}
}