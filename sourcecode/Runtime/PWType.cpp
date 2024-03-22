#include "PWType.h"
#include "PWCastFunction.h"
#include "PWNomType.h"
#include "RTTypeHead.h"
#include "CompileHelpers.h"
#include "PWSubstStack.h"
#include "NomType.h"
#include "RTTypeEq.h"
#include "PWMaybeType.h"
#include "PWClassType.h"
#include "PWInstanceType.h"
#include "PWTypeVar.h"
#include "RTOutput.h"
#include "PWTopType.h"
#include "PWBotType.h"
#include "PWDynType.h"

using namespace llvm;
using namespace std;
namespace Nom
{
	namespace Runtime
	{
		llvm::Type* PWType::GetLLVMType()
		{
			return RTTypeHead::GetLLVMType();
		}
		llvm::Type* PWType::GetWrappedLLVMType()
		{
			return NLLVMPointer(GetLLVMType());
		}
		llvm::MDNode* PWType::GetDefaultTypeCasesNode()
		{
			static MDNode * node = MDNode::get(LLVMCONTEXT, { MDString::get(LLVMCONTEXT, "branch_weights"), 
				ConstantAsMetadata::get(MakeInt32(0)),
				ConstantAsMetadata::get(MakeInt32(9)),
				ConstantAsMetadata::get(MakeInt32(9)),
				ConstantAsMetadata::get(MakeInt32(9)),
				ConstantAsMetadata::get(MakeInt32(9)),
				ConstantAsMetadata::get(MakeInt32(9)),
				ConstantAsMetadata::get(MakeInt32(9)),
				ConstantAsMetadata::get(MakeInt32(9)) });
			return node;
		}
		llvm::MDNode* PWType::GetExpectClassTypeNode()
		{
			static MDNode* node = MDNode::get(LLVMCONTEXT, { MDString::get(LLVMCONTEXT, "branch_weights"),
				ConstantAsMetadata::get(MakeInt32(0)),
				ConstantAsMetadata::get(MakeInt32(100)),
				ConstantAsMetadata::get(MakeInt32(60)),
				ConstantAsMetadata::get(MakeInt32(40)),
				ConstantAsMetadata::get(MakeInt32(9)),
				ConstantAsMetadata::get(MakeInt32(3)),
				ConstantAsMetadata::get(MakeInt32(3)),
				ConstantAsMetadata::get(MakeInt32(3)) });
			return node;
		}
		llvm::MDNode* PWType::GetExpectInstanceTypeNode()
		{
			static MDNode* node = MDNode::get(LLVMCONTEXT, { MDString::get(LLVMCONTEXT, "branch_weights"),
				ConstantAsMetadata::get(MakeInt32(0)),
				ConstantAsMetadata::get(MakeInt32(60)),
				ConstantAsMetadata::get(MakeInt32(100)),
				ConstantAsMetadata::get(MakeInt32(40)),
				ConstantAsMetadata::get(MakeInt32(9)),
				ConstantAsMetadata::get(MakeInt32(3)),
				ConstantAsMetadata::get(MakeInt32(3)),
				ConstantAsMetadata::get(MakeInt32(3)) });
			return node;
		}
		llvm::MDNode* PWType::GetExpectMaybeTypeNode()
		{
			static MDNode* node = MDNode::get(LLVMCONTEXT, { MDString::get(LLVMCONTEXT, "branch_weights"),
				ConstantAsMetadata::get(MakeInt32(0)),
				ConstantAsMetadata::get(MakeInt32(40)),
				ConstantAsMetadata::get(MakeInt32(40)),
				ConstantAsMetadata::get(MakeInt32(40)),
				ConstantAsMetadata::get(MakeInt32(100)),
				ConstantAsMetadata::get(MakeInt32(3)),
				ConstantAsMetadata::get(MakeInt32(3)),
				ConstantAsMetadata::get(MakeInt32(3)) });
		}
		llvm::MDNode* PWType::GetExpectBotTypeNode()
		{
			static MDNode* node = MDNode::get(LLVMCONTEXT, { MDString::get(LLVMCONTEXT, "branch_weights"),
				ConstantAsMetadata::get(MakeInt32(0)),
				ConstantAsMetadata::get(MakeInt32(9)),
				ConstantAsMetadata::get(MakeInt32(9)),
				ConstantAsMetadata::get(MakeInt32(9)),
				ConstantAsMetadata::get(MakeInt32(9)),
				ConstantAsMetadata::get(MakeInt32(100)),
				ConstantAsMetadata::get(MakeInt32(9)),
				ConstantAsMetadata::get(MakeInt32(60)) });
			return node;
		}
		llvm::MDNode* PWType::GetExpectTopTypeNode()
		{
			static MDNode* node = MDNode::get(LLVMCONTEXT, { MDString::get(LLVMCONTEXT, "branch_weights"),
				ConstantAsMetadata::get(MakeInt32(0)),
				ConstantAsMetadata::get(MakeInt32(9)),
				ConstantAsMetadata::get(MakeInt32(9)),
				ConstantAsMetadata::get(MakeInt32(9)),
				ConstantAsMetadata::get(MakeInt32(9)),
				ConstantAsMetadata::get(MakeInt32(9)),
				ConstantAsMetadata::get(MakeInt32(100)),
				ConstantAsMetadata::get(MakeInt32(60)) });
			return node;
		}
		llvm::MDNode* PWType::GetExpectDynTypeNode()
		{
			static MDNode* node = MDNode::get(LLVMCONTEXT, { MDString::get(LLVMCONTEXT, "branch_weights"),
				ConstantAsMetadata::get(MakeInt32(0)),
				ConstantAsMetadata::get(MakeInt32(9)),
				ConstantAsMetadata::get(MakeInt32(9)),
				ConstantAsMetadata::get(MakeInt32(9)),
				ConstantAsMetadata::get(MakeInt32(9)),
				ConstantAsMetadata::get(MakeInt32(9)),
				ConstantAsMetadata::get(MakeInt32(40)),
				ConstantAsMetadata::get(MakeInt32(100)) });
			return node;
		}
		llvm::MDNode* PWType::GetExpectTypeVarNode()
		{
			static MDNode* node = MDNode::get(LLVMCONTEXT, { MDString::get(LLVMCONTEXT, "branch_weights"),
				ConstantAsMetadata::get(MakeInt32(0)),
				ConstantAsMetadata::get(MakeInt32(30)),
				ConstantAsMetadata::get(MakeInt32(30)),
				ConstantAsMetadata::get(MakeInt32(100)),
				ConstantAsMetadata::get(MakeInt32(9)),
				ConstantAsMetadata::get(MakeInt32(9)),
				ConstantAsMetadata::get(MakeInt32(9)),
				ConstantAsMetadata::get(MakeInt32(50)) });
			return node;
		}
		void PWType::InitializeType(NomBuilder& builder, TypeKind kind, llvm::Value* hash, PWNomType nomtypeptr, PWCastFunction castFun) const
		{
			MakeInvariantStore(builder, MakeInt(kind), RTTypeHead::GetLLVMType(), wrapped, MakeInt32(RTTypeHeadFields::Kind));
			MakeInvariantStore(builder, hash, RTTypeHead::GetLLVMType(), wrapped, MakeInt32(RTTypeHeadFields::Hash));
			MakeInvariantStore(builder, nomtypeptr, RTTypeHead::GetLLVMType(), wrapped, MakeInt32(RTTypeHeadFields::NomType));
			MakeInvariantStore(builder, castFun, RTTypeHead::GetLLVMType(), wrapped, MakeInt32(RTTypeHeadFields::CastFun));
		}
		llvm::Value* PWType::ReadKind(NomBuilder& builder) const
		{
			return MakeInvariantLoad(builder, RTTypeHead::GetLLVMType(), wrapped, MakeInt32(RTTypeHeadFields::Kind), "typeKind");
		}
		llvm::Value* PWType::ReadHash(NomBuilder& builder) const
		{
			return MakeInvariantLoad(builder, RTTypeHead::GetLLVMType(), wrapped, MakeInt32(RTTypeHeadFields::Hash), "typeHash");
		}
		PWCastFunction PWType::ReadCastFun(NomBuilder& builder) const
		{
			return MakeInvariantLoad(builder, RTTypeHead::GetLLVMType(), wrapped, MakeInt32(RTTypeHeadFields::CastFun), "castfun");
		}
		/// <summary>
		/// 
		/// </summary>
		/// <param name="builder"></param>
		/// <param name="branchWeights"></param>
		/// <param name="substitutions"></param>
		/// <param name="onClassType"></param>
		/// <param name="onInstanceType"></param>
		/// <param name="resolveTypeVars"></param>
		/// <param name="onMaybeType"></param>
		/// <param name="onBotType"></param>
		/// <param name="onTopType"></param>
		/// <param name="onDynType"></param>
		/// <param name="onTypeVar">Must be null if resolveTypeVars is set to true, and substitutions must not be null</param>
		/// <param name="defaultHandler"></param>
		void PWType::GenerateTypeKindSwitch(NomBuilder& builder,
			MDNode* branchWeights,
			PWSubstStack substitutions,
			std::function<void(NomBuilder&, PWClassType, PWSubstStack)> onClassType, 
			std::function<void(NomBuilder&, PWInstanceType, PWSubstStack)> onInstanceType, 
			bool resolveTypeVars, 
			std::function<void(NomBuilder&, PWMaybeType, PWSubstStack)> onMaybeType, 
			std::function<void(NomBuilder&, PWBotType, PWSubstStack)> onBotType, 
			std::function<void(NomBuilder&, PWTopType, PWSubstStack)> onTopType, 
			std::function<void(NomBuilder&, PWDynType, PWSubstStack)> onDynType, 
			std::function<void(NomBuilder&, PWTypeVar, PWSubstStack)> onTypeVar, 
			std::function<void(NomBuilder&, PWType, PWSubstStack)> defaultHandler) const
		{
			BasicBlock* startBlock = builder->GetInsertBlock();
			BasicBlock* typeVarBlock=nullptr;
			BasicBlock* classTypeBlock=nullptr;
			BasicBlock* instanceTypeBlock=nullptr;
			BasicBlock* maybeTypeBlock=nullptr;
			BasicBlock* topTypeBlock=nullptr;
			BasicBlock* botTypeBlock = nullptr;
			BasicBlock* dynTypeBlock = nullptr;
			BasicBlock* outBlock = BasicBlock::Create(builder->getContext(), "typeSwitchOut", builder.GetFunction());

			PWType tp = *this;
			unsigned int casecount = 0;

			if (resolveTypeVars)
			{
				if (onTypeVar)
				{
					throw new std::exception();
				}
				if (substitutions == nullptr)
				{
					throw new std::exception();
				}
				BasicBlock* headBlock = BasicBlock::Create(builder->getContext(), "typeSwitchStart", builder.GetFunction(), outBlock);
				builder->CreateBr(headBlock);

				builder->SetInsertPoint(headBlock);
				PWPhi<PWType> typeVarPHI = PWPhi<PWType>::CreatePtr(builder, 2, "typePHI");
				PWPhi<PWSubstStack> substStackPHI = PWPhi<PWSubstStack>::CreatePtr(builder, 2, "substPHI");
				typeVarPHI->addIncoming(this->operator llvm::Value * (), startBlock);
				substStackPHI->addIncoming(substitutions, startBlock);
				
				typeVarBlock = BasicBlock::Create(builder->getContext(), "typeVarResolve", builder.GetFunction());

				builder->SetInsertPoint(typeVarBlock);
				PWTypeVar var = typeVarPHI.wrapped;
				PWSubstStack newStack;
				PWType newType=substStackPHI.operator Nom::Runtime::PWSubstStack().Pop(builder, var, &newStack);
				typeVarPHI->addIncoming(newType, builder->GetInsertBlock());
				substStackPHI->addIncoming(newStack, builder->GetInsertBlock());
				builder->CreateBr(headBlock);

				substitutions = substStackPHI;
				tp = typeVarPHI;
				startBlock = headBlock;
				casecount++;
			}

			BasicBlock* defaultBlock = BasicBlock::Create(builder->getContext(), "typeSwitchDefault", builder.GetFunction(), outBlock);
			builder->SetInsertPoint(defaultBlock);
			if (defaultHandler)
			{
				defaultHandler(builder, tp, substitutions);
				if (builder->GetInsertBlock()->getTerminator() == nullptr)
				{
					builder->CreateBr(outBlock);
				}
			}
			else
			{
				RTOutput_Fail::MakeBlockFailOutputBlock(builder, "Unexpected type kind encountered!", defaultBlock);
			}

			if (onClassType)
			{
				classTypeBlock = BasicBlock::Create(builder->getContext(), "classTypeBlock", builder.GetFunction(), defaultBlock);
				builder->SetInsertPoint(classTypeBlock);
				onClassType(builder, tp.wrapped, substitutions);
				if (builder->GetInsertBlock()->getTerminator() == nullptr)
				{
					builder->CreateBr(outBlock);
				}
				casecount++;
			}
			if (onInstanceType)
			{
				instanceTypeBlock = BasicBlock::Create(builder->getContext(), "instanceTypeBlock", builder.GetFunction(), defaultBlock);
				builder->SetInsertPoint(instanceTypeBlock);
				onInstanceType(builder, tp.wrapped, substitutions);
				if (builder->GetInsertBlock()->getTerminator() == nullptr)
				{
					builder->CreateBr(outBlock);
				}
				casecount++;
			}
			if (onMaybeType)
			{
				maybeTypeBlock = BasicBlock::Create(builder->getContext(), "maybeTypeBlock", builder.GetFunction(), defaultBlock);
				builder->SetInsertPoint(maybeTypeBlock);
				onMaybeType(builder, tp.wrapped, substitutions);
				if (builder->GetInsertBlock()->getTerminator() == nullptr)
				{
					builder->CreateBr(outBlock);
				}
				casecount++;
			}
			if (onTypeVar)
			{
				typeVarBlock = BasicBlock::Create(builder->getContext(), "typeVarBlock", builder.GetFunction(), defaultBlock);
				builder->SetInsertPoint(typeVarBlock);
				onTypeVar(builder, tp.wrapped, substitutions);
				if (builder->GetInsertBlock()->getTerminator() == nullptr)
				{
					builder->CreateBr(outBlock);
				}
				casecount++;
			}
			if (onBotType)
			{
				botTypeBlock = BasicBlock::Create(builder->getContext(), "botTypeBlock", builder.GetFunction(), defaultBlock);
				builder->SetInsertPoint(botTypeBlock);
				onBotType(builder, tp.wrapped, substitutions);
				if (builder->GetInsertBlock()->getTerminator() == nullptr)
				{
					builder->CreateBr(outBlock);
				}
				casecount++;
			}
			if (onTopType)
			{
				topTypeBlock = BasicBlock::Create(builder->getContext(), "topTypeBlock", builder.GetFunction(), defaultBlock);
				builder->SetInsertPoint(topTypeBlock);
				onTopType(builder, tp.wrapped, substitutions);
				if (builder->GetInsertBlock()->getTerminator() == nullptr)
				{
					builder->CreateBr(outBlock);
				}
				casecount++;
			}
			if (onDynType)
			{
				dynTypeBlock = BasicBlock::Create(builder->getContext(), "dynTypeBlock", builder.GetFunction(), defaultBlock);
				builder->SetInsertPoint(dynTypeBlock);
				onDynType(builder, tp.wrapped, substitutions);
				if (builder->GetInsertBlock()->getTerminator() == nullptr)
				{
					builder->CreateBr(outBlock);
				}
				casecount++;
			}

			if (casecount < 7&&branchWeights->getNumOperands()==9)
			{
				llvm::SmallVector<Metadata*, 8> args;
				args.push_back(branchWeights->getOperand(0));
				args.push_back(branchWeights->getOperand(1));
				if (classTypeBlock != nullptr)
				{
					args.push_back(branchWeights->getOperand(2));
				}
				if (instanceTypeBlock != nullptr)
				{
					args.push_back(branchWeights->getOperand(3));
				}
				if (typeVarBlock != nullptr)
				{
					args.push_back(branchWeights->getOperand(4));
				}
				if (maybeTypeBlock != nullptr)
				{
					args.push_back(branchWeights->getOperand(5));
				}
				if (botTypeBlock != nullptr)
				{
					args.push_back(branchWeights->getOperand(6));
				}
				if (topTypeBlock != nullptr)
				{
					args.push_back(branchWeights->getOperand(7));
				}
				if (dynTypeBlock != nullptr)
				{
					args.push_back(branchWeights->getOperand(8));
				}
				branchWeights = MDNode::get(builder->getContext(), args);
			}

			builder->SetInsertPoint(startBlock);
			Value* kind = ReadKind(builder);
			auto kindswitch = builder->CreateSwitch(kind, defaultBlock, casecount, branchWeights);

			if (classTypeBlock != nullptr)
			{
				kindswitch->addCase(MakeIntLike(kind, static_cast<uint64_t>(TypeKind::TKClass)), classTypeBlock);
			}
			if (instanceTypeBlock != nullptr)
			{
				kindswitch->addCase(MakeIntLike(kind, static_cast<uint64_t>(TypeKind::TKInstance)), instanceTypeBlock);
			}
			if (typeVarBlock != nullptr)
			{
				kindswitch->addCase(MakeIntLike(kind, static_cast<uint64_t>(TypeKind::TKVariable)), typeVarBlock);
			}
			if (maybeTypeBlock != nullptr)
			{
				kindswitch->addCase(MakeIntLike(kind, static_cast<uint64_t>(TypeKind::TKMaybe)), maybeTypeBlock);
			}
			if (botTypeBlock != nullptr)
			{
				kindswitch->addCase(MakeIntLike(kind, static_cast<uint64_t>(TypeKind::TKBottom)), botTypeBlock);
			}
			if (topTypeBlock != nullptr)
			{
				kindswitch->addCase(MakeIntLike(kind, static_cast<uint64_t>(TypeKind::TKTop)), topTypeBlock);
			}
			if (dynTypeBlock != nullptr)
			{
				kindswitch->addCase(MakeIntLike(kind, static_cast<uint64_t>(TypeKind::TKDynamic)), dynTypeBlock);
			}

			builder->SetInsertPoint(outBlock);
		}

		PWPhi<PWBool> PWType::GenerateEqPtrCheck(NomBuilder& builder, PWType other, bool checkPtrEq, BasicBlock** startBlock, BasicBlock** mergeBlock) const
		{
			*startBlock = builder->GetInsertBlock();
			BasicBlock* deepCheckBlock;
			*mergeBlock = BasicBlock::Create(builder->getContext(), "typeEqMerge", builder.GetFunction());

			builder->SetInsertPoint(*mergeBlock);
			PWPhi<PWBool> retPHI = PWPhi<PWBool>::Create(builder, 6 + (checkPtrEq ? 1 : 0), "typeEqBool");

			if (checkPtrEq)
			{
				deepCheckBlock = BasicBlock::Create(builder->getContext(), "typeEqDeepCheck", builder.GetFunction(), *mergeBlock);
				auto eq = CreatePointerEq(builder, wrapped, other, "typePtrEq");
				builder->CreateCondBr(eq, *mergeBlock, deepCheckBlock);
				retPHI->addIncoming(MakeUInt(1, 1), *startBlock);

				builder->SetInsertPoint(deepCheckBlock);
			}
		}

		PWBool PWType::IsEq(NomBuilder& builder, PWType other, PWSubstStack ownSubst, PWSubstStack otherSubst, bool optimistic, bool checkPtrEq) const
		{
			BasicBlock* startBlock;
			BasicBlock* mergeBlock;
			PWPhi<PWBool> retPHI = GenerateEqPtrCheck(builder, other, checkPtrEq, &startBlock, &mergeBlock);

			other.GenerateTypeKindSwitch(builder, GetDefaultTypeCasesNode(), ownSubst,
				[&retPHI, this, &ownSubst, optimistic, &mergeBlock](NomBuilder& b, PWClassType type, PWSubstStack substs) -> void { auto sub = this->IsEq(b, type, ownSubst, substs, optimistic, false); retPHI->addIncoming(sub, b->GetInsertBlock()); b->CreateBr(mergeBlock); },
				[&retPHI, this, &ownSubst, optimistic, &mergeBlock](NomBuilder& b, PWInstanceType type, PWSubstStack substs) -> void { auto sub = this->IsEq(b, type, ownSubst, substs, optimistic, false); retPHI->addIncoming(sub, b->GetInsertBlock()); b->CreateBr(mergeBlock); },
				true,
				[&retPHI, this, &ownSubst, optimistic, &mergeBlock](NomBuilder& b, PWMaybeType type, PWSubstStack substs) -> void { auto sub = this->IsEq(b, type, ownSubst, substs, optimistic, false); retPHI->addIncoming(sub, b->GetInsertBlock()); b->CreateBr(mergeBlock); },
				[&retPHI, this, &ownSubst, optimistic, &mergeBlock](NomBuilder& b, PWBotType type, PWSubstStack substs) -> void { auto sub = this->IsEq(b, type, ownSubst, substs, optimistic, false); retPHI->addIncoming(sub, b->GetInsertBlock()); b->CreateBr(mergeBlock); },
				[&retPHI, this, &ownSubst, optimistic, &mergeBlock](NomBuilder& b, PWTopType type, PWSubstStack substs) -> void { auto sub = this->IsEq(b, type, ownSubst, substs, optimistic, false); retPHI->addIncoming(sub, b->GetInsertBlock()); b->CreateBr(mergeBlock); },
				[&retPHI, this, &ownSubst, optimistic, &mergeBlock](NomBuilder& b, PWDynType type, PWSubstStack substs) -> void { auto sub = this->IsEq(b, type, ownSubst, substs, optimistic, false); retPHI->addIncoming(sub, b->GetInsertBlock()); b->CreateBr(mergeBlock); });

			builder->SetInsertPoint(mergeBlock);
			return retPHI;
		}
		PWBool PWType::IsEq(NomBuilder& builder, PWClassType other, PWSubstStack ownSubst, PWSubstStack otherSubst, bool optimistic, bool checkPtrEq) const
		{
			return other.IsEq(builder, *this, otherSubst, ownSubst, optimistic, checkPtrEq);
		}
		PWBool PWType::IsEq(NomBuilder& builder, PWInstanceType other, PWSubstStack ownSubst, PWSubstStack otherSubst, bool optimistic, bool checkPtrEq) const
		{
			llvm::Value* blck = builder->GetInsertBlock();
			
			return PWBool();
		}
		PWBool PWType::IsEq(NomBuilder& builder, PWTypeVar other, PWSubstStack ownSubst, PWSubstStack otherSubst, bool optimistic, bool checkPtrEq) const
		{
			return PWBool();
		}
		PWBool PWType::IsEq(NomBuilder& builder, PWMaybeType other, PWSubstStack ownSubst, PWSubstStack otherSubst, bool optimistic, bool checkPtrEq) const
		{
			return PWBool();
		}
		PWBool PWType::IsEq(NomBuilder& builder, PWTopType other, PWSubstStack ownSubst, PWSubstStack otherSubst, bool optimistic, bool checkPtrEq) const
		{
			return PWBool();
		}
		PWBool PWType::IsEq(NomBuilder& builder, PWBotType other, PWSubstStack ownSubst, PWSubstStack otherSubst, bool optimistic, bool checkPtrEq) const
		{
			if(optimistic)
			{
				GenerateTypeKindSwitch(builder, GetExpectBotTypeNode(), ownSubst, nullptr, nullptr, true, )
		}
		PWBool PWType::IsEq(NomBuilder& builder, PWDynType other, PWSubstStack ownSubst, PWSubstStack otherSubst, bool optimistic, bool checkPtrEq) const
		{
			return PWBool();
		}
		PWBool PWType::IsEq(NomBuilder& builder, NomTypeRef other, PWSubstStack ownSubst, PWSubstStack otherSubst, bool optimistic, bool checkPtrEq) const
		{
			//BasicBlock* varUnfoldBlock=BasicBlock::Create()
			//auto kind = ReadKind(builder);
			//auto isTypeVar = builder->CreateICmpEQ(kind, MakeIntLike(kind, static_cast<uint64_t>(TypeKind::TKVariable)));
			//builder->CreateCondBr(isTypeVar, )
			//switch (other->GetKind())
			//{
			//	case TypeKind::TKClass:
			//	case TypeKind::TKInstance:
			//	{
			//		break;
			//	}
			//	case TypeKind::TKBottom:
			//	{
			//		break;
			//	}
			//	case TypeKind::TKTop:
			//	{
			//		break;
			//	}
			//	case TypeKind::TKLambda:
			//	case TypeKind::TKRecord:
			//	case TypeKind::TKPartialApp:
			//	case TypeKind::TKDynamic:
			//	{
			//		break;
			//	}
			//	case TypeKind::TKMaybe:
			//	{
			//		break;
			//	}
			//	case TypeKind::TKVariable:
			//	{
			//		break;
			//	}
			//}
		}
		PWBool PWType::IsSubtype(NomBuilder& builder, PWType other, PWSubstStack ownSubst, PWSubstStack otherSubst, bool optimistic) const
		{
			return PWBool();
		}
		PWBool PWType::IsSubtype(NomBuilder& builder, NomTypeRef other, PWSubstStack ownSubst, PWSubstStack otherSubst, bool optimistic) const
		{
			return PWBool();
		}
	}
}
