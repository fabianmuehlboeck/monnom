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
		enum class RTTypeHeadFields : unsigned char { Kind = 0, Hash = 1, NomType = 2 };

		class RTTypeHead : public ARTRep<RTTypeHead, RTTypeHeadFields>
		{
		public:
			static ::llvm::StructType* GetLLVMType();
			static llvm::Constant* GetConstant(TypeKind kind, llvm::Constant* hash, const NomType* type);
			static void CreateInitialization(NomBuilder& builder, llvm::Module& mod, llvm::Value* ptr, TypeKind kind, llvm::Value* hash, llvm::Value* nomtypeptr);
			static llvm::Constant* GetVariable(const int index);
			static int32_t KindOffset();
			static int32_t HashOffset();
			static int32_t NomTypeOffset();
			TypeKind& Kind() const { return *((TypeKind*)(Entry(KindOffset()))); }
			intptr_t& Hash() const { return *((intptr_t*)(Entry(HashOffset()))); }
			NomType*& GetNomType() const { return *((NomType**)(Entry(NomTypeOffset()))); }

			static llvm::Value* GenerateReadTypeKind(NomBuilder& builder, llvm::Value* type);
			static llvm::Value* GenerateReadTypeHash(NomBuilder& builder, llvm::Value* type);

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