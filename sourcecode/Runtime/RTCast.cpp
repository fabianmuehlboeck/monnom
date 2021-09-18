#include "RTCast.h"
#include "NomDynamicType.h"
#include "CompileHelpers.h"
#include "NomTopType.h"
#include "NomClassType.h"
#include "NomDynamicType.h"
#include "NomBottomType.h"
#include "BoolClass.h"
#include "IntClass.h"
#include "FloatClass.h"
#include "RefValueHeader.h"
#include "RTVTable.h"
#include "RTTypeEq.h"
#include "NomTypeVar.h"
#include "llvm/ADT/SmallVector.h"
#include "RTOutput.h"
#include "RTClassType.h"
#include "ObjectClass.h"
#include "RTConfig.h"
#include "CastStats.h"
#include "LambdaHeader.h"
#include "RTLambda.h"
#include "NomTypeRegistry.h"
#include "RTInterface.h"
#include "RTSubtyping.h"
#include "RTSignature.h"
#include "NomType.h"
#include "CallingConvConf.h"
#include "NullClass.h"
#include "NomMaybeType.h"
#include "RTInstanceType.h"
#include "RTMaybeType.h"
#include "RTCompileConfig.h"
#include "RTDictionary.h"
#include "StructuralValueHeader.h"
#include "Metadata.h"

using namespace llvm;
using namespace std;
namespace Nom
{
	namespace Runtime
	{
		llvm::FunctionType* RTCast::GetAdjustFunctionType()
		{
			static auto ft = FunctionType::get(BOOLTYPE, ArrayRef<Type*>({ REFTYPE, TYPETYPE }), false);
			return ft;
		}

		llvm::Value* GenerateEnvSubstitutions(NomBuilder& builder, CompileEnv* env, BasicBlock** outBlocks, int outBlockCount, NomTypeRef type = nullptr)
		{
			auto targcount = env->GetEnvTypeArgumentCount() + env->GetLocalTypeArgumentCount();
			if (targcount > 0 && (type == nullptr || type->ContainsVariables()))
			{
				if (env->GetLocalTypeArgumentCount() > 0)
				{
					auto targarr = builder->CreateAlloca(TYPETYPE, MakeInt(targcount), "targarr");
					RTSubstStackValue rtss = RTSubstStackValue(builder, builder->CreateGEP(targarr, MakeInt32(targcount)), nullptr, MakeInt32(targcount), targarr);
					for (int32_t i = 0; i < targcount; i++)
					{
						if (type == nullptr || type->ContainsVariableIndex(i))
						{
							MakeInvariantStore(builder, env->GetTypeArgument(builder, i), builder->CreateGEP(targarr, MakeInt32(targcount - (i + 1))));
						}
					}
					rtss.MakeTypeListInvariant(builder);
					BasicBlock* currentBlock = builder->GetInsertBlock();

					for (int i = 0; i < outBlockCount; i++)
					{
						BasicBlock* newOutBlock = BasicBlock::Create(LLVMCONTEXT, "outReleaseSubstStack", currentBlock->getParent());
						builder->SetInsertPoint(newOutBlock);
						rtss.MakeRelease(builder);
						builder->CreateBr(outBlocks[i]);

						outBlocks[i] = newOutBlock;
					}
					builder->SetInsertPoint(currentBlock);

					return rtss;
				}
				else
				{
					RTSubstStackValue rtss = RTSubstStackValue(builder, env->GetEnvTypeArgumentArray(builder));
					BasicBlock* currentBlock = builder->GetInsertBlock();

					for (int i = 0; i < outBlockCount; i++)
					{
						BasicBlock* newOutBlock = BasicBlock::Create(LLVMCONTEXT, "outReleaseSubstStack", currentBlock->getParent());
						builder->SetInsertPoint(newOutBlock);
						rtss.MakeRelease(builder);
						builder->CreateBr(outBlocks[i]);

						outBlocks[i] = newOutBlock;
					}
					builder->SetInsertPoint(currentBlock);

					return rtss;
				}
			}
			else
			{
				return ConstantPointerNull::get(RTSubtyping::TypeArgumentListStackType()->getPointerTo());
			}
		}

		llvm::Value* RTCast::GenerateMonotonicCast(NomBuilder& builder, CompileEnv* env, NomValue& value, NomClassTypeRef type)
		{
			BasicBlock* origBlock = builder->GetInsertBlock();
			Function* fun = origBlock->getParent();

			BasicBlock* outFalseBlock = RTOutput_Fail::GenerateFailOutputBlock(builder, "Cast failed!");

			BasicBlock* outBlock = BasicBlock::Create(LLVMCONTEXT, "MonotonicCastOut", fun);
			BasicBlock* outTrueBlock = outBlock;

			//PHINode* outPHI;
			{
				builder->SetInsertPoint(outBlock);
				//outPHI = builder->CreatePHI(value->getType(), 2, "castSuccess");
			}

			BasicBlock* refValueBlock = nullptr, * intBlock = nullptr, * floatBlock = nullptr, * primitiveIntBlock = nullptr, * primitiveFloatBlock = nullptr, * primitiveBoolBlock = nullptr;

			builder->SetInsertPoint(origBlock);

			if (NomCastStats)
			{
				builder->CreateCall(GetIncMonotonicCastsFunction(*builder->GetInsertBlock()->getParent()->getParent()), {});
			}

			int refWeight = 100;
			int intWeight = 40;
			int floatWeight = 30;
			int boolWeight = 20;

			if (type->IsSubtype(NomIntClass::GetInstance()->GetType(), true))
			{
				refWeight = 30;
				floatWeight = 1;
				boolWeight = 1;
				intWeight = 100;
			}
			else if (type->IsSubtype(NomFloatClass::GetInstance()->GetType(), true))
			{
				refWeight = 30;
				floatWeight = 100;
				boolWeight = 1;
				intWeight = 1;
			}
			else if (type->IsSubtype(NomBoolClass::GetInstance()->GetType(), true))
			{
				refWeight = 30;
				floatWeight = 1;
				boolWeight = 100;
				intWeight = 1;
			}

			int valueCases = RefValueHeader::GenerateRefOrPrimitiveValueSwitch(builder, value, &refValueBlock, &intBlock, &floatBlock, false, &primitiveIntBlock, nullptr, &primitiveFloatBlock, nullptr, &primitiveBoolBlock, nullptr, refWeight, intWeight, floatWeight, boolWeight);

			SmallVector<tuple<BasicBlock*, Value*, Value*>, 8> targsInObjectSources;
			Value* vtableVar = nullptr;
			if (refValueBlock != nullptr)
			{
				builder->SetInsertPoint(refValueBlock);
				if (!type->Named->IsInterface())
				{
					BasicBlock* directMatchBlock = BasicBlock::Create(LLVMCONTEXT, "directClassMatch", fun);
					BasicBlock* noDirectMatchBlock = BasicBlock::Create(LLVMCONTEXT, "noDirectClassMatch", fun);
					vtableVar = RefValueHeader::GenerateReadVTablePointer(builder, value);
					auto clsptr = RTClass::GetInterfaceReference(builder, vtableVar);
					auto clsMatch = CreatePointerEq(builder, clsptr, type->Named->GetInterfaceDescriptor(*fun->getParent()));
					builder->CreateCondBr(clsMatch, directMatchBlock, noDirectMatchBlock);

					builder->SetInsertPoint(directMatchBlock);
					BasicBlock* outBlocks[2] = { outTrueBlock,outFalseBlock };
					if (type->Arguments.size() != 0)
					{
						auto substStack = GenerateEnvSubstitutions(builder, env, outBlocks, 2, type);
						int argpos = 0;
						for (auto& arg : type->Arguments)
						{
							BasicBlock* nextBlock = BasicBlock::Create(LLVMCONTEXT, "next", fun);
							auto targ = ObjectHeader::GenerateReadTypeArgument(builder, value, argpos);
							RTTypeEq::CreateInlineTypeEqCheck(builder, targ, arg, ConstantPointerNull::get(RTSubstStack::GetLLVMType()->getPointerTo()), substStack, nextBlock, outBlocks[1], outBlocks[1]);
							builder->SetInsertPoint(nextBlock);
							argpos++;
						}
					}
					builder->CreateBr(outBlocks[0]);

					builder->SetInsertPoint(noDirectMatchBlock);
				}
				BasicBlock* classBlock = nullptr, * structuralValueBlock = nullptr;

				RefValueHeader::GenerateNominalStructuralSwitch(builder, value, &vtableVar, &classBlock, (type->Named->IsInterface() ? &structuralValueBlock : &outFalseBlock));
				if (classBlock != nullptr)
				{
					builder->SetInsertPoint(classBlock);
					PHINode* typeArgsPHI = nullptr;
					BasicBlock* foundMatchBlock = nullptr;
					BasicBlock* outBlocks[2] = { outTrueBlock,outFalseBlock };
					if (type->Arguments.size() > 0)
					{
						foundMatchBlock = BasicBlock::Create(LLVMCONTEXT, "foundMatch", fun);
						builder->SetInsertPoint(foundMatchBlock);
						typeArgsPHI = builder->CreatePHI(SuperInstanceEntryType()->getPointerTo(), 2, "matchingEntry");
						auto substStack = GenerateEnvSubstitutions(builder, env, outBlocks, 2, type);
						auto origTypeArgs = ObjectHeader::GeneratePointerToTypeArguments(builder, value);
						auto typeArgs = MakeInvariantLoad(builder, typeArgsPHI, MakeInt32(SuperInstanceEntryFields::TypeArgs), "typeArgs", AtomicOrdering::NotAtomic);
						auto typeEqFun = RTTypeEq::Instance(false).GetLLVMElement(*fun->getParent());
						RTSubstStackValue rssv = RTSubstStackValue(builder, origTypeArgs);
						BasicBlock* outBlock[2];
						rssv.MakeReleaseBlocks(builder, outBlocks[0], &outBlock[0], outBlocks[1], &outBlock[1]);
						int pos = 0;
						for (auto& arg : type->Arguments)
						{
							auto targ = MakeInvariantLoad(builder, builder->CreateGEP(typeArgs, { MakeInt32(-(pos + 1)) }), "typeArg", AtomicOrdering::NotAtomic);
							auto isTypeEq = builder->CreateCall(RTTypeEq::GetLLVMFunctionType(false), typeEqFun, { targ, rssv, arg->GetLLVMElement(*fun->getParent()), substStack });
							isTypeEq->setCallingConv(NOMCC);
							CreateExpect(builder, isTypeEq, MakeUInt(1, 1));

							BasicBlock* nextBlock = BasicBlock::Create(LLVMCONTEXT, "nextArg", fun);

							builder->CreateCondBr(isTypeEq, nextBlock, outBlock[1], GetLikelyFirstBranchMetadata());

							builder->SetInsertPoint(nextBlock);
						}
						builder->CreateBr(outBlock[0]);

						builder->SetInsertPoint(classBlock);
					}
					else
					{
						foundMatchBlock = outTrueBlock;
					}
					if (type->Named->IsInterface())
					{
						auto clsptr = RTClass::GetInterfaceReference(builder, vtableVar);
						auto supercount = RTInterface::GenerateReadSuperInterfaceCount(builder, clsptr);
						auto supers = RTInterface::GenerateReadSuperInterfaces(builder, clsptr);
						BasicBlock* loopHeadBlock = BasicBlock::Create(LLVMCONTEXT, "findInterfaceMatch$head", fun);
						BasicBlock* loopBodyBlock = BasicBlock::Create(LLVMCONTEXT, "findInterfaceMatch$body", fun);
						auto origBlock = builder->GetInsertBlock();
						builder->CreateBr(loopHeadBlock);

						builder->SetInsertPoint(loopHeadBlock);
						auto posPHI = builder->CreatePHI(supercount->getType(), 2, "superIndex");
						posPHI->addIncoming(MakeIntLike(supercount, 0), origBlock);

						auto stillEntriesLeft = builder->CreateICmpULT(posPHI, supercount);
						CreateExpect(builder, stillEntriesLeft, MakeUInt(1, 1));
						builder->CreateCondBr(stillEntriesLeft, loopBodyBlock, outFalseBlock, GetLikelyFirstBranchMetadata());

						builder->SetInsertPoint(loopBodyBlock);
						auto entry = builder->CreateGEP(supers, posPHI);
						auto superIface = MakeInvariantLoad(builder, builder->CreateGEP(entry, { MakeInt32(0), MakeInt32(SuperInstanceEntryFields::Class) }), "superInterface", AtomicOrdering::NotAtomic);
						auto superMatch = CreatePointerEq(builder, superIface, type->Named->GetInterfaceDescriptor(*fun->getParent()));
						posPHI->addIncoming(builder->CreateAdd(posPHI, MakeIntLike(posPHI, 1)), builder->GetInsertBlock());
						builder->CreateCondBr(superMatch, foundMatchBlock, loopHeadBlock);
						if (typeArgsPHI != nullptr)
						{
							typeArgsPHI->addIncoming(entry, builder->GetInsertBlock());
						}
					}
					else
					{
						auto clsptr = RTClass::GetInterfaceReference(builder, vtableVar);
						auto supers = RTInterface::GenerateReadSuperInstances(builder, clsptr);
						BasicBlock* checkSuperClassBlock = BasicBlock::Create(LLVMCONTEXT, "checkSuperClass", fun);
						auto supercount = RTInterface::GenerateReadSuperClassCount(builder, clsptr);
						auto superIndex = MakeIntLike(supercount, type->Named->GetSuperClassCount());
						auto lessThan = builder->CreateICmpULT(superIndex, supercount);
						CreateExpect(builder, lessThan, MakeUInt(1, 1));
						builder->CreateCondBr(lessThan, checkSuperClassBlock, outFalseBlock, GetLikelyFirstBranchMetadata());

						builder->SetInsertPoint(checkSuperClassBlock);
						auto entry = builder->CreateGEP(supers, superIndex);
						auto superClass = MakeInvariantLoad(builder, builder->CreateGEP(entry, { MakeInt32(0), MakeInt32(SuperInstanceEntryFields::Class) }), "superClass", AtomicOrdering::NotAtomic);
						auto superMatch = CreatePointerEq(builder, superClass, type->Named->GetInterfaceDescriptor(*fun->getParent()));
						CreateExpect(builder, lessThan, MakeUInt(1, 1));
						builder->CreateCondBr(superMatch, foundMatchBlock, outFalseBlock, GetLikelyFirstBranchMetadata());
						if (typeArgsPHI != nullptr)
						{
							typeArgsPHI->addIncoming(entry, builder->GetInsertBlock());
						}
					}
				}

				////Nominally typed objects; just collecting blocks here for unified treatments with ints/floats/bools outside
				//if (classBlock != nullptr)
				//{
				//	targsInObjectSources.push_back(make_tuple(classBlock, *value, vtableVar));
				//}

				if (structuralValueBlock != nullptr)
				{
					builder->SetInsertPoint(structuralValueBlock);
					BasicBlock* outBlocks[2] = { outTrueBlock,outFalseBlock };
					auto substStack = GenerateEnvSubstitutions(builder, env, outBlocks, 2, type);
					StructuralValueHeader::GenerateMonotonicStructuralCast(builder, fun, outBlocks[0], outBlocks[1], value, type, substStack);
				}
			}

			if (intBlock != nullptr)
			{
				builder->SetInsertPoint(intBlock);
				if (NomIntClass::GetInstance()->GetType()->IsSubtype(type, false))
				{
					builder->CreateBr(outTrueBlock);
				}
				else if (NomIntClass::GetInstance()->GetType()->IsDisjoint(type))
				{
					builder->CreateBr(outFalseBlock);
				}
				else
				{
					targsInObjectSources.push_back(make_tuple(intBlock, *value, (Value*)ConstantExpr::getPointerCast(NomIntClass::GetInstance()->GetLLVMElement(*fun->getParent()), RTVTable::GetLLVMType()->getPointerTo())));
				}
			}

			if (floatBlock != nullptr)
			{
				builder->SetInsertPoint(floatBlock);
				if (NomFloatClass::GetInstance()->GetType()->IsSubtype(type, false))
				{
					builder->CreateBr(outTrueBlock);
				}
				else if (NomFloatClass::GetInstance()->GetType()->IsDisjoint(type))
				{
					builder->CreateBr(outFalseBlock);
				}
				else
				{
					targsInObjectSources.push_back(make_tuple(floatBlock, *value, (Value*)ConstantExpr::getPointerCast(NomFloatClass::GetInstance()->GetLLVMElement(*fun->getParent()), RTVTable::GetLLVMType()->getPointerTo())));
				}
			}

			if (primitiveIntBlock != nullptr)
			{
				builder->SetInsertPoint(primitiveIntBlock);
				if (NomIntClass::GetInstance()->GetType()->IsSubtype(type, false))
				{
					builder->CreateBr(outTrueBlock);
				}
				else if (NomIntClass::GetInstance()->GetType()->IsDisjoint(type))
				{
					builder->CreateBr(outFalseBlock);
				}
				else
				{
					targsInObjectSources.push_back(make_tuple(primitiveIntBlock, *value, (Value*)ConstantExpr::getPointerCast(NomIntClass::GetInstance()->GetLLVMElement(*fun->getParent()), RTVTable::GetLLVMType()->getPointerTo())));
				}
			}

			if (primitiveFloatBlock != nullptr)
			{
				builder->SetInsertPoint(primitiveFloatBlock);
				if (NomFloatClass::GetInstance()->GetType()->IsSubtype(type, false))
				{
					builder->CreateBr(outTrueBlock);
				}
				else if (NomFloatClass::GetInstance()->GetType()->IsDisjoint(type))
				{
					builder->CreateBr(outFalseBlock);
				}
				else
				{
					targsInObjectSources.push_back(make_tuple(primitiveFloatBlock, *value, (Value*)ConstantExpr::getPointerCast(NomFloatClass::GetInstance()->GetLLVMElement(*fun->getParent()), RTVTable::GetLLVMType()->getPointerTo())));
				}
			}

			if (primitiveBoolBlock != nullptr)
			{
				builder->SetInsertPoint(primitiveBoolBlock);
				if (NomBoolClass::GetInstance()->GetType()->IsSubtype(type, false))
				{
					builder->CreateBr(outTrueBlock);
				}
				else if (NomBoolClass::GetInstance()->GetType()->IsDisjoint(type))
				{
					builder->CreateBr(outFalseBlock);
				}
				else
				{
					targsInObjectSources.push_back(make_tuple(primitiveBoolBlock, *value, (Value*)ConstantExpr::getPointerCast(NomBoolClass::GetInstance()->GetLLVMElement(*fun->getParent()), RTVTable::GetLLVMType()->getPointerTo())));
				}
			}

			int nominalSubtypingCases = targsInObjectSources.size();
			if (nominalSubtypingCases > 0)
			{
				BasicBlock* nominalSubtypingCheck = BasicBlock::Create(LLVMCONTEXT, "nominalSubtypingCheck", fun);
				Value* ifacePtr = nullptr;
				PHINode* ifacePHI = nullptr;
				Value* typeArgsPtr = nullptr;
				PHINode* typeArgsPHI = nullptr;
				if (nominalSubtypingCases > 1)
				{
					builder->SetInsertPoint(nominalSubtypingCheck);
					ifacePHI = builder->CreatePHI(RTInterface::GetLLVMType()->getPointerTo(), nominalSubtypingCases, "interfacePointer");
					typeArgsPHI = builder->CreatePHI(TYPETYPE->getPointerTo(), nominalSubtypingCases, "typeArgs");
					ifacePtr = ifacePHI;
					typeArgsPtr = typeArgsPHI;
				}
				for (auto& tpl : targsInObjectSources)
				{
					builder->SetInsertPoint(std::get<0>(tpl));
					auto typeArgsCast = builder->CreatePointerCast(std::get<1>(tpl), TYPETYPE->getPointerTo());
					auto ifaceCast = RTClass::GetInterfaceReference(builder, std::get<2>(tpl));

					if (nominalSubtypingCases > 1)
					{
						typeArgsPHI->addIncoming(typeArgsCast, builder->GetInsertBlock());
						ifacePHI->addIncoming(ifaceCast, builder->GetInsertBlock());
					}
					else
					{
						typeArgsPtr = typeArgsCast;
						ifacePtr = ifaceCast;
					}
					builder->CreateBr(nominalSubtypingCheck);
				}


				builder->SetInsertPoint(nominalSubtypingCheck);
				//auto stackPtr = builder->CreateIntrinsic(Intrinsic::stacksave, {}, {});
				//builder->CreateIntrinsic(Intrinsic::stackrestore, {}, { stackPtr });
				auto instanceType = builder->CreateAlloca(RTInstanceType::GetLLVMType(), MakeInt32(1));
				auto instanceTypeRef = builder->CreatePointerCast(instanceType, POINTERTYPE);
				builder->CreateIntrinsic(llvm::Intrinsic::lifetime_start, { POINTERTYPE }, { ConstantExpr::getPtrToInt(ConstantExpr::getGetElementPtr(RTInstanceType::GetLLVMType(), ConstantPointerNull::get(RTInstanceType::GetLLVMType()->getPointerTo()), MakeInt32(1)), numtype(size_t)), instanceTypeRef });
				RTInstanceType::CreateInitialization(builder, *fun->getParent(), instanceType, MakeInt<size_t>(0), ConstantPointerNull::get(POINTERTYPE), ifacePtr, typeArgsPtr);
				auto instanceInvariantID = builder->CreateIntrinsic(llvm::Intrinsic::invariant_start, { POINTERTYPE }, { ConstantExpr::getPtrToInt(ConstantExpr::getGetElementPtr(RTInstanceType::GetLLVMType(), ConstantPointerNull::get(RTInstanceType::GetLLVMType()->getPointerTo()), MakeInt32(1)), numtype(size_t)), instanceTypeRef });
				BasicBlock* outBlocks[2] = { outTrueBlock,outFalseBlock };
				
				auto substStack = GenerateEnvSubstitutions(builder, env, outBlocks, 2, type);
				RTSubtyping::CreateInlineSubtypingCheck(builder, builder->CreatePointerCast(instanceType, TYPETYPE), nullptr, type, substStack, outBlocks[0], nullptr, outBlocks[1]);

				builder->SetInsertPoint(outBlocks[0]->getTerminator());
				builder->CreateIntrinsic(llvm::Intrinsic::invariant_end, { POINTERTYPE }, { instanceInvariantID, ConstantExpr::getGetElementPtr(RTInstanceType::GetLLVMType(), ConstantPointerNull::get(RTInstanceType::GetLLVMType()->getPointerTo()), MakeInt32(1)), instanceTypeRef });
				builder->CreateIntrinsic(llvm::Intrinsic::lifetime_end, { POINTERTYPE }, { ConstantExpr::getGetElementPtr(RTInstanceType::GetLLVMType(), ConstantPointerNull::get(RTInstanceType::GetLLVMType()->getPointerTo()), MakeInt32(1)), instanceTypeRef });

				builder->SetInsertPoint(outBlocks[1]->getTerminator());
				builder->CreateIntrinsic(llvm::Intrinsic::invariant_end, { POINTERTYPE }, { instanceInvariantID, ConstantExpr::getGetElementPtr(RTInstanceType::GetLLVMType(), ConstantPointerNull::get(RTInstanceType::GetLLVMType()->getPointerTo()), MakeInt32(1)), instanceTypeRef });
				builder->CreateIntrinsic(llvm::Intrinsic::lifetime_end, { POINTERTYPE }, { ConstantExpr::getGetElementPtr(RTInstanceType::GetLLVMType(), ConstantPointerNull::get(RTInstanceType::GetLLVMType()->getPointerTo()), MakeInt32(1)), instanceTypeRef });

			}

			builder->SetInsertPoint(outBlock);
			return value;
		}

		llvm::Value* RTCast::GenerateMonotonicCast(NomBuilder& builder, CompileEnv* env, NomValue& value, llvm::Value* type)
		{
			BasicBlock* origBlock = builder->GetInsertBlock();
			Function* fun = origBlock->getParent();

			auto castFun = RTTypeHead::GenerateReadCastFun(builder, type);
			auto castFunCall = builder->CreateCall(GetCastFunctionType(), castFun, { type, value });
			castFunCall->setCallingConv(NOMCC);
			return castFunCall;
		}

		uint64_t RTCast::nextCastSiteID()
		{
			static uint64_t csid = 1; return csid++;
		}


		llvm::Value* RTCast::GenerateCast(NomBuilder& builder, CompileEnv* env, NomValue value, NomTypeRef type)
		{
			BasicBlock* origBlock = builder->GetInsertBlock();
			Function* fun = origBlock->getParent();
			BasicBlock* castOutBlock = BasicBlock::Create(LLVMCONTEXT, "castOut", fun);
			PHINode* castOutPHI = nullptr;
			if (value.GetNomType()->IsSubtype(type, false))
			{
				builder->CreateBr(castOutBlock);
				builder->SetInsertPoint(castOutBlock);
				return value;
			}
			if (NomCastStats)
			{
				builder->CreateCall(GetIncCastsFunction(*fun->getParent()), {});
			}
			switch (value.GetNomType()->GetKind())
			{
			case TypeKind::TKBottom:
				builder->CreateBr(castOutBlock);
				builder->SetInsertPoint(castOutBlock);
				return value; //this means we're in dead code, as actual values can't have this type
			case TypeKind::TKMaybe:
				if (NomNullClass::GetInstance()->GetType()->IsSubtype(type))
				{
					Value* castTimeStamp = nullptr;
					if (NomCastStats)
					{
						castTimeStamp = builder->CreateCall(GetGetTimestampFunction(*fun->getParent()), {});
					}
					auto nullobjptr = builder->CreatePtrToInt(NomNullObject::GetInstance()->GetLLVMElement(*env->Module), INTTYPE);
					auto tpeq = builder->CreateICmpEQ(nullobjptr, builder->CreatePtrToInt(value, INTTYPE));
					if (NomCastStats)
					{
						builder->CreateCall(GetIncCastTimeFunction(*fun->getParent()), { castTimeStamp });
					}
					BasicBlock* checkNormalBlock = BasicBlock::Create(LLVMCONTEXT, "checkNonNull", fun);
					builder->CreateCondBr(tpeq, castOutBlock, checkNormalBlock, GetLikelySecondBranchMetadata());

					if (castOutPHI == nullptr)
					{
						builder->SetInsertPoint(castOutBlock);
						castOutPHI = builder->CreatePHI(value->getType(), 3);
					}
					castOutPHI->addIncoming(value, origBlock);

					builder->SetInsertPoint(checkNormalBlock);
				}
				//else, fall through (note, if class types don't fall through anymore, their case needs to be moved)
			case TypeKind::TKClass:
				//todo: optimize by drilling down to how far the statically known type and target type match, for now, fall through and do everything dynamically
			default:
				switch (type->GetKind())
				{
				case TypeKind::TKBottom:
				{
					BasicBlock* errorBlock = RTOutput_Fail::GenerateFailOutputBlock(builder, "Tried to cast value to bottom type!");
					builder->CreateBr(errorBlock);
					return value;
				}
				case TypeKind::TKTop:
				case TypeKind::TKDynamic:
					throw new std::exception(); //the subtyping check at the beginning should have caught that; something's wrong if this execption is thrown
				case TypeKind::TKMaybe:
				{
					Value* castTimeStamp = nullptr;
					if (NomCastStats)
					{
						castTimeStamp = builder->CreateCall(GetGetTimestampFunction(*fun->getParent()), {});
					}
					origBlock = builder->GetInsertBlock();
					BasicBlock* outBlock = BasicBlock::Create(LLVMCONTEXT, "outBlock", fun);
					BasicBlock* IsNotNullBlock = BasicBlock::Create(LLVMCONTEXT, "isNotNull", fun);
					auto nullobjptr = builder->CreatePtrToInt(NomNullObject::GetInstance()->GetLLVMElement(*env->Module), INTTYPE);
					auto nullcmp = builder->CreateICmpEQ(nullobjptr, builder->CreatePtrToInt(value, INTTYPE));
					builder->CreateIntrinsic(Intrinsic::expect, { inttype(1) }, { nullcmp, MakeUInt(1,0) });
					builder->CreateCondBr(nullcmp, outBlock, IsNotNullBlock, GetLikelySecondBranchMetadata());

					builder->SetInsertPoint(IsNotNullBlock);
					auto castresult = GenerateCast(builder, env, value, ((NomMaybeTypeRef)type)->PotentialType);
					BasicBlock* notNullBlock = builder->GetInsertBlock();
					builder->CreateBr(outBlock);

					builder->SetInsertPoint(outBlock);
					auto resultPHI = builder->CreatePHI(value->getType(), 2, "subtypingResult");
					resultPHI->addIncoming(value, origBlock);
					resultPHI->addIncoming(castresult, notNullBlock);
					if (NomCastStats)
					{
						builder->CreateCall(GetIncCastTimeFunction(*fun->getParent()), { castTimeStamp });
					}
					builder->CreateBr(castOutBlock);

					builder->SetInsertPoint(castOutBlock);
					if (castOutPHI != nullptr)
					{
						castOutPHI->addIncoming(resultPHI, outBlock);
						return castOutPHI;
					}
					return resultPHI;
				}
				case TypeKind::TKClass:
				{
					origBlock = builder->GetInsertBlock();
					Value* castTimeStamp = nullptr;
					if (NomCastStats)
					{
						castTimeStamp = builder->CreateCall(GetGetTimestampFunction(*fun->getParent()), {});
					}
					auto itype = (NomClassTypeRef)type;
					auto monoCastResult = GenerateMonotonicCast(builder, env, value, itype);
					//builder->CreateIntrinsic(Intrinsic::expect, { inttype(1) }, { monoCastResult,MakeUInt(1,1) });
					if (NomCastStats)
					{
						builder->CreateCall(GetIncCastTimeFunction(*fun->getParent()), { castTimeStamp });
					}
					builder->CreateBr(castOutBlock);

					builder->SetInsertPoint(castOutBlock);
					if (castOutPHI != nullptr)
					{
						castOutPHI->addIncoming(monoCastResult, origBlock);
						return castOutPHI;
					}
					return monoCastResult;
				}
				case TypeKind::TKVariable:
				{
					origBlock = builder->GetInsertBlock();
					Value* castTimeStamp = nullptr;
					if (NomCastStats)
					{
						castTimeStamp = builder->CreateCall(GetGetTimestampFunction(*fun->getParent()), {});
					}
					auto monoCastResult = GenerateMonotonicCast(builder, env, value, env->GetTypeArgument(builder, ((NomTypeVarRef)type)->GetIndex()));
					//builder->CreateIntrinsic(Intrinsic::expect, { inttype(1) }, { monoCastResult,MakeUInt(1,1) });
					if (NomCastStats)
					{
						builder->CreateCall(GetIncCastTimeFunction(*fun->getParent()), { castTimeStamp });
					}
					builder->CreateBr(castOutBlock);

					builder->SetInsertPoint(castOutBlock);
					if (castOutPHI != nullptr)
					{
						castOutPHI->addIncoming(monoCastResult, origBlock);
						return castOutPHI;
					}
					return monoCastResult;
				}
				default:
					throw new std::exception();
				}
			}
		}

		llvm::Value* RTCast::GenerateCast(NomBuilder& builder, CompileEnv* env, llvm::Value* value, NomTypeRef type)
		{
			return GenerateCast(builder, env, NomValue(value, &NomDynamicType::Instance(), false), type);
		}
		llvm::Function* FailingAdjustFun::createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const
		{
			Function* fun = mod.getFunction("RT_NOM_CASTADJUST_FAILING");
			if (fun != nullptr)
			{
				return fun;
			}
			fun = Function::Create(RTCast::GetAdjustFunctionType(), linkage, "RT_NOM_CASTADJUST_FAILING", mod);
			fun->setCallingConv(NOMCC);
			NomBuilder builder;

			BasicBlock* start = BasicBlock::Create(LLVMCONTEXT, "", fun);
			builder->SetInsertPoint(start);
			builder->CreateRet(MakeUInt(1, 0));
			return fun;
		}
		llvm::Function* FailingAdjustFun::findLLVMElement(llvm::Module& mod) const
		{
			return mod.getFunction("RT_NOM_CASTADJUST_FAILING");
		}
	}
}
