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
#include "Metadata.h"
#include "CompileEnv.h"
#include "PWRefValue.h"
#include "PWObject.h"

using namespace llvm;
using namespace std;
namespace Nom
{
	namespace Runtime
	{

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
			if (val->getType()->isIntegerTy(static_cast<unsigned int>(INTTYPE->getPrimitiveSizeInBits())))
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
			if (val->getType()->isIntegerTy(static_cast<unsigned int>(INTTYPE->getPrimitiveSizeInBits())))
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
				if (expected->isIntegerTy(static_cast<unsigned int>(INTTYPE->getPrimitiveSizeInBits())))
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
		// Note: this depends on POINTERTYPE and REFTYPE being different, to either just encode/decode raw values for IMTs or do actual packing/unpacking
		llvm::Value* EnsurePackedUnpacked(NomBuilder& builder, llvm::Value* val, llvm::Type* type)
		{
			auto valType = val->getType();
			if (valType == type)
			{
				return val;
			}
			if (valType == POINTERTYPE)
			{
				if (type == INTTYPE)
				{
					return builder->CreatePtrToInt(val, INTTYPE);
				}
				else if (type == FLOATTYPE)
				{
					return builder->CreateBitCast(builder->CreatePtrToInt(val, INTTYPE), FLOATTYPE);
				}
				else if (type == BOOLTYPE)
				{
					return builder->CreatePtrToInt(val, BOOLTYPE);
				}
				else if (type == TYPETYPE || type==REFTYPE)
				{
					return builder->CreatePointerCast(val, type);
				}
				else
				{
					throw new std::exception();
				}
			}
			else if(type==POINTERTYPE)
			{
				return WrapAsPointer(builder, val);
			}
			else
			{
				if (valType == REFTYPE && type == INTTYPE)
				{
					return UnpackInt(builder, NomValue(val, NomIntClass::GetInstance()->GetType()));
				}
				else if (valType == REFTYPE && type == FLOATTYPE)
				{
					return UnpackFloat(builder, NomValue(val, NomFloatClass::GetInstance()->GetType()));
				}
				else if (valType == REFTYPE && type == BOOLTYPE)
				{
					return UnpackFloat(builder, NomValue(val, NomBoolClass::GetInstance()->GetType()));
				}
				else if (valType == INTTYPE && type == REFTYPE)
				{
					return PackInt(builder, val);
				}
				else if (valType == FLOATTYPE && type == REFTYPE)
				{
					return PackFloat(builder, val);
				}
				else if (valType == BOOLTYPE && type == REFTYPE)
				{
					return PackBool(builder, val);
				}
				else
				{
					throw new std::exception();
				}
			}
		}

		/// <summary>
		/// Integer Packing Protocol:
		/// Step 1: Left shift by 2 bits, then arithmetic right shift by 2. 
		/// Step 2: Check whether the result equals the original value, which means the top three bits are all equal, i.e. 000 or 111.
		///   - if so, the packed int is the result of the left shift by 2 plus an or with 3 (to marke the to LSBs as 11).
		///   - for all other combinations, the integer is boxed into a newly allocated reference value
		/// Thus, all small enough (|n| ~ < 2^61) are masked, while bigger absolute values are boxed 
		/// </summary>
		/// <param name="builder"></param>
		/// <param name="intval"></param>
		/// <returns></returns>
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

			auto shlresult = builder->CreateShl(intval, MakeIntLike(intval, 2), "leftShiftedValue");
			auto shrresult = builder->CreateAShr(shlresult, MakeIntLike(shlresult, 2), "rightShiftedValue");
			auto shiftseq = builder->CreateICmpEQ(shrresult, intval, "shiftedValuesEq");
			CreateExpect(builder, shiftseq, MakeUInt(1, 1));
			builder->CreateCondBr(shiftseq, maskedIntBlock, boxedIntBlock, GetLikelyFirstBranchMetadata());

			builder->SetInsertPoint(maskedIntBlock);
			auto maskedInt = builder->CreateOr(shlresult, MakeIntLike(shlresult,3));
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
			if (NomCastStats)
			{
				builder->CreateCall(GetIncIntUnpacksFunction(*builder->GetInsertBlock()->getParent()->getParent()), {});
			}
			Function* fun = builder->GetInsertBlock()->getParent();
			BasicBlock* refValueBlock = nullptr, * maskedIntBlock = nullptr, * floatBlock = nullptr, * primitiveIntBlock = nullptr;
			Value* primitiveIntVal = nullptr;
			RefValueHeader::GenerateRefOrPrimitiveValueSwitch(builder, value, &refValueBlock, &maskedIntBlock, &floatBlock, false, &primitiveIntBlock, &primitiveIntVal, nullptr, nullptr, nullptr, nullptr, 30, 100, 1, 1);

			unsigned int phiCases = (refValueBlock != nullptr ? 1 : 0) + (maskedIntBlock != nullptr ? 1 : 0) + (primitiveIntBlock != nullptr ? 1 : 0);

			BasicBlock* mergeBlock = BasicBlock::Create(LLVMCONTEXT, "intUnpackMerge", fun);
			builder->SetInsertPoint(mergeBlock);
			PHINode* mergePHI = builder->CreatePHI(INTTYPE, phiCases, "unpackedInt");

			if (refValueBlock != nullptr)
			{
				builder->SetInsertPoint(refValueBlock);
				if (!value.GetNomType()->IsSubtype(GetIntClassType(), false))
				{
					static const char* const errormsg = "Tried to unpack non-integer value as an Int!";
					BasicBlock* unboxErrorBlock = RTOutput_Fail::GenerateFailOutputBlock(builder, errormsg);
					BasicBlock* unboxValidBlock = BasicBlock::Create(LLVMCONTEXT, "intUnboxValid", fun);
					auto vtableptr = builder->CreatePtrToInt(ObjectHeader::GenerateReadVTablePointer(builder, value), numtype(intptr_t));
					auto isInt = builder->CreateICmpEQ(vtableptr, ConstantExpr::getPtrToInt(NomIntClass::GetInstance()->GetLLVMElement(*fun->getParent()), numtype(intptr_t)));
					builder->CreateCondBr(isInt, unboxValidBlock, unboxErrorBlock, GetLikelyFirstBranchMetadata());

					builder->SetInsertPoint(unboxValidBlock);
					refValueBlock = unboxValidBlock;
				}
				if (NomCastStats)
				{
					builder->CreateCall(GetIncIntUnboxesFunction(*builder->GetInsertBlock()->getParent()->getParent()), {});
				}
				auto boxField = builder->CreatePtrToInt(PWObject(value).ReadField(builder, MakeInt32(0), false), INTTYPE, "unboxedInt");
				mergePHI->addIncoming(boxField, refValueBlock);
				builder->CreateBr(mergeBlock);
			}
			if (maskedIntBlock != nullptr)
			{
				builder->SetInsertPoint(maskedIntBlock);
				auto refAsInt = builder->CreatePtrToInt(value, INTTYPE);
				auto unpackedInt = UnpackMaskedInt(builder, refAsInt);
				mergePHI->addIncoming(unpackedInt, builder->GetInsertBlock());
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

		NomValue UnpackMaskedInt(NomBuilder& builder, llvm::Value* refAsInt)
		{
			if (refAsInt->getType() == REFTYPE)
			{
				refAsInt = builder->CreatePtrToInt(refAsInt, INTTYPE);
			}
			return builder->CreateAShr(refAsInt, MakeIntLike(refAsInt, 2), "unpackedInt");
		}

		/// <summary>
		/// If MSBs 2 and 3 are 10 or 01, float is packed by left funnel shift 3.
		/// Otherwise, if float is positive or negative zero, we return a reference to the default objects for those values.
		/// Otherwise, float is boxed.
		/// </summary>
		/// <param name="builder"></param>
		/// <param name="floatval"></param>
		/// <returns></returns>
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
			BasicBlock* checkPosZeroBlock = BasicBlock::Create(LLVMCONTEXT, "checkPosZeroFloat", fun);
			BasicBlock* checkNegZeroFloatBlock = BasicBlock::Create(LLVMCONTEXT, "checkNegZeroFloat", fun);
			BasicBlock* boxFloatBlock = BasicBlock::Create(LLVMCONTEXT, "boxFloat", fun);
			BasicBlock* outBlock = BasicBlock::Create(LLVMCONTEXT, "packFloatOut", fun);
			auto floatAsInt = builder->CreateBitCast(floatval, INTTYPE);
			auto posExpTag = 0x4000000000000000ULL;
			auto negExpTag = 0x2000000000000000ULL;
			auto exponentMask = 0x6000000000000000ULL;
			auto isPositiveGoodExponent = builder->CreateICmpEQ(builder->CreateAnd(floatAsInt, MakeIntLike(floatAsInt, exponentMask)), MakeIntLike(floatAsInt, posExpTag));
			auto isNegativeGoodExponent = builder->CreateICmpEQ(builder->CreateAnd(floatAsInt, MakeIntLike(floatAsInt, exponentMask)), MakeIntLike(floatAsInt, negExpTag));
			auto isGoodExponent = builder->CreateOr(isPositiveGoodExponent, isNegativeGoodExponent);
			auto funnelShiftedFloat = builder->CreateIntToPtr(builder->CreateIntrinsic(Intrinsic::fshl, { floatAsInt->getType() }, { floatAsInt, floatAsInt, ConstantInt::get(floatAsInt->getType(), 3) }), REFTYPE, "maskedFloat");
			CreateExpect(builder, isGoodExponent, MakeIntLike(isGoodExponent, 1));
			builder->CreateCondBr(isGoodExponent, outBlock, checkPosZeroBlock, GetLikelyFirstBranchMetadata());

			builder->SetInsertPoint(checkPosZeroBlock);
			auto isPosZero = builder->CreateFCmpOEQ(floatval, ConstantFP::get(FLOATTYPE, 0.0));
			CreateExpect(builder, isPosZero, MakeIntLike(isPosZero, 1));
			builder->CreateCondBr(isPosZero, outBlock, checkNegZeroFloatBlock, GetLikelyFirstBranchMetadata());

			builder->SetInsertPoint(checkNegZeroFloatBlock);
			auto isNegZero = builder->CreateFCmpOEQ(floatval, ConstantFP::getNegativeZero(FLOATTYPE));
			CreateExpect(builder, isNegZero, MakeIntLike(isNegZero, 1));
			builder->CreateCondBr(isNegZero, outBlock, boxFloatBlock, GetLikelyFirstBranchMetadata());

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
			auto floatPHI = builder->CreatePHI(REFTYPE, 4, "packedFloat");
			floatPHI->addIncoming(funnelShiftedFloat, incomingBlock);
			floatPHI->addIncoming(NomFloatObjects::GetPosZero(*fun->getParent()), checkPosZeroBlock);
			floatPHI->addIncoming(NomFloatObjects::GetNegZero(*fun->getParent()), checkNegZeroFloatBlock);
			floatPHI->addIncoming(boxedFloat, boxFloatBlock);
			return floatPHI;
		}

		llvm::Value* UnpackFloat(NomBuilder& builder, NomValue value)
		{
			if (NomCastStats)
			{
				builder->CreateCall(GetIncFloatUnpacksFunction(*builder->GetInsertBlock()->getParent()->getParent()), {});
			}
			Function* fun = builder->GetInsertBlock()->getParent();
			BasicBlock* refValueBlock = nullptr, *maskedFloatBlock=nullptr, * intBlock = nullptr, * primitiveFloatBlock = nullptr;
			Value* primitiveFloatVal = nullptr;

			unsigned int phiCases = RefValueHeader::GenerateRefOrPrimitiveValueSwitch(builder, value, &refValueBlock, &intBlock, &maskedFloatBlock, false, nullptr, nullptr, &primitiveFloatBlock, &primitiveFloatVal, nullptr, nullptr, 30, 1, 100, 1);

			BasicBlock* mergeBlock = BasicBlock::Create(LLVMCONTEXT, "floatUnpackMerge", fun);
			builder->SetInsertPoint(mergeBlock);
			PHINode* mergePHI = builder->CreatePHI(FLOATTYPE, phiCases, "unpackedFloat");

			if (refValueBlock != nullptr)
			{
				builder->SetInsertPoint(refValueBlock);
				if (!value.GetNomType()->IsSubtype(GetFloatClassType(), false))
				{
					static const char* const errormsg = "Tried to unpack non-float value as a Float!";
					BasicBlock* unboxErrorBlock = RTOutput_Fail::GenerateFailOutputBlock(builder, errormsg);
					BasicBlock* unboxValidBlock = BasicBlock::Create(LLVMCONTEXT, "floatUnboxValid", fun);
					auto vtableptr = builder->CreatePtrToInt(ObjectHeader::GenerateReadVTablePointer(builder, value), numtype(intptr_t));
					auto isFloat = builder->CreateICmpEQ(vtableptr, ConstantExpr::getPtrToInt(NomFloatClass::GetInstance()->GetLLVMElement(*fun->getParent()), numtype(intptr_t)));
					builder->CreateCondBr(isFloat, unboxValidBlock, unboxErrorBlock, GetLikelyFirstBranchMetadata());

					builder->SetInsertPoint(unboxValidBlock);
					refValueBlock = unboxValidBlock;
				}
				if (NomCastStats)
				{
					builder->CreateCall(GetIncFloatUnboxesFunction(*builder->GetInsertBlock()->getParent()->getParent()), {});
				}
				auto boxField = builder->CreateBitCast(builder->CreatePtrToInt(PWObject(value).ReadField(builder, MakeInt32(0), false), INTTYPE), FLOATTYPE, "unboxedFloat");
				mergePHI->addIncoming(boxField, refValueBlock);
				builder->CreateBr(mergeBlock);
			}
			if (maskedFloatBlock != nullptr)
			{
				builder->SetInsertPoint(maskedFloatBlock);
				auto refAsInt = builder->CreatePtrToInt(value, INTTYPE);
				auto unmaskedFloat = UnpackMaskedFloat(builder, refAsInt);
				mergePHI->addIncoming(unmaskedFloat, maskedFloatBlock);
				builder->CreateBr(mergeBlock);
			}
			if (intBlock != nullptr)
			{
				RTOutput_Fail::MakeBlockFailOutputBlock(builder, "Tried to unpack packed Int value as a Float!", intBlock);
			}
			if (primitiveFloatBlock != nullptr)
			{
				builder->SetInsertPoint(primitiveFloatBlock);
				mergePHI->addIncoming(primitiveFloatVal, primitiveFloatBlock);
				builder->CreateBr(mergeBlock);
			}

			builder->SetInsertPoint(mergeBlock);
			return mergePHI;
		}
		NomValue UnpackMaskedFloat(NomBuilder& builder, llvm::Value* refAsInt)
		{
			if (refAsInt->getType() == REFTYPE)
			{
				refAsInt = builder->CreatePtrToInt(refAsInt, INTTYPE);
			}
			return builder->CreateBitCast(builder->CreateIntrinsic(Intrinsic::fshr, { INTTYPE }, { refAsInt, refAsInt, ConstantInt::get(refAsInt->getType(), 3) }), FLOATTYPE, "unmaskedFloat");
		}

		llvm::Value* PackBool(NomBuilder& builder, llvm::Value* b)
		{
			return NomBoolObjects::PackBool(builder, b);
		}

		llvm::Value* UnpackBool(NomBuilder& builder, llvm::Value* b, [[maybe_unused]] bool verify)
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
