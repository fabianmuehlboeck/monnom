#include "RTTypeHead.h"
#include "RTTypes.h"
#include "NomJIT.h"
#include "Context.h"
#include "NomType.h"
#include "CompileHelpers.h"
#include "RTOutput.h"

using namespace llvm;
namespace Nom
{
	namespace Runtime
	{

		llvm::StructType* RTTypeHead::GetLLVMType()
		{
			static llvm::StructType* llvmtype = llvm::StructType::create(LLVMCONTEXT, "LLVMRTTypeHead");
			static bool dontrepeat = true;
			if (dontrepeat)
			{
				dontrepeat = false;
				llvmtype->setBody(
					numtype(TypeKind), //Kind
					numtype(size_t),   //Hash
					POINTERTYPE  //NomType
				);
			}
			return llvmtype;
		}

		llvm::Constant* RTTypeHead::GetConstant(TypeKind kind, llvm::Constant* hash, const NomType* type)
		{
			return llvm::ConstantStruct::get(GetLLVMType(), MakeInt((unsigned char)kind), hash, GetLLVMPointer(type));
		}

		void RTTypeHead::CreateInitialization(NomBuilder& builder, llvm::Module& mod, llvm::Value* ptr, TypeKind kind, llvm::Value* hash, llvm::Value* nomtypeptr)
		{
			ptr = builder->CreatePointerCast(ptr, GetLLVMType()->getPointerTo());
			MakeStore(builder, mod, MakeInt((unsigned char)kind), builder->CreateGEP(ptr, { MakeInt32(0), MakeInt32((unsigned char)RTTypeHeadFields::Kind) }));
			MakeStore(builder, mod, hash, builder->CreateGEP(ptr, { MakeInt32(0), MakeInt32((unsigned char)RTTypeHeadFields::Hash) }));
			MakeStore(builder, mod, nomtypeptr, builder->CreateGEP(ptr, { MakeInt32(0), MakeInt32((unsigned char)RTTypeHeadFields::NomType) }));
		}

		llvm::Constant* RTTypeHead::GetVariable(const int index)
		{
			return llvm::ConstantStruct::get(GetLLVMType(), MakeInt((unsigned char)TypeKind::TKVariable), MakeInt((intptr_t)index));
		}


		int32_t RTTypeHead::KindOffset()
		{
			static const int32_t offset = GetLLVMLayout()->getElementOffset((unsigned char)RTTypeHeadFields::Kind); return offset;
		}

		int32_t RTTypeHead::HashOffset()
		{
			static const int32_t offset = GetLLVMLayout()->getElementOffset((unsigned char)RTTypeHeadFields::Hash); return offset;
		}

		int32_t RTTypeHead::NomTypeOffset()
		{
			static const int32_t offset = GetLLVMLayout()->getElementOffset((unsigned char)RTTypeHeadFields::NomType); return offset;
		}

		llvm::Value* RTTypeHead::GenerateReadTypeKind(NomBuilder& builder, llvm::Value* type)
		{
			return MakeLoad(builder, type, GetLLVMType()->getPointerTo(), MakeInt32(RTTypeHeadFields::Kind), "typeKind");
		}

		llvm::Value* RTTypeHead::GenerateReadTypeHash(NomBuilder& builder, llvm::Value* type)
		{
			return MakeLoad(builder, type, GetLLVMType()->getPointerTo(), MakeInt32(RTTypeHeadFields::Hash), "typeHash");
		}

		int RTTypeHead::GenerateTypeKindSwitch(NomBuilder& builder, llvm::Value* type, llvm::BasicBlock** classTypeBlock, llvm::BasicBlock** topTypeBlock, llvm::BasicBlock** typeVarBlock, llvm::BasicBlock** bottomTypeBlock, llvm::BasicBlock** instanceTypeBlock, llvm::BasicBlock** structTypeBlock, llvm::BasicBlock** partialAppTypeBlock, llvm::BasicBlock** dynamicTypeBlock, llvm::BasicBlock** maybeTypeBlock, llvm::BasicBlock* failBlock)
		{
			BasicBlock* origBlock = builder->GetInsertBlock();
			Function* fun = origBlock->getParent();

			BasicBlock* _classTypeBlock = nullptr, * _topTypeBlock = nullptr, * _typeVarBlock = nullptr, * _bottomTypeBlock = nullptr, * _instanceTypeBlock = nullptr, * _structTypeBlock = nullptr, * _partialAppTypeBlock = nullptr, * _dynamicTypeBlock = nullptr, * _maybeTypeBlock = nullptr;

			if (failBlock == nullptr)
			{
				failBlock = RTOutput_Fail::GenerateFailOutputBlock(builder, "Unhandled type kind!");
			}
			int cases = 0;

			auto typeKind = GenerateReadTypeKind(builder, type);
			auto typeSwitch = builder->CreateSwitch(typeKind, failBlock, 9);
			if (classTypeBlock != nullptr)
			{
				if (*classTypeBlock == nullptr)
				{
					_classTypeBlock = BasicBlock::Create(LLVMCONTEXT, "classType", fun);
					*classTypeBlock = _classTypeBlock;
					cases++;
				}
				else
				{
					_classTypeBlock = *classTypeBlock;
				}
				typeSwitch->addCase(MakeInt<TypeKind>(TypeKind::TKClass), _classTypeBlock);
			}
			if (topTypeBlock != nullptr)
			{
				if (*topTypeBlock == nullptr)
				{
					_topTypeBlock = BasicBlock::Create(LLVMCONTEXT, "topType", fun);
					*topTypeBlock = _topTypeBlock;
					cases++;
				}
				else
				{
					_topTypeBlock = *topTypeBlock;
				}
				typeSwitch->addCase(MakeInt<TypeKind>(TypeKind::TKTop), _topTypeBlock);
			}
			if (typeVarBlock != nullptr)
			{
				if (*typeVarBlock == nullptr)
				{
					_typeVarBlock = BasicBlock::Create(LLVMCONTEXT, "typeVar", fun);
					*typeVarBlock = _typeVarBlock;
					cases++;
				}
				else
				{
					_typeVarBlock = *typeVarBlock;
				}
				typeSwitch->addCase(MakeInt<TypeKind>(TypeKind::TKVariable), _typeVarBlock);
			}
			if (bottomTypeBlock != nullptr)
			{
				if (*bottomTypeBlock == nullptr)
				{
					_bottomTypeBlock = BasicBlock::Create(LLVMCONTEXT, "bottomType", fun);
					*bottomTypeBlock = _bottomTypeBlock;
					cases++;
				}
				else
				{
					_bottomTypeBlock = *bottomTypeBlock;
				}
				typeSwitch->addCase(MakeInt<TypeKind>(TypeKind::TKBottom), _bottomTypeBlock);
			}
			if (instanceTypeBlock != nullptr)
			{
				if (*instanceTypeBlock == nullptr)
				{
					_instanceTypeBlock = BasicBlock::Create(LLVMCONTEXT, "instanceType", fun);
					*instanceTypeBlock = _instanceTypeBlock;
					cases++;
				}
				else
				{
					_instanceTypeBlock = *instanceTypeBlock;
				}
				typeSwitch->addCase(MakeInt<TypeKind>(TypeKind::TKInstance), _instanceTypeBlock);
			}
			if (structTypeBlock != nullptr)
			{
				if (*structTypeBlock == nullptr)
				{
					_structTypeBlock = BasicBlock::Create(LLVMCONTEXT, "structType", fun);
					*structTypeBlock = _structTypeBlock;
					cases++;
				}
				else
				{
					_structTypeBlock = *structTypeBlock;
				}
				typeSwitch->addCase(MakeInt<TypeKind>(TypeKind::TKStruct), _structTypeBlock);
			}
			if (partialAppTypeBlock != nullptr)
			{
				if (*partialAppTypeBlock == nullptr)
				{
					_partialAppTypeBlock = BasicBlock::Create(LLVMCONTEXT, "partialAppType", fun);
					*partialAppTypeBlock = _partialAppTypeBlock;
					cases++;
				}
				else
				{
					_partialAppTypeBlock = *partialAppTypeBlock;
				}
				typeSwitch->addCase(MakeInt<TypeKind>(TypeKind::TKPartialApp), _partialAppTypeBlock);
			}
			if (dynamicTypeBlock != nullptr)
			{
				if (*dynamicTypeBlock == nullptr)
				{
					_dynamicTypeBlock = BasicBlock::Create(LLVMCONTEXT, "dynamicType", fun);
					*dynamicTypeBlock = _dynamicTypeBlock;
					cases++;
				}
				else
				{
					_dynamicTypeBlock = *dynamicTypeBlock;
				}
				typeSwitch->addCase(MakeInt<TypeKind>(TypeKind::TKDynamic), _dynamicTypeBlock);
			}
			if (maybeTypeBlock != nullptr)
			{
				if (*maybeTypeBlock == nullptr)
				{
					_maybeTypeBlock = BasicBlock::Create(LLVMCONTEXT, "maybeType", fun);
					*maybeTypeBlock = _maybeTypeBlock;
					cases++;
				}
				else
				{
					_maybeTypeBlock = *maybeTypeBlock;
				}
				typeSwitch->addCase(MakeInt<TypeKind>(TypeKind::TKMaybe), _maybeTypeBlock);
			}
			return cases;
		}

		RTTypeHead::RTTypeHead(const char* entry) : ARTRep<RTTypeHead, RTTypeHeadFields>(entry)
		{

		}
		RTTypeHead::RTTypeHead(const void* entry) : ARTRep<RTTypeHead, RTTypeHeadFields>(entry) 
		{

		}
	}
}