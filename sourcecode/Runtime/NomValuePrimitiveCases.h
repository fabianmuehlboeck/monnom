#pragma once
#include "NomValue.h"
PUSHDIAGSUPPRESSION
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/IRBuilder.h"
POPDIAGSUPPRESSION
#include "NomType.h"
#include "IntClass.h"
#include "FloatClass.h"
#include "NomClassType.h"
#include "Context.h"
#include "CompileHelpers.h"

namespace Nom
{
	namespace Runtime
	{
		template<class HandleInt, class HandleFloat, class HandleRef>
		void GeneratePrimitiveTypeCase(NomBuilder& builder, NomValue& value, HandleInt handleInt, HandleFloat handleFloat, HandleRef handleRef, NomValue* returnValues, int numReturns)
		{
			llvm::BasicBlock* startBlock = builder->GetInsertBlock();

			auto vtype = value.GetNomType();
			auto inttype = NomIntClass::GetInstance()->GetType();
			auto floattype = NomFloatClass::GetInstance()->GetType();
			bool intPossible = !vtype->IsDisjoint(inttype);
			bool floatPossible = !vtype->IsDisjoint(floattype);

			if ((!intPossible) && (!floatPossible))
			{
				handleRef(builder, value, nullptr, returnValues);
				return;
			}
			if (value->getType()->isIntegerTy(64) || vtype->IsSubtype(inttype, false))
			{
				handleInt(builder, value, returnValues);
				return;
			}
			if (value->getType()->isFloatingPointTy() || vtype->IsSubtype(floattype, false))
			{
				handleFloat(builder, value, returnValues);
				return;
			}

			if (value->getType() != REFTYPE)
			{
				throw new std::exception();
			}

			llvm::BasicBlock* intBlock = BasicBlock::Create(LLVMCONTEXT, "INTBLOCK", startBlock->getParent());
			llvm::BasicBlock* posintBlock = intBlock;
			llvm::BasicBlock* negintBlock = intBlock;
			llvm::BasicBlock* extractintBlock = intBlock;

			if (intPossible)
			{
				posintBlock = BasicBlock::Create(LLVMCONTEXT, "PINTTAG", startBlock->getParent(), intBlock);
				negintBlock = BasicBlock::Create(LLVMCONTEXT, "NINTTAG", startBlock->getParent(), intBlock);
				extractintBlock = BasicBlock::Create(LLVMCONTEXT, "INTEXTRACT", startBlock->getParent(), intBlock);
			}

			llvm::BasicBlock* floatBlock = BasicBlock::Create(LLVMCONTEXT, "FLOATBLOCK", startBlock->getParent());
			llvm::BasicBlock* floatUnpackBlock = floatBlock;
			llvm::BasicBlock* extractfloatBlock = floatBlock;

			if (floatPossible)
			{
				floatUnpackBlock = BasicBlock::Create(LLVMCONTEXT, "FLOATTAG", startBlock->getParent());
				extractfloatBlock = BasicBlock::Create(LLVMCONTEXT, "FLOATEXTRACT", startBlock->getParent());
			}

			llvm::BasicBlock* refBlock = BasicBlock::Create(LLVMCONTEXT, "REFBLOCK", startBlock->getParent());
			llvm::BasicBlock* refCheckBlock = BasicBlock::Create(LLVMCONTEXT, "REFTAG", startBlock->getParent());
			llvm::BasicBlock* returnBlock = BasicBlock::Create(LLVMCONTEXT, "TCRETURN", startBlock->getParent());

			auto refAsInt = builder->CreatePtrToInt(value, INTTYPE, "refasint");
			auto tag = builder->CreateTrunc(refAsInt, inttype(3), "tag");
			auto swtch = builder->CreateSwitch(tag, floatUnpackBlock, 3);
			swtch->addCase(MakeInt(3, (int64_t)0), refCheckBlock);
			swtch->addCase(MakeInt(3, (int64_t)3), posintBlock);
			swtch->addCase(MakeInt(3, (int64_t)7), negintBlock);

			NomValue* intReturns = makealloca(NomValue, numReturns);
			NomValue* floatReturns = makealloca(NomValue, numReturns);
			NomValue* refReturns = makealloca(NomValue, numReturns);

			builder->SetInsertPoint(refCheckBlock);
			auto vtableptr = builder->CreatePtrToInt(ObjectHeader::GenerateReadVTablePointer(builder, value), numtype(intptr_t));
			if (!intPossible)
			{
				auto isFloat = builder->CreateICmpEQ(vtableptr, ConstantExpr::getPtrToInt(NomFloatClass::GetInstance()->GetLLVMElement(*startBlock->getParent()->getParent()), numtype(intptr_t)));
				builder->CreateCondBr(isFloat, extractfloatBlock, refBlock);
			}
			else if (!floatPossible)
			{
				auto isInt = builder->CreateICmpEQ(vtableptr, ConstantExpr::getPtrToInt(NomIntClass::GetInstance()->GetLLVMElement(*startBlock->getParent()->getParent()), numtype(intptr_t)));
				builder->CreateCondBr(isInt, extractintBlock, refBlock);
			}
			else
			{
				auto isInt = builder->CreateICmpEQ(vtableptr, ConstantExpr::getPtrToInt(NomIntClass::GetInstance()->GetLLVMElement(*startBlock->getParent()->getParent()), numtype(intptr_t)));
				auto isFloat = builder->CreateICmpEQ(vtableptr, ConstantExpr::getPtrToInt(NomFloatClass::GetInstance()->GetLLVMElement(*startBlock->getParent()->getParent()), numtype(intptr_t)));
				auto combo = builder->CreateOr(builder->CreateShl(builder->CreateZExt(isInt, inttype(2)), MakeUInt(2, 1)), builder->CreateZExt(isFloat, inttype(2)));
				auto cswitch = builder->CreateSwitch(combo, refBlock, 2);
				cswitch->addCase(MakeUInt(2, 1), extractfloatBlock);
				cswitch->addCase(MakeUInt(2, 2), extractintBlock);
			}

			builder->SetInsertPoint(refBlock);
			handleRef(builder, value, vtableptr, refReturns);
			refBlock = builder->GetInsertBlock();
			builder->CreateBr(returnBlock);


			if (intPossible)
			{
				builder->SetInsertPoint(extractintBlock);
				auto extractInt = builder->CreatePtrToInt(ObjectHeader::ReadField(builder, value, MakeInt32(0), false), INTTYPE, "unboxedInt");
				builder->CreateBr(intBlock);

				builder->SetInsertPoint(posintBlock);
				auto intMask = GetMask(64, 0, 2);
				auto prefunnel = builder->CreateAnd(intMask, refAsInt);
				auto posFsh = builder->CreateIntrinsic(Intrinsic::fshr, { prefunnel->getType() }, { prefunnel, prefunnel, ConstantInt::get(prefunnel->getType(), 3) });
				builder->CreateBr(intBlock);

				builder->SetInsertPoint(negintBlock);
				auto negFsh = builder->CreateIntrinsic(Intrinsic::fshr, { refAsInt->getType() }, {refAsInt, refAsInt, ConstantInt::get(refAsInt->getType(), 3)});
				builder->CreateBr(intBlock);

				builder->SetInsertPoint(intBlock);
				auto intval = builder->CreatePHI(INTTYPE, 3, "INTVAL");
				intval->addIncoming(posFsh, posintBlock);
				intval->addIncoming(negFsh, negintBlock);
				intval->addIncoming(extractInt, extractintBlock);
				auto nv = NomValue(intval, inttype, false);
				handleInt(builder, nv, intReturns);
				intBlock = builder->GetInsertBlock();
				builder->CreateBr(returnBlock);
			}
			else
			{
				builder->SetInsertPoint(intBlock); 
				builder->CreateIntrinsic(Intrinsic::trap, {}, {});
				builder->CreateBr(returnBlock);
			}

			if (floatPossible)
			{
				BasicBlock* normalValueBlock = BasicBlock::Create(LLVMCONTEXT, "normalValueBlock", startBlock->getParent());
				BasicBlock* posZeroBlock = BasicBlock::Create(LLVMCONTEXT, "posZeroBlock", startBlock->getParent());
				BasicBlock* negZeroBlock = BasicBlock::Create(LLVMCONTEXT, "negZeroBlock", startBlock->getParent());

				builder->SetInsertPoint(extractfloatBlock);
				auto boxField = builder->CreateBitCast(builder->CreatePtrToInt(ObjectHeader::ReadField(builder, value, MakeInt32(0), false), INTTYPE), FLOATTYPE, "unboxedFloat");
				builder->CreateBr(floatBlock);

				builder->SetInsertPoint(floatUnpackBlock);

				auto checkZeroSwitch = builder->CreateSwitch(refAsInt, normalValueBlock, 2);
				checkZeroSwitch->addCase(MakeUInt(64, 1), posZeroBlock);
				checkZeroSwitch->addCase(MakeUInt(64, 5), negZeroBlock);

				builder->SetInsertPoint(normalValueBlock);
				auto unmaskedFloat = builder->CreateBitCast(builder->CreateIntrinsic(Intrinsic::fshr, { refAsInt->getType() }, { refAsInt, refAsInt, ConstantInt::get(refAsInt->getType(), 3) }), FLOATTYPE, "unmaskedFloat");
				builder->CreateBr(floatBlock);


				builder->SetInsertPoint(posZeroBlock);
				builder->CreateBr(floatBlock);

				builder->SetInsertPoint(negZeroBlock);
				builder->CreateBr(floatBlock);

				builder->SetInsertPoint(floatBlock);
				auto floatval = builder->CreatePHI(FLOATTYPE, 4, "FLOATVAL");
				floatval->addIncoming(boxField, extractfloatBlock);
				floatval->addIncoming(unmaskedFloat, normalValueBlock);
				floatval->addIncoming(ConstantFP::get(FLOATTYPE, 0.0), posZeroBlock);
				floatval->addIncoming(ConstantFP::getNegativeZero(FLOATTYPE), negZeroBlock);
				auto nv = NomValue(floatval, floattype, false);
				handleFloat(builder, nv, floatReturns);
				floatBlock = builder->GetInsertBlock();
				builder->CreateBr(returnBlock);
			}
			else
			{
				builder->SetInsertPoint(floatBlock); 
				builder->CreateIntrinsic(Intrinsic::trap, {}, {});
				builder->CreateBr(returnBlock);
			}

			builder->SetInsertPoint(returnBlock);
			int casenum = 1 + (intPossible ? 1 : 0) + (floatPossible ? 1 : 0);
			for (int i = 0; i < numReturns; i++)
			{
				auto returnPHI = builder->CreatePHI(refReturns[i]->getType(), casenum, Twine("tcret").concat(Twine(i)));
				auto returnType = refReturns[i].GetNomType();
				auto isFCall = refReturns[i].IsFunctionCall();
				if (intPossible)
				{
					returnPHI->addIncoming(intReturns[i], intBlock);
					returnType = JoinTypes(returnType,intReturns[i].GetNomType());
					isFCall = isFCall && intReturns[i].IsFunctionCall();
				}
				if (floatPossible)
				{
					returnPHI->addIncoming(floatReturns[i], floatBlock);
					returnType = JoinTypes(returnType,floatReturns[i].GetNomType());
					isFCall = isFCall && floatReturns[i].IsFunctionCall();
				}
				returnPHI->addIncoming(refReturns[i], refBlock);
				returnValues[i] = NomValue(returnPHI, returnType, isFCall);
			}
		}

	}
}
