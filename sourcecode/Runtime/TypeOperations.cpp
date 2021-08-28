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

		/// <summary>
		/// Integer Packing Protocol:
		/// Step 1: Left Funnel Shift by 3 bits, i.e. the three most significant bits become the three least significant bits, and everything else is shifted left by 3.
		///         These most significant bits are going to be replaced by the mask.
		/// Step 2: Check three least significant bits of funnel shift result (i.e. the three most significant bits of the actual number)
		///   - if 000, the mask becomes 011, i.e. a positive masked integer
		///   - if 111, the mask becomes 111, i.e. a negative masked integer
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
			if (NomCastStats)
			{
				builder->CreateCall(GetIncIntUnpacksFunction(*builder->GetInsertBlock()->getParent()->getParent()), {});
			}
			Function* fun = builder->GetInsertBlock()->getParent();
			BasicBlock* refValueBlock = nullptr, * posMaskedIntBlock = nullptr, *negMaskedIntBlock = nullptr, * floatBlock = nullptr, * primitiveIntBlock = nullptr;
			Value* primitiveIntVal = nullptr;
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
				if (NomCastStats)
				{
					builder->CreateCall(GetIncIntUnboxesFunction(*builder->GetInsertBlock()->getParent()->getParent()), {});
				}
				auto boxField = builder->CreatePtrToInt(ObjectHeader::ReadField(builder, value, MakeInt32(0), false), INTTYPE, "unboxedInt");
				mergePHI->addIncoming(boxField, refValueBlock);
				builder->CreateBr(mergeBlock);
			}
			if (posMaskedIntBlock != nullptr)
			{
				builder->SetInsertPoint(posMaskedIntBlock);
				auto refAsInt = builder->CreatePtrToInt(value, INTTYPE);
				auto posFsh = UnpackPosMaskedInt(builder, refAsInt);
				mergePHI->addIncoming(posFsh, posMaskedIntBlock);
				builder->CreateBr(mergeBlock);
			}
			if (negMaskedIntBlock != nullptr)
			{
				builder->SetInsertPoint(negMaskedIntBlock);
				auto refAsInt = builder->CreatePtrToInt(value, INTTYPE);
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

		/// <summary>
		/// Float Packing Protocol:
		/// The following float masks exist:
		/// - 001: either positive 0 if all other bits are 0, or some other floating point value
		/// - 101: either negative 0 if all other bits are 0, or some other floating point value
		/// - 010: a floating point value whose most significant bits happen to be 010
		/// - 110: a floating point value whose most significant bits happen to be 110
		/// Step 1: Left Funnel Shift by 3 bits, i.e. the three most significant bits become the three least significant bits, and everything else is shifted left by 3.
		///         These most significant bits are going to be replaced by the mask.
		/// Step 2: Check the whole funnel shift result (assuming 0s for any left-out positions to the left)
		///   - if 000, float is positive 0.0, mask with 001
		///   - if 100, float is negative 0.0, mask with 101
		///   - if 101 or 001, the value is an extremely small value (1.49...E-154) and LSBs would collide with above masks, so these values are boxed
		///   - otherwise, check the two least significant bits:
		///     - if 01 or 10, the exponent is is between -511 and +512 (out of its overall -1022 -- +1023 range), and the sign bit is either 0 or 1, thus covering a good mid-range of values.
		///       these values are left in the funnel shifted state, so the mask is just one of 001, 101, 010, or 110 (the mask collisions with the zero values don't matter because at least some other bit can distinguish them)
		///     - all other values (i.e. those with more extreme exponents) are boxed
		/// NOTE: together with the 000 mask for reference values and the 011/111 masks for integers, this leaves the mask 100, which is used in freezing reference values in dictionaries
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

		llvm::Value* UnpackFloat(NomBuilder& builder, NomValue value)
		{
			if (NomCastStats)
			{
				builder->CreateCall(GetIncFloatUnpacksFunction(*builder->GetInsertBlock()->getParent()->getParent()), {});
			}
			Function* fun = builder->GetInsertBlock()->getParent();
			BasicBlock* refValueBlock = nullptr, * posZeroFloatBlock = nullptr, * negZeroFloatBlock = nullptr, *maskedFloatBlock=nullptr, * intBlock = nullptr, * primitiveFloatBlock = nullptr;
			Value* primitiveFloatVal = nullptr;

			RefValueHeader::GenerateRefOrPrimitiveValueSwitch(builder, value, &refValueBlock, &intBlock, &intBlock, &posZeroFloatBlock, &negZeroFloatBlock, &maskedFloatBlock, false, nullptr, nullptr, &primitiveFloatBlock, &primitiveFloatVal, nullptr, nullptr);

			int phiCases = (refValueBlock != nullptr ? 1 : 0) + (posZeroFloatBlock != nullptr ? 1 : 0) + (negZeroFloatBlock != nullptr ? 1 : 0) + (maskedFloatBlock != nullptr ? 1 : 0);

			BasicBlock* mergeBlock = BasicBlock::Create(LLVMCONTEXT, "floatUnpackMerge", fun);
			builder->SetInsertPoint(mergeBlock);
			PHINode* mergePHI = builder->CreatePHI(FLOATTYPE, phiCases, "unpackedFloat");

			if (refValueBlock != nullptr)
			{
				builder->SetInsertPoint(refValueBlock);
				if (value.GetNomType()->IsSubtype(GetFloatClassType(), false))
				{
					static const char* const errormsg = "Tried to unpack non-float value as a Float!";
					BasicBlock* unboxErrorBlock = RTOutput_Fail::GenerateFailOutputBlock(builder, errormsg);
					BasicBlock* unboxValidBlock = BasicBlock::Create(LLVMCONTEXT, "floatUnboxValid", fun);
					auto vtableptr = builder->CreatePtrToInt(ObjectHeader::GenerateReadVTablePointer(builder, value), numtype(intptr_t));
					auto isFloat = builder->CreateICmpEQ(vtableptr, ConstantExpr::getPtrToInt(NomIntClass::GetInstance()->GetLLVMElement(*fun->getParent()), numtype(intptr_t)));
					builder->CreateCondBr(isFloat, unboxValidBlock, unboxErrorBlock);

					builder->SetInsertPoint(unboxValidBlock);
					refValueBlock = unboxValidBlock;
				}
				if (NomCastStats)
				{
					builder->CreateCall(GetIncFloatUnboxesFunction(*builder->GetInsertBlock()->getParent()->getParent()), {});
				}
				auto boxField = builder->CreateBitCast(builder->CreatePtrToInt(ObjectHeader::ReadField(builder, value, MakeInt32(0), false), INTTYPE), FLOATTYPE, "unboxedFloat");
				mergePHI->addIncoming(boxField, refValueBlock);
				builder->CreateBr(mergeBlock);
			}
			if (posZeroFloatBlock != nullptr)
			{
				builder->SetInsertPoint(posZeroFloatBlock);
				mergePHI->addIncoming(ConstantFP::get(FLOATTYPE, 0.0), posZeroFloatBlock);
				builder->CreateBr(mergeBlock);
			}
			if (negZeroFloatBlock != nullptr)
			{
				builder->SetInsertPoint(negZeroFloatBlock);
				mergePHI->addIncoming(ConstantFP::getNegativeZero(FLOATTYPE), negZeroFloatBlock);
				builder->CreateBr(mergeBlock);
			}
			if (maskedFloatBlock != nullptr)
			{
				builder->SetInsertPoint(maskedFloatBlock);
				auto refAsInt = builder->CreatePtrToInt(value, INTTYPE);
				auto unmaskedFloat = builder->CreateBitCast(builder->CreateIntrinsic(Intrinsic::fshr, { INTTYPE }, { refAsInt, refAsInt, ConstantInt::get(refAsInt->getType(), 3) }), FLOATTYPE, "unmaskedFloat");
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