#include "TypeOperations.h"
#include "BoolClass.h"
#include "IntClass.h"
#include "FloatClass.h"
#include "NomClassType.h"
#include "instructions/CastInstruction.h"
#include "CompileHelpers.h"
#include "NomVMInterface.h"
#include "RTConfig.h"
#include "RefValueHeader.h"
#include "RTOutput.h"
#include "CastStats.h"
#include "NomBuilder.h"

using namespace llvm;
using namespace std;
namespace Nom
{
	namespace Runtime
	{
		//llvm::SwitchInst* CreateRefKindSwitch(NomBuilder &builder, llvm::Value* refval, llvm::BasicBlock * defaultBlock, llvm::BasicBlock* refBlock, llvm::BasicBlock* intBlock, llvm::BasicBlock* floatBlock)
		//{
		//	if (refval->getType() != INTTYPE && !refval->getType()->isIntegerTy(2))
		//	{
		//		if (refval->getType() != REFTYPE)
		//		{
		//			throw new std::exception();
		//		}
		//		refval = builder->CreatePtrToInt(refval, INTTYPE, "refAsInt");
		//	}
		//	auto tag = refval;
		//	if (tag->getType()->getPrimitiveSizeInBits() > 2)
		//	{
		//		tag = builder->CreateTrunc(refval, inttype(2), "refTag");
		//	}
		//	int caseCount = 0;
		//	if (refBlock != nullptr)
		//	{
		//		caseCount++;
		//	}
		//	if (intBlock != nullptr)
		//	{
		//		caseCount++;
		//	}
		//	if (floatBlock != nullptr)
		//	{
		//		caseCount+=2;
		//	}
		//	auto switchInst = builder->CreateSwitch(tag, defaultBlock, caseCount);
		//	if (refBlock != nullptr)
		//	{
		//		switchInst->addCase(MakeUInt(2, 0), refBlock);
		//	}
		//	if (intBlock != nullptr)
		//	{
		//		switchInst->addCase(MakeUInt(2, 3), intBlock);
		//	}
		//	if (floatBlock != nullptr)
		//	{
		//		switchInst->addCase(MakeUInt(2, 1), floatBlock);
		//		switchInst->addCase(MakeUInt(2, 2), floatBlock);
		//	}
		//	return switchInst;
		//}

		llvm::Value* EnsureUnpackedInt(NomBuilder &builder, CompileEnv* env, NomValue& val)
		{
			if (val->getType()->isIntegerTy(INTTYPE->getIntegerBitWidth()))
			{
				return val;
			}
			return UnpackInt(builder, CastInstruction::MakeCast(builder, env, val, NomIntClass::GetInstance()->GetType()));
		}

		llvm::Value* EnsureUnpackedFloat(NomBuilder &builder, CompileEnv* env, NomValue& val)
		{
			if (val->getType()->isDoubleTy())
			{
				return val;
			}
			return UnpackFloat(builder, CastInstruction::MakeCast(builder, env, val, NomFloatClass::GetInstance()->GetType()));
		}

		llvm::Value* EnsureUnpackedBool(NomBuilder &builder, CompileEnv* env, NomValue& val)
		{
			if (val->getType()->isIntegerTy(1))
			{
				return val;
			}
			return UnpackBool(builder, CastInstruction::MakeCast(builder, env, val, NomBoolClass::GetInstance()->GetType()));
		}

		NomValue EnsurePacked(NomBuilder& builder, NomValue& val)
		{
			if (val->getType()->isIntegerTy(1))
			{
				return NomValue(PackBool(builder, val), val.GetNomType());
			}
			if (val->getType()->isIntegerTy(INTTYPE->getPrimitiveSizeInBits()))
			{
				return NomValue(PackInt(builder, val), val.GetNomType());
			}
			if (val->getType()->isFloatingPointTy())
			{
				return NomValue(PackFloat(builder, val), val.GetNomType());
			}
			if (val->getType() == REFTYPE)
			{
				return val;
			}
			return NomValue(builder->CreatePointerCast(val, REFTYPE), val.GetNomType());
		}

		llvm::Value* EnsurePacked(NomBuilder& builder, llvm::Value* val)
		{
			if (val->getType()->isIntegerTy(1))
			{
				return PackBool(builder, val);
			}
			if (val->getType()->isIntegerTy(INTTYPE->getPrimitiveSizeInBits()))
			{
				return PackInt(builder, val);
			}
			if (val->getType()->isFloatingPointTy())
			{
				return PackFloat(builder, val);
			}
			if (val->getType() == REFTYPE)
			{
				return val;
			}
			return builder->CreatePointerCast(val, REFTYPE);
		}

		llvm::Value* EnsureType(NomBuilder& builder, CompileEnv* env, NomValue& val, NomTypeRef type, llvm::Type *expected)
		{

			if (type == NomBoolClass::GetInstance()->GetType())
			{
				if (expected->isIntegerTy(1))
				{
					return EnsureUnpackedBool(builder, env, val);
				}
				else if (expected == REFTYPE&&val.GetNomType()->IsSubtype(NomBoolClass::GetInstance()->GetType()))
				{
					return EnsurePacked(builder, val);
				}
				else
				{
					NomValue nv = CastInstruction::MakeCast(builder, env, val, type);
					return EnsurePacked(builder, nv);
				}
			}
			else if (type == NomIntClass::GetInstance()->GetType())
			{
				if (expected->isIntegerTy(INTTYPE->getPrimitiveSizeInBits()))
				{
					return EnsureUnpackedInt(builder, env, val);
				}
				else if (expected == REFTYPE && val.GetNomType()->IsSubtype(NomIntClass::GetInstance()->GetType()))
				{
					return EnsurePacked(builder, val);
				}
				else
				{
					NomValue nv = CastInstruction::MakeCast(builder, env, val, type);
					return EnsurePacked(builder, nv);
				}
			}
			else if (type == NomFloatClass::GetInstance()->GetType())
			{
				if (expected->isDoubleTy())
				{
					return EnsureUnpackedFloat(builder, env, val);
				}
				else if (expected == REFTYPE && val.GetNomType()->IsSubtype(NomIntClass::GetInstance()->GetType()))
				{
					return EnsurePacked(builder, val);
				}
				else
				{
					NomValue nv = CastInstruction::MakeCast(builder, env, val, type);
					return EnsurePacked(builder, nv);
				}
			}
			else
			{
				NomValue nv = CastInstruction::MakeCast(builder, env, val, type);
				return EnsurePacked(builder, nv);
			}
		}
		llvm::Value* EnsureType(NomBuilder& builder, CompileEnv* env, NomValue& val, NomTypeRef type)
		{
			if (type == NomBoolClass::GetInstance()->GetType())
			{
				return EnsureUnpackedBool(builder, env, val);
			}
			else if (type == NomIntClass::GetInstance()->GetType())
			{
				return EnsureUnpackedInt(builder, env, val);
			}
			else if (type == NomFloatClass::GetInstance()->GetType())
			{
				return EnsureUnpackedFloat(builder, env, val);
			}
			else
			{
				NomValue nv = CastInstruction::MakeCast(builder, env, val, type);
				return EnsurePacked(builder, nv);
			}
		}
		llvm::Value* WrapAsPointer(NomBuilder& builder, llvm::Value* val)
		{
			if (val->getType() != POINTERTYPE)
			{
				if (val->getType() == REFTYPE || val->getType() == TYPETYPE)
				{
					return builder->CreatePointerCast(val, POINTERTYPE);
				}
				if (val->getType()->isIntegerTy(INTTYPE->getIntegerBitWidth()))
				{
					return builder->CreateIntToPtr(val, POINTERTYPE);
				}
				if (val->getType()->isDoubleTy())
				{
					return builder->CreateIntToPtr(builder->CreateBitCast(val, INTTYPE), POINTERTYPE);
				}
				if (val->getType()->isIntegerTy(BOOLTYPE->getIntegerBitWidth()))
				{
					return builder->CreateIntToPtr(val, POINTERTYPE);
				}
				throw new std::exception();
			}
			return val;
		}
		//llvm::Value* IsPtr(NomBuilder& builder, llvm::Value* val)
		//{
		//	if (val->getType()->isIntegerTy() || val->getType()->isFloatingPointTy())
		//	{
		//		return llvm::ConstantInt::get(llvm::IntegerType::get(LLVMCONTEXT, 1), 0, false);
		//	}
		//	return builder->CreateICmpEQ(builder->CreateURem(builder->CreatePtrToInt(val, INTTYPE), llvm::ConstantInt::get(INTTYPE, 4, false)), llvm::ConstantInt::get(INTTYPE, 0, false));
		//}
		//llvm::Value* IsInt(NomBuilder& builder, NomValue &refval)
		//{
		//	BasicBlock* incomingBlock = builder->GetInsertBlock();
		//	auto fun = incomingBlock->getParent();

		//	BasicBlock* refValueBlock = nullptr, * packedIntBlock = nullptr, * packedFloatBlock = nullptr, * primitiveIntBlock = nullptr, * primitiveFloatBlock = nullptr, * primitiveBoolBlock = nullptr;
		//	BasicBlock* outBlock = BasicBlock::Create(LLVMCONTEXT,"isIntOut",fun);

		//	int cases = RefValueHeader::GenerateRefOrPrimitiveValueSwitch(builder, refval, &refValueBlock, &packedIntBlock, &packedFloatBlock, false, &primitiveIntBlock, nullptr, &primitiveFloatBlock, nullptr, &primitiveBoolBlock, nullptr);

		//	PHINode* outPHI = nullptr;
		//	if (cases > 0)
		//	{
		//		builder->SetInsertPoint(outBlock);
		//		outPHI = builder->CreatePHI(inttype(1), cases, "isInt");
		//	}

		//	if (refValueBlock != nullptr)
		//	{
		//		builder->SetInsertPoint(refValueBlock);
		//		outPHI->addIncoming(MakeUInt(1, 0), refValueBlock);
		//	}

		//	builder->SetInsertPoint(outBlock);
		//	//BasicBlock* floatBlock = BasicBlock::Create(LLVMCONTEXT, "floatBlock", fun);
		//	//BasicBlock* refBlock = BasicBlock::Create(LLVMCONTEXT, "refBlock", fun);
		//	//BasicBlock* intBlock = BasicBlock::Create(LLVMCONTEXT, "intBlock", fun);
		//	//BasicBlock* outBlock = BasicBlock::Create(LLVMCONTEXT, "outBlock", fun);
		//	//CreateRefKindSwitch(builder, refval, floatBlock, refBlock, intBlock);

		//	//builder->SetInsertPoint(floatBlock);
		//	//builder->CreateBr(outBlock);

		//	//builder->SetInsertPoint(intBlock);
		//	//builder->CreateBr(outBlock);

		//	//builder->SetInsertPoint(refBlock);
		//	//auto vtablePtr = builder->CreatePtrToInt(RefValueHeader::GenerateReadVTablePointer(builder, refval), numtype(intptr_t), "vTablePtr");
		//	//auto matchValue = builder->CreateICmpEQ(vtablePtr, builder->CreatePtrToInt(NomIntClass::GetInstance()->GetLLVMElement(*fun->getParent()), numtype(intptr_t), "intVTable"), "vTableMatch");
		//	//builder->CreateBr(outBlock);

		//	//builder->SetInsertPoint(outBlock);
		//	//auto returnVal = builder->CreatePHI(inttype(1), 3);
		//	//returnVal->addIncoming(matchValue, refBlock);
		//	//returnVal->addIncoming(MakeUInt(1, 1), intBlock);
		//	//returnVal->addIncoming(MakeUInt(1, 0), floatBlock);
		//	//return returnVal;
		//}
		//llvm::Value* IsFloat(NomBuilder& builder, NomValue& refval)
		//{
		//	BasicBlock* incomingBlock = builder->GetInsertBlock();
		//	auto fun = incomingBlock->getParent();
		//	BasicBlock* floatBlock = BasicBlock::Create(LLVMCONTEXT, "floatBlock", fun);
		//	BasicBlock* refBlock = BasicBlock::Create(LLVMCONTEXT, "refBlock", fun);
		//	BasicBlock* intBlock = BasicBlock::Create(LLVMCONTEXT, "intBlock", fun);
		//	BasicBlock* outBlock = BasicBlock::Create(LLVMCONTEXT, "outBlock", fun);
		//	CreateRefKindSwitch(builder, refval, floatBlock, refBlock, intBlock);

		//	builder->SetInsertPoint(floatBlock);
		//	builder->CreateBr(outBlock);

		//	builder->SetInsertPoint(intBlock);
		//	builder->CreateBr(outBlock);

		//	builder->SetInsertPoint(refBlock);
		//	auto vtablePtr = builder->CreatePtrToInt(RefValueHeader::GenerateReadVTablePointer(builder, refval), numtype(intptr_t), "vTablePtr");
		//	auto matchValue = builder->CreateICmpEQ(vtablePtr, builder->CreatePtrToInt(NomFloatClass::GetInstance()->GetLLVMElement(*fun->getParent()), numtype(intptr_t), "intVTable"), "vTableMatch");
		//	builder->CreateBr(outBlock);

		//	builder->SetInsertPoint(outBlock);
		//	auto returnVal = builder->CreatePHI(inttype(1), 3);
		//	returnVal->addIncoming(matchValue, refBlock);
		//	returnVal->addIncoming(MakeUInt(1, 0), intBlock);
		//	returnVal->addIncoming(MakeUInt(1, 1), floatBlock);
		//	return returnVal;
		//	/*llvm::Value* floatmask = (llvm::ConstantInt::get(INTTYPE, 2, false));
		//	return builder->CreateICmpUGT(builder->CreateTrunc(builder->CreatePtrToInt(refval, INTTYPE), llvm::IntegerType::get(LLVMCONTEXT, 2)), MakeInt(2, (uint64_t)1));*/
		//}
		llvm::Value* PackInt(NomBuilder& builder, llvm::Value* intval)
		{
			if (intval->getType() != INTTYPE)
			{
				throw new std::exception();
			}
			if (NomCastStats)
			{
				builder->CreateCall(GetIncIntPacksFunction(*builder->GetInsertBlock()->getParent()->getParent()), {});
			}
			BasicBlock* incomingBlock = builder->GetInsertBlock();
			auto fun = incomingBlock->getParent();
			BasicBlock* maskedIntBlock = BasicBlock::Create(LLVMCONTEXT, "maskIntBlock", fun);
			BasicBlock* boxedIntBlock = BasicBlock::Create(LLVMCONTEXT, "boxIntBlock", fun);
			BasicBlock* outBlock = BasicBlock::Create(LLVMCONTEXT, "packedIntBlock", fun);
			auto fshresult = builder->CreateIntrinsic(Intrinsic::fshl, { intval->getType() }, { intval, intval, ConstantInt::get(intval->getType(), 3) });
			auto intSwitch = builder->CreateSwitch(builder->CreateTrunc(fshresult, inttype(3)), boxedIntBlock);
			intSwitch->addCase(ConstantInt::get(inttype(3), 7), maskedIntBlock);
			intSwitch->addCase(ConstantInt::get(inttype(3), 0), maskedIntBlock);

			builder->SetInsertPoint(maskedIntBlock);
			auto maskedInt = builder->CreateOr(fshresult, ConstantInt::get(fshresult->getType(), 3));
			auto packedInt = builder->CreateIntToPtr(maskedInt, REFTYPE);
			builder->CreateBr(outBlock);

			builder->SetInsertPoint(boxedIntBlock);
			if (NomCastStats)
			{
				builder->CreateCall(GetIncIntBoxesFunction(*builder->GetInsertBlock()->getParent()->getParent()), {});
			}
			auto allocfun = GetNewAlloc(fun->getParent());
			auto boxedInt = builder->CreateCall(allocfun, { MakeInt<size_t>(1), MakeInt<size_t>(0) }, "boxedInt");
			RefValueHeader::GenerateWriteVTablePointer(builder, boxedInt, NomIntClass::GetInstance()->GetLLVMElement(*fun->getParent()));
			ObjectHeader::WriteField(builder, boxedInt, MakeInt32(0), builder->CreateIntToPtr(intval, REFTYPE), false);
			boxedIntBlock = builder->GetInsertBlock();
			builder->CreateBr(outBlock);
			
			builder->SetInsertPoint(outBlock);
			auto intPHI = builder->CreatePHI(REFTYPE, 2, "packedInt");
			intPHI->addIncoming(packedInt, maskedIntBlock);
			intPHI->addIncoming(boxedInt, boxedIntBlock);
			return intPHI;
		}
		llvm::Value* UnpackInt(NomBuilder& builder, NomValue value)
		{
			Function* fun = builder->GetInsertBlock()->getParent();
			BasicBlock* refValueBlock = nullptr, * posMaskedIntBlock = nullptr, *negMaskedIntBlock = nullptr, * floatBlock = nullptr, * primitiveIntBlock = nullptr;
			Value* primitiveIntVal = nullptr;
			auto refAsInt = builder->CreatePtrToInt(value, INTTYPE);
			RefValueHeader::GenerateRefOrPrimitiveValueSwitch(builder, value, &refValueBlock, &posMaskedIntBlock, &negMaskedIntBlock, &floatBlock, &floatBlock, &floatBlock, false, &primitiveIntBlock, &primitiveIntVal, nullptr, nullptr, nullptr, nullptr);

			int phiCases = (refValueBlock != nullptr ? 1 : 0) + (posMaskedIntBlock != nullptr ? 1 : 0) + (negMaskedIntBlock != nullptr ? 1 : 0) + (primitiveIntBlock != nullptr ? 1 : 0);

			BasicBlock* mergeBlock = BasicBlock::Create(LLVMCONTEXT, "intUnpackMerge", fun);
			builder->SetInsertPoint(mergeBlock);
			PHINode* mergePHI = builder->CreatePHI(INTTYPE, phiCases, "unpackedInt");

			if (refValueBlock != nullptr)
			{
				builder->SetInsertPoint(refValueBlock);
				if (value.GetNomType()->IsSubtype(GetIntClassType(), false))
				{
					static const char* const errormsg = "Tried to unpack non-integer value as an Int!";
					BasicBlock* unboxErrorBlock = RTOutput_Fail::GenerateFailOutputBlock(builder, errormsg);
					BasicBlock* unboxValidBlock = BasicBlock::Create(LLVMCONTEXT, "intUnboxValid", fun);
					auto vtableptr = builder->CreatePtrToInt(ObjectHeader::GenerateReadVTablePointer(builder, value), numtype(intptr_t));
					auto isInt = builder->CreateICmpEQ(vtableptr, ConstantExpr::getPtrToInt(NomIntClass::GetInstance()->GetLLVMElement(*fun->getParent()), numtype(intptr_t)));
					builder->CreateCondBr(isInt, unboxValidBlock, unboxErrorBlock);

					builder->SetInsertPoint(unboxValidBlock);
					refValueBlock = unboxValidBlock;
				}
				auto boxField = builder->CreatePtrToInt(ObjectHeader::ReadField(builder, value, MakeInt32(0), false), INTTYPE, "unboxedInt");
				mergePHI->addIncoming(boxField, refValueBlock);
				builder->CreateBr(mergeBlock);
			}
			if (posMaskedIntBlock != nullptr)
			{
				builder->SetInsertPoint(posMaskedIntBlock);
				auto posFsh = UnpackPosMaskedInt(builder, refAsInt);
				mergePHI->addIncoming(posFsh, posMaskedIntBlock);
				builder->CreateBr(mergeBlock);
			}
			if (negMaskedIntBlock != nullptr)
			{
				builder->SetInsertPoint(negMaskedIntBlock);
				auto negFsh = UnpackNegMaskedInt(builder, refAsInt);
				mergePHI->addIncoming(negFsh, negMaskedIntBlock);
				builder->CreateBr(mergeBlock);
			}
			if (floatBlock != nullptr)
			{
				RTOutput_Fail::MakeBlockFailOutputBlock(builder, "Tried to unpack packed Float value as an Int!", floatBlock);
			}
			if (primitiveIntBlock != nullptr)
			{
				builder->SetInsertPoint(primitiveIntBlock);
				mergePHI->addIncoming(primitiveIntVal, primitiveIntBlock);
				builder->CreateBr(mergeBlock);
			}

			builder->SetInsertPoint(mergeBlock);
			return mergePHI;
		}

		NomValue UnpackPosMaskedInt(NomBuilder& builder, llvm::Value* refAsInt)
		{
			auto intMask = GetMask(64, 0, 2);
			auto prefunnel = builder->CreateAnd(intMask, refAsInt);
			return builder->CreateIntrinsic(Intrinsic::fshr, { prefunnel->getType() }, { prefunnel, prefunnel, ConstantInt::get(prefunnel->getType(), 3) });
		}
		NomValue UnpackNegMaskedInt(NomBuilder& builder, llvm::Value* refAsInt)
		{
			return builder->CreateIntrinsic(Intrinsic::fshr, { refAsInt->getType() }, { refAsInt, refAsInt, ConstantInt::get(refAsInt->getType(), 3) });
		}

		llvm::Value* UnpackInt(NomBuilder& builder, llvm::Value* refval, bool verify)
		{
			if (!refval->getType()->isPointerTy())
			{
				throw new std::exception();
			}
			if (NomCastStats)
			{
				builder->CreateCall(GetIncIntUnpacksFunction(*builder->GetInsertBlock()->getParent()->getParent()), {});
			}
			BasicBlock* incomingBlock = builder->GetInsertBlock();
			auto fun = incomingBlock->getParent();
			BasicBlock* floatBlock = BasicBlock::Create(LLVMCONTEXT, "floatBlock", fun);
			BasicBlock* posMaskedIntBlock = BasicBlock::Create(LLVMCONTEXT, "posMaskedIntBlock", fun);
			BasicBlock* negMaskedIntBlock = BasicBlock::Create(LLVMCONTEXT, "negMaskedIntBlock", fun);
			BasicBlock* boxedIntBlock = BasicBlock::Create(LLVMCONTEXT, "boxedIntBlock", fun);
			BasicBlock* outBlock = BasicBlock::Create(LLVMCONTEXT, "packedIntBlock", fun);

			auto refAsInt = builder->CreatePtrToInt(refval, INTTYPE);
			auto kindSwitch = builder->CreateSwitch(builder->CreateTrunc(refAsInt, inttype(3), "tag"), floatBlock);
			kindSwitch->addCase(MakeUInt(3, 0), boxedIntBlock);
			kindSwitch->addCase(MakeUInt(3, 3), posMaskedIntBlock);
			kindSwitch->addCase(MakeUInt(3, 7), negMaskedIntBlock);

			builder->SetInsertPoint(posMaskedIntBlock);
			auto intMask = GetMask(64, 0, 2);
			auto prefunnel = builder->CreateAnd(intMask, refAsInt);
			auto posFsh = builder->CreateIntrinsic(Intrinsic::fshr, { prefunnel->getType() }, { prefunnel, prefunnel, ConstantInt::get(prefunnel->getType(), 3) });
			builder->CreateBr(outBlock);

			builder->SetInsertPoint(negMaskedIntBlock);
			auto negFsh = builder->CreateIntrinsic(Intrinsic::fshr, { refAsInt->getType() }, { refAsInt, refAsInt, ConstantInt::get(refAsInt->getType(), 3) });
			builder->CreateBr(outBlock);

			builder->SetInsertPoint(boxedIntBlock);
			if (NomCastStats)
			{
				builder->CreateCall(GetIncIntUnboxesFunction(*builder->GetInsertBlock()->getParent()->getParent()), {});
			}
			if (verify)
			{
				BasicBlock* unboxErrorBlock = BasicBlock::Create(LLVMCONTEXT, "unboxError", fun);
				BasicBlock* unboxValidBlock = BasicBlock::Create(LLVMCONTEXT, "unboxValid", fun);
				auto vtableptr = builder->CreatePtrToInt(ObjectHeader::GenerateReadVTablePointer(builder, refval), numtype(intptr_t));
				auto isInt = builder->CreateICmpEQ(vtableptr, ConstantExpr::getPtrToInt(NomIntClass::GetInstance()->GetLLVMElement(*fun->getParent()), numtype(intptr_t)));
				builder->CreateCondBr(isInt, unboxValidBlock, unboxErrorBlock);
				
				builder->SetInsertPoint(unboxErrorBlock);
				static const char* nonint_errorMessage = "Tried to unpack non-integer value as int!";
				builder->CreateCall(RTOutput_Fail::GetLLVMElement(*fun->getParent()), GetLLVMPointer(nonint_errorMessage))->setCallingConv(RTOutput_Fail::GetLLVMElement(*fun->getParent())->getCallingConv());
				CreateDummyReturn(builder, fun);

				builder->SetInsertPoint(unboxValidBlock);
				boxedIntBlock = unboxValidBlock;
			}
			auto boxField = builder->CreatePtrToInt(ObjectHeader::ReadField(builder, refval, MakeInt32(0), false), INTTYPE, "unboxedInt");
			builder->CreateBr(outBlock);

			builder->SetInsertPoint(floatBlock);
			static const char* float_errorMessage = "Tried to unpack float as int!";
			builder->CreateCall(RTOutput_Fail::GetLLVMElement(*fun->getParent()), GetLLVMPointer(float_errorMessage))->setCallingConv(RTOutput_Fail::GetLLVMElement(*fun->getParent())->getCallingConv());
			CreateDummyReturn(builder, fun);

			builder->SetInsertPoint(outBlock);
			auto intPHI = builder->CreatePHI(INTTYPE, 3, "unpackedIntA");
			intPHI->addIncoming(posFsh, posMaskedIntBlock);
			intPHI->addIncoming(negFsh, negMaskedIntBlock);
			intPHI->addIncoming(boxField, boxedIntBlock);
			return intPHI;
			/*llvm::Module* mod = builder->GetInsertBlock()->getParent()->getParent();
			llvm::Function * unpackFun = Function::Create()
			builder->CreateTrunc(refval, inttype(2));
			builder->CreateCondBr()*/
			//return builder->CreateAShr(builder->CreatePtrToInt(refval, INTTYPE), 2);
		}
		llvm::Value* PackFloat(NomBuilder& builder, llvm::Value* floatval)
		{
			if (NomCastStats)
			{
				builder->CreateCall(GetIncFloatPacksFunction(*builder->GetInsertBlock()->getParent()->getParent()), {});
			}
			if (floatval->getType() != FLOATTYPE)
			{
				throw new std::exception();
			}
			BasicBlock* incomingBlock = builder->GetInsertBlock();
			auto fun = incomingBlock->getParent();
#ifdef FLOATDEBUG
			Module &mod = *fun->getParent();

			Function* packFloatDbgFun = mod.getFunction("RT_NOM_DBG_PrintPackFloatDebug");
			if (packFloatDbgFun == nullptr)
			{
				packFloatDbgFun = Function::Create(FunctionType::get(Type::getVoidTy(LLVMCONTEXT), { FLOATTYPE, INTTYPE }, false), GlobalValue::LinkageTypes::ExternalLinkage, "RT_NOM_DBG_PrintPackFloatDebug", mod);
			}
			Function* boxFloatDbgFun = mod.getFunction("RT_NOM_DBG_PrintBoxFloatDebug");
			if (boxFloatDbgFun == nullptr)
			{
				boxFloatDbgFun = Function::Create(FunctionType::get(Type::getVoidTy(LLVMCONTEXT), { FLOATTYPE, POINTERTYPE }, false), GlobalValue::LinkageTypes::ExternalLinkage, "RT_NOM_DBG_PrintBoxFloatDebug", mod);
			}
#endif
			BasicBlock* normalFloatBlock = BasicBlock::Create(LLVMCONTEXT, "normalFloatBlock", fun);
			BasicBlock* normalFloatBlock1 = BasicBlock::Create(LLVMCONTEXT, "normalFloatBlock1", fun);
			BasicBlock* zeroFloatBlock = BasicBlock::Create(LLVMCONTEXT, "zeroFloatBlock", fun);
			//BasicBlock* zeroCollisionFloatBlock = BasicBlock::Create(LLVMCONTEXT, "zeroCollisionFloatBlock", fun);
			//BasicBlock* maskFloatBlock = BasicBlock::Create(LLVMCONTEXT, "maskFloatBlock", fun);
			BasicBlock* boxFloatBlock = BasicBlock::Create(LLVMCONTEXT, "boxFloatBlock", fun);
			BasicBlock* outBlock = BasicBlock::Create(LLVMCONTEXT, "packedFloatBlock", fun);
			auto floatAsInt = builder->CreateBitCast(floatval, INTTYPE);
			auto fshresult = builder->CreateIntrinsic(Intrinsic::fshl, { floatAsInt->getType() }, { floatAsInt, floatAsInt, ConstantInt::get(floatAsInt->getType(), 3) });

			auto specialSwitch = builder->CreateSwitch(fshresult, normalFloatBlock, 2);
			specialSwitch->addCase(MakeUInt(64, 0), zeroFloatBlock);
			specialSwitch->addCase(MakeUInt(64, 4), zeroFloatBlock);
			specialSwitch->addCase(MakeUInt(64, 1), boxFloatBlock);
			specialSwitch->addCase(MakeUInt(64, 5), boxFloatBlock);

			builder->SetInsertPoint(normalFloatBlock);
			auto floatSwitch = builder->CreateSwitch(builder->CreateTrunc(fshresult, inttype(2)), boxFloatBlock);
			floatSwitch->addCase(ConstantInt::get(inttype(2), 1), normalFloatBlock1);
			floatSwitch->addCase(ConstantInt::get(inttype(2), 2), normalFloatBlock1);

			builder->SetInsertPoint(normalFloatBlock1);
			auto fshresultAsPtr = builder->CreateIntToPtr(fshresult, REFTYPE, "shiftedFloatRef");
#ifdef FLOATDEBUG
			builder->CreateCall(packFloatDbgFun, {floatval, fshresult });
#endif
			builder->CreateBr(outBlock);

			builder->SetInsertPoint(zeroFloatBlock);
			auto maskedFloatVal = builder->CreateOr(fshresult, MakeUInt(64, 1));
			auto zeroFloat = builder->CreateIntToPtr(maskedFloatVal, REFTYPE, "maskedZeroFloat");
#ifdef FLOATDEBUG
			builder->CreateCall(packFloatDbgFun, { floatval, maskedFloatVal });
#endif
			builder->CreateBr(outBlock);

			//builder->SetInsertPoint(maskFloatBlock);
			//auto maskedInt = builder->CreateOr(fshresult, ConstantInt::get(fshresult->getType(), 3));
			//auto packedInt = builder->CreateIntToPtr(maskedInt, REFTYPE);
			//builder->CreateBr(outBlock);

			builder->SetInsertPoint(boxFloatBlock);
			if (NomCastStats)
			{
				builder->CreateCall(GetIncFloatBoxesFunction(*builder->GetInsertBlock()->getParent()->getParent()), {});
			}
			auto allocfun = GetNewAlloc(fun->getParent());
			auto boxedFloat = builder->CreateCall(allocfun, { MakeInt<size_t>(1), MakeInt<size_t>(0) }, "boxedFloat");
			RefValueHeader::GenerateWriteVTablePointer(builder, boxedFloat, NomFloatClass::GetInstance()->GetLLVMElement(*fun->getParent()));
			ObjectHeader::WriteField(builder, boxedFloat, MakeInt32(0), builder->CreateIntToPtr(builder->CreateBitCast(floatval, INTTYPE), REFTYPE), false);
#ifdef FLOATDEBUG
			builder->CreateCall(boxFloatDbgFun, { floatval, builder->CreatePointerCast(boxedFloat, POINTERTYPE) });
#endif
			boxFloatBlock = builder->GetInsertBlock();
			builder->CreateBr(outBlock);

			builder->SetInsertPoint(outBlock);
			auto floatPHI = builder->CreatePHI(REFTYPE, 3, "packedFloat");
			floatPHI->addIncoming(zeroFloat, zeroFloatBlock);
			floatPHI->addIncoming(fshresultAsPtr, normalFloatBlock1);
			floatPHI->addIncoming(boxedFloat, boxFloatBlock);
			return floatPHI;
			/*llvm::Value* floatmask = (llvm::ConstantInt::get(INTTYPE, 2, false));
			return builder->CreateIntToPtr(builder->CreateOr(builder->CreateBitCast(floatval, INTTYPE), floatmask), REFTYPE);*/
		}
		llvm::Value* UnpackFloat(NomBuilder& builder, llvm::Value* refval, bool verify)
		{
			if (NomCastStats)
			{
				builder->CreateCall(GetIncFloatUnpacksFunction(*builder->GetInsertBlock()->getParent()->getParent()), {});
			}
			BasicBlock* incomingBlock = builder->GetInsertBlock();
			auto fun = incomingBlock->getParent();
#ifdef FLOATDEBUG
			Module& mod = *fun->getParent();

			Function* unpackFloatDbgFun = mod.getFunction("RT_NOM_DBG_PrintUnpackFloatDebug");
			if (unpackFloatDbgFun == nullptr)
			{
				unpackFloatDbgFun = Function::Create(FunctionType::get(Type::getVoidTy(LLVMCONTEXT), { FLOATTYPE, INTTYPE }, false), GlobalValue::LinkageTypes::ExternalLinkage, "RT_NOM_DBG_PrintUnpackFloatDebug", mod);
			}
			Function* unboxFloatDbgFun = mod.getFunction("RT_NOM_DBG_PrintUnboxFloatDebug");
			if (unboxFloatDbgFun == nullptr)
			{
				unboxFloatDbgFun = Function::Create(FunctionType::get(Type::getVoidTy(LLVMCONTEXT), { FLOATTYPE, POINTERTYPE }, false), GlobalValue::LinkageTypes::ExternalLinkage, "RT_NOM_DBG_PrintUnboxFloatDebug", mod);
			}
#endif
			BasicBlock* intBlock = BasicBlock::Create(LLVMCONTEXT, "intBlock", fun);
			BasicBlock* normalValueBlock = BasicBlock::Create(LLVMCONTEXT, "normalValueBlock", fun);
			BasicBlock* posZeroBlock = BasicBlock::Create(LLVMCONTEXT, "posZeroBlock", fun);
			BasicBlock* negZeroBlock = BasicBlock::Create(LLVMCONTEXT, "negZeroBlock", fun);
			BasicBlock* maskedFloatBlock = BasicBlock::Create(LLVMCONTEXT, "maskedFloatBlock", fun);
			BasicBlock* boxedFloatBlock = BasicBlock::Create(LLVMCONTEXT, "boxedFloatBlock", fun);
			BasicBlock* outBlock = BasicBlock::Create(LLVMCONTEXT, "packedIntBlock", fun);

			auto refAsInt = builder->CreatePtrToInt(refval, INTTYPE);

			auto checkZeroSwitch = builder->CreateSwitch(refAsInt, normalValueBlock, 2);
			checkZeroSwitch->addCase(MakeUInt(64, 1), posZeroBlock);
			checkZeroSwitch->addCase(MakeUInt(64, 5), negZeroBlock);

			builder->SetInsertPoint(normalValueBlock);
			auto kindSwitch = builder->CreateSwitch(builder->CreateTrunc(refAsInt, inttype(3), "tag"), intBlock);
			kindSwitch->addCase(MakeUInt(3, 0), boxedFloatBlock);
			kindSwitch->addCase(MakeUInt(3, 1), maskedFloatBlock);
			kindSwitch->addCase(MakeUInt(3, 2), maskedFloatBlock);
			kindSwitch->addCase(MakeUInt(3, 5), maskedFloatBlock);
			kindSwitch->addCase(MakeUInt(3, 6), maskedFloatBlock);

			builder->SetInsertPoint(posZeroBlock);
#ifdef FLOATDEBUG
			builder->CreateCall(unpackFloatDbgFun, { ConstantFP::get(FLOATTYPE, 0.0), refAsInt });
#endif
			builder->CreateBr(outBlock);

			builder->SetInsertPoint(negZeroBlock);
#ifdef FLOATDEBUG
			builder->CreateCall(unpackFloatDbgFun, { ConstantFP::getNegativeZero(FLOATTYPE), refAsInt });
#endif
			builder->CreateBr(outBlock);

			builder->SetInsertPoint(maskedFloatBlock);
			auto unmaskedFloat = builder->CreateBitCast(builder->CreateIntrinsic(Intrinsic::fshr, { INTTYPE }, { refAsInt, refAsInt, ConstantInt::get(refAsInt->getType(), 3) }), FLOATTYPE, "unmaskedFloat");
#ifdef FLOATDEBUG
			builder->CreateCall(unpackFloatDbgFun, { unmaskedFloat, refAsInt });
#endif
			builder->CreateBr(outBlock);

			builder->SetInsertPoint(boxedFloatBlock);
			if (NomCastStats)
			{
				builder->CreateCall(GetIncFloatUnboxesFunction(*builder->GetInsertBlock()->getParent()->getParent()), {});
			}
			auto boxField = builder->CreateBitCast(builder->CreatePtrToInt(ObjectHeader::ReadField(builder, refval, MakeInt32(0), false), INTTYPE), FLOATTYPE, "unboxedFloat");
#ifdef FLOATDEBUG
			builder->CreateCall(unboxFloatDbgFun, { boxField, builder->CreatePointerCast(refval, POINTERTYPE) });
#endif
			builder->CreateBr(outBlock);

			builder->SetInsertPoint(intBlock);
			static const char* int_errorMessage = "Tried to unpack int as float!";
			builder->CreateCall(RTOutput_Fail::GetLLVMElement(*fun->getParent()), GetLLVMPointer(int_errorMessage))->setCallingConv(RTOutput_Fail::GetLLVMElement(*fun->getParent())->getCallingConv());
			CreateDummyReturn(builder, fun);

			builder->SetInsertPoint(outBlock);
			auto floatPHI = builder->CreatePHI(FLOATTYPE, 3, "unpackedFloat");
			floatPHI->addIncoming(boxField, boxedFloatBlock);
			floatPHI->addIncoming(unmaskedFloat, maskedFloatBlock);
			floatPHI->addIncoming(ConstantFP::get(FLOATTYPE, 0.0), posZeroBlock);
			floatPHI->addIncoming(ConstantFP::getNegativeZero(FLOATTYPE), negZeroBlock);
			return floatPHI;
			/*llvm::Value* floatmask = builder->CreateXor(llvm::ConstantInt::get(INTTYPE, 2, false), llvm::ConstantInt::getAllOnesValue(INTTYPE));
			return builder->CreateBitCast(builder->CreateAnd(builder->CreatePtrToInt(refval, INTTYPE), floatmask), FLOATTYPE);*/
		}

		NomValue UnpackMaskedFloat(NomBuilder& builder, llvm::Value* refAsInt)
		{
			return builder->CreateBitCast(builder->CreateIntrinsic(Intrinsic::fshr, { INTTYPE }, { refAsInt, refAsInt, ConstantInt::get(refAsInt->getType(), 3) }), FLOATTYPE, "unmaskedFloat");
		}

		llvm::Value* PackBool(NomBuilder& builder, llvm::Value* b)
		{
			return NomBoolObjects::PackBool(builder, b);
		}

		llvm::Value* UnpackBool(NomBuilder& builder, llvm::Value* b, bool verify)
		{
			return NomBoolObjects::UnpackBool(builder, b);
		}
	}
}

#ifdef FLOATDEBUG
extern "C" DLLEXPORT void RT_NOM_DBG_PrintUnpackFloatDebug(double f, int64_t i)
{
	cout << "Unpacking " << i << " as float: " << f << "\n";
}

extern "C" DLLEXPORT void RT_NOM_DBG_PrintUnboxFloatDebug(double f, void* i)
{
	cout << "Unboxing " << std::hex << (intptr_t)i << std::dec << " as float: " << f << "\n";
}

extern "C" DLLEXPORT void RT_NOM_DBG_PrintPackFloatDebug(double f, int64_t i)
{
	cout << "Packing float " << f << " as: " << i << "\n";
}

extern "C" DLLEXPORT void RT_NOM_DBG_PrintBoxFloatDebug(double f, void* i)
{
	cout << "Boxing float " << f << " in: " << std::hex << (intptr_t)i << std::dec << "\n";
}
#endif