#include "RTTypeHead.h"
#include "NomJIT.h"
#include "Context.h"
#include "NomType.h"
#include "CompileHelpers.h"
#include "RTOutput.h"
#include "RTCast.h"
#include "RTSubstStack.h"
#include "RTMaybeType.h"
#include "IMT.h"

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
					POINTERTYPE,	   //NomType
					GetCastFunctionType()->getPointerTo()
				);
			}
			return llvmtype;
		}

		llvm::Constant* RTTypeHead::GetConstant(TypeKind kind, llvm::Constant* hash, const NomType* type,/* llvm::Constant* subtypingFun, llvm::Constant* typeEqFun, */llvm::Constant* castFun/*, llvm::Constant* dsjFun*/)
		{
			return llvm::ConstantStruct::get(GetLLVMType(), MakeInt((unsigned char)kind), hash, GetLLVMPointer(type), castFun);
		}

		void RTTypeHead::CreateInitialization(NomBuilder& builder, llvm::Module& mod, llvm::Value* ptr, TypeKind kind, llvm::Value* hash, llvm::Value* nomtypeptr, /*llvm::Value* subtypingFun, llvm::Value* typeEqFun,*/ llvm::Value* castFun/*, llvm::Value* dsjFun*/)
		{
			ptr = builder->CreatePointerCast(ptr, GetLLVMType()->getPointerTo());
			MakeInvariantStore(builder, mod, MakeInt((unsigned char)kind), builder->CreateGEP(ptr, { MakeInt32(0), MakeInt32((unsigned char)RTTypeHeadFields::Kind) }));
			MakeInvariantStore(builder, mod, hash, builder->CreateGEP(ptr, { MakeInt32(0), MakeInt32((unsigned char)RTTypeHeadFields::Hash) }));
			MakeInvariantStore(builder, mod, nomtypeptr, builder->CreateGEP(ptr, { MakeInt32(0), MakeInt32((unsigned char)RTTypeHeadFields::NomType) }));
			//MakeInvariantStore(builder, mod, subtypingFun, builder->CreateGEP(ptr, { MakeInt32(0), MakeInt32((unsigned char)RTTypeHeadFields::SubtypingFun) }));
			//MakeInvariantStore(builder, mod, typeEqFun, builder->CreateGEP(ptr, { MakeInt32(0), MakeInt32((unsigned char)RTTypeHeadFields::TypeEqFun) }));
			MakeInvariantStore(builder, mod, castFun, builder->CreateGEP(ptr, { MakeInt32(0), MakeInt32((unsigned char)RTTypeHeadFields::CastFun) }));
			//MakeInvariantStore(builder, mod, dsjFun, builder->CreateGEP(ptr, { MakeInt32(0), MakeInt32((unsigned char)RTTypeHeadFields::IsDisjointFun) }));
		}

		llvm::Constant* RTTypeHead::GetVariable(const int index)
		{
			return llvm::ConstantStruct::get(GetLLVMType(), MakeInt((unsigned char)TypeKind::TKVariable), MakeInt((intptr_t)index));
		}


		llvm::Value* RTTypeHead::GenerateReadTypeKind(NomBuilder& builder, llvm::Value* type)
		{
			return MakeInvariantLoad(builder, type, GetLLVMType()->getPointerTo(), MakeInt32(RTTypeHeadFields::Kind), "typeKind");
		}

		llvm::Value* RTTypeHead::GenerateReadTypeHash(NomBuilder& builder, llvm::Value* type)
		{
			return MakeInvariantLoad(builder, type, GetLLVMType()->getPointerTo(), MakeInt32(RTTypeHeadFields::Hash), "typeHash");
		}

		llvm::Value* RTTypeHead::GenerateReadSubtypingFun(NomBuilder& builder, llvm::Value* type)
		{
			throw new std::exception();
			//return MakeInvariantLoad(builder, type, GetLLVMType()->getPointerTo(), MakeInt32(RTTypeHeadFields::SubtypingFun), "subtypingFun");
		}

		llvm::Value* RTTypeHead::GenerateReadTypeEqFun(NomBuilder& builder, llvm::Value* type)
		{
			throw new std::exception();
			//return MakeInvariantLoad(builder, type, GetLLVMType()->getPointerTo(), MakeInt32(RTTypeHeadFields::TypeEqFun), "typeEqFun");
		}

		llvm::Value* RTTypeHead::GenerateReadCastFun(NomBuilder& builder, llvm::Value* type)
		{
			return MakeInvariantLoad(builder, type, GetLLVMType()->getPointerTo(), MakeInt32(RTTypeHeadFields::CastFun), "castfun");
		}

		llvm::Value* RTTypeHead::GenerateReadIsDisjointFun(NomBuilder& builder, llvm::Value* type)
		{
			throw new std::exception();
			//return MakeInvariantLoad(builder, type, GetLLVMType()->getPointerTo(), MakeInt32(RTTypeHeadFields::IsDisjointFun), "isDisjointFun");
		}

		int RTTypeHead::GenerateTypeKindSwitchRecurse(NomBuilder& builder, llvm::Value* type, llvm::Value* substStack, llvm::Value** innerTypeVar, llvm::Value** innerSubstStackVar, llvm::BasicBlock** classTypeBlockVar, llvm::BasicBlock** topTypeBlockVar, llvm::BasicBlock** typeVarBlockVar, llvm::BasicBlock** bottomTypeBlockVar, llvm::BasicBlock** instanceTypeBlockVar, llvm::BasicBlock** dynamicTypeBlockVar, llvm::BasicBlock** maybeTypeBlockVar, llvm::BasicBlock* failBlock)
		{
			BasicBlock* origBlock = builder->GetInsertBlock();
			Function* fun = origBlock->getParent();
			BasicBlock* loopHeadBlock = BasicBlock::Create(LLVMCONTEXT, "kindSwitchLoopHead", fun);
			if (failBlock == nullptr)
			{
				failBlock = RTOutput_Fail::GenerateFailOutputBlock(builder, "Unhandled type kind!");
			}
			builder->CreateBr(loopHeadBlock);

			builder->SetInsertPoint(loopHeadBlock);
			PHINode* typePHI = builder->CreatePHI(TYPETYPE, 2 + maybeTypeBlockVar == nullptr ? 1 : 0);
			PHINode* substPHI = builder->CreatePHI(substStack->getType(), 2 + maybeTypeBlockVar == nullptr ? 1 : 0);
			typePHI->addIncoming(type, origBlock);
			substPHI->addIncoming(substStack, origBlock);

			auto typeKind = GenerateReadTypeKind(builder, typePHI);
			auto typeSwitch = builder->CreateSwitch(typeKind, failBlock, 9);

			if (innerTypeVar != nullptr)
			{
				*innerTypeVar = typePHI;
			}
			if (innerSubstStackVar != nullptr)
			{
				*innerSubstStackVar = substPHI;
			}

			int cases = 0;
			BasicBlock* classTypeBlock = nullptr, * topTypeBlock = nullptr, * typeVarBlock = nullptr, * bottomTypeBlock = nullptr, * instanceTypeBlock = nullptr, * partialAppTypeBlock = nullptr, * dynamicTypeBlock = nullptr, * maybeTypeBlock = nullptr;
			BasicBlock* typeVarUnfoldBlock = BasicBlock::Create(LLVMCONTEXT, "typeVarUnfold", fun);
			BasicBlock* typeVarSubstBlock = BasicBlock::Create(LLVMCONTEXT, "typeVarSubstitute", fun);

			if (classTypeBlockVar != nullptr)
			{
				if (*classTypeBlockVar == nullptr)
				{
					classTypeBlock = BasicBlock::Create(LLVMCONTEXT, "classType", fun);
					*classTypeBlockVar = classTypeBlock;
					cases++;
				}
				else
				{
					classTypeBlock = *classTypeBlockVar;
				}
				typeSwitch->addCase(MakeInt<TypeKind>(TypeKind::TKClass), classTypeBlock);
			}
			if (topTypeBlockVar != nullptr)
			{
				if (*topTypeBlockVar == nullptr)
				{
					topTypeBlock = BasicBlock::Create(LLVMCONTEXT, "topType", fun);
					*topTypeBlockVar = topTypeBlock;
					cases++;
				}
				else
				{
					topTypeBlock = *topTypeBlockVar;
				}
				typeSwitch->addCase(MakeInt<TypeKind>(TypeKind::TKTop), topTypeBlock);
			}

			typeSwitch->addCase(MakeInt<TypeKind>(TypeKind::TKVariable), typeVarUnfoldBlock);
			builder->SetInsertPoint(typeVarUnfoldBlock);

			if (typeVarBlockVar != nullptr)
			{
				if (*typeVarBlockVar == nullptr)
				{
					typeVarBlock = BasicBlock::Create(LLVMCONTEXT, "typeVar", fun);
					*typeVarBlockVar = typeVarBlock;
					cases++;
				}
				else
				{
					typeVarBlock = *typeVarBlockVar;
				}
			}
			else
			{
				typeVarBlock = failBlock;
			}

			builder->CreateCondBr(builder->CreateIsNotNull(substPHI), typeVarSubstBlock, typeVarBlock);

			builder->SetInsertPoint(typeVarSubstBlock);
			Value* newSubst = nullptr;
			auto newType = RTSubstStack::Pop(builder, substPHI, typePHI, &newSubst);
			typePHI->addIncoming(newType, builder->GetInsertBlock());
			substPHI->addIncoming(newSubst, builder->GetInsertBlock());
			builder->CreateBr(loopHeadBlock);

			if (bottomTypeBlockVar != nullptr)
			{
				if (*bottomTypeBlockVar == nullptr)
				{
					bottomTypeBlock = BasicBlock::Create(LLVMCONTEXT, "bottomType", fun);
					*bottomTypeBlockVar = bottomTypeBlock;
					cases++;
				}
				else
				{
					bottomTypeBlock = *bottomTypeBlockVar;
				}
				typeSwitch->addCase(MakeInt<TypeKind>(TypeKind::TKBottom), bottomTypeBlock);
			}
			if (instanceTypeBlockVar != nullptr)
			{
				if (*instanceTypeBlockVar == nullptr)
				{
					instanceTypeBlock = BasicBlock::Create(LLVMCONTEXT, "instanceType", fun);
					*instanceTypeBlockVar = instanceTypeBlock;
					cases++;
				}
				else
				{
					instanceTypeBlock = *instanceTypeBlockVar;
				}
				typeSwitch->addCase(MakeInt<TypeKind>(TypeKind::TKInstance), instanceTypeBlock);
			}
			if (dynamicTypeBlockVar != nullptr)
			{
				if (*dynamicTypeBlockVar == nullptr)
				{
					dynamicTypeBlock = BasicBlock::Create(LLVMCONTEXT, "dynamicType", fun);
					*dynamicTypeBlockVar = dynamicTypeBlock;
					cases++;
				}
				else
				{
					dynamicTypeBlock = *dynamicTypeBlockVar;
				}
				typeSwitch->addCase(MakeInt<TypeKind>(TypeKind::TKDynamic), dynamicTypeBlock);
			}
			if (maybeTypeBlockVar != nullptr)
			{
				if (*maybeTypeBlockVar == nullptr)
				{
					maybeTypeBlock = BasicBlock::Create(LLVMCONTEXT, "maybeType", fun);
					*maybeTypeBlockVar = maybeTypeBlock;
					cases++;
				}
				else
				{
					maybeTypeBlock = *maybeTypeBlockVar;
				}
				typeSwitch->addCase(MakeInt<TypeKind>(TypeKind::TKMaybe), maybeTypeBlock);
			}
			else
			{
				maybeTypeBlock = BasicBlock::Create(LLVMCONTEXT, "maybeType", fun);
				builder->SetInsertPoint(maybeTypeBlock);
				auto nextType = RTMaybeType::GenerateReadPotentialType(builder, typePHI);
				typePHI->addIncoming(nextType, builder->GetInsertBlock());
				substPHI->addIncoming(substPHI, builder->GetInsertBlock());
				typeSwitch->addCase(MakeInt<TypeKind>(TypeKind::TKMaybe), maybeTypeBlock);
				builder->CreateBr(loopHeadBlock);
			}
			return cases;
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
				typeSwitch->addCase(MakeInt<TypeKind>(TypeKind::TKRecord), _structTypeBlock);
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
		llvm::FunctionType* GetSubtypingFunctionType()
		{
			return GetIMTFunctionType();
		}
		llvm::FunctionType* GetTypeEqFunctionType()
		{
			return GetIMTFunctionType();
		}
		llvm::FunctionType* GetCastFunctionType()
		{
			return GetIMTFunctionType();
		}
		llvm::FunctionType* GetIsDisjointFunctionType()
		{
			return GetIMTFunctionType();
		}
	}
}