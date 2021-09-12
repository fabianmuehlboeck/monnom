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

			BasicBlock* outTrueBlock = BasicBlock::Create(LLVMCONTEXT, "MonotonicCastOutTrue", fun);
			BasicBlock* outFalseBlock = BasicBlock::Create(LLVMCONTEXT, "MonotonicCastOutFalse", fun);

			BasicBlock* outBlock = BasicBlock::Create(LLVMCONTEXT, "MonotonicCastOut", fun);

			PHINode* outPHI;
			{
				builder->SetInsertPoint(outBlock);
				outPHI = builder->CreatePHI(BOOLTYPE, 2, "castSuccess");

				builder->SetInsertPoint(outTrueBlock);
				builder->CreateBr(outBlock);
				outPHI->addIncoming(MakeUInt(1, 1), outTrueBlock);

				builder->SetInsertPoint(outFalseBlock);
				builder->CreateBr(outBlock);
				outPHI->addIncoming(MakeUInt(1, 0), outFalseBlock);
			}

			BasicBlock* refValueBlock = nullptr, * intBlock = nullptr, * floatBlock = nullptr, * primitiveIntBlock = nullptr, * primitiveFloatBlock = nullptr, * primitiveBoolBlock = nullptr;

			builder->SetInsertPoint(origBlock);

			if (NomCastStats)
			{
				builder->CreateCall(GetIncMonotonicCastsFunction(*builder->GetInsertBlock()->getParent()->getParent()), {});
			}

			auto stackPtr = builder->CreateIntrinsic(Intrinsic::stacksave, {}, {});

			int valueCases = RefValueHeader::GenerateRefOrPrimitiveValueSwitch(builder, value, &refValueBlock, &intBlock, &floatBlock, false, &primitiveIntBlock, nullptr, &primitiveFloatBlock, nullptr, &primitiveBoolBlock, nullptr);

			SmallVector<tuple<BasicBlock*, Value*, Value*>, 8> targsInObjectSources;
			Value* vtableVar = nullptr;
			if (refValueBlock != nullptr)
			{
				builder->SetInsertPoint(refValueBlock);

				BasicBlock* classBlock = nullptr, * structuralValueBlock = nullptr;

				int vtableCases = RefValueHeader::GenerateNominalStructuralSwitch(builder, value, &vtableVar, &classBlock, &structuralValueBlock);

				//Nominally typed objects; just collecting blocks here for unified treatments with ints/floats/bools outside
				if (classBlock != nullptr)
				{
					targsInObjectSources.push_back(make_tuple(classBlock, *value, vtableVar));
				}

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
					auto ifaceCast = RTClass::GetInterfaceReference(builder,std::get<2>(tpl));

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
				auto instanceType = builder->CreateAlloca(RTInstanceType::GetLLVMType(), MakeInt32(1));
				RTInstanceType::CreateInitialization(builder, *fun->getParent(), instanceType, MakeInt<size_t>(0), ConstantPointerNull::get(POINTERTYPE), ifacePtr, typeArgsPtr);

				BasicBlock* outBlocks[2] = { outTrueBlock,outFalseBlock };
				auto substStack = GenerateEnvSubstitutions(builder, env, outBlocks, 2, type);
				RTSubtyping::CreateInlineSubtypingCheck(builder, builder->CreatePointerCast(instanceType, TYPETYPE), nullptr, type, substStack, outBlocks[0], nullptr, outBlocks[1]);
			}

			builder->SetInsertPoint(outBlock);
			builder->CreateIntrinsic(Intrinsic::stackrestore, {}, { stackPtr });
			return outPHI;
		}

		llvm::Value* RTCast::GenerateMonotonicCast(NomBuilder& builder, CompileEnv* env, NomValue& value, llvm::Value* type)
		{
			BasicBlock* origBlock = builder->GetInsertBlock();
			Function* fun = origBlock->getParent();

			BasicBlock* outTrueBlock = BasicBlock::Create(LLVMCONTEXT, "MonotonicCastOutTrue", fun);
			BasicBlock* outFalseBlock = BasicBlock::Create(LLVMCONTEXT, "MonotonicCastOutFalse", fun);

			BasicBlock* outBlock = BasicBlock::Create(LLVMCONTEXT, "MonotonicCastOut", fun);
			PHINode* outPHI;
			{
				builder->SetInsertPoint(outBlock);
				outPHI = builder->CreatePHI(BOOLTYPE, 2, "castSuccess");

				builder->SetInsertPoint(outTrueBlock);
				builder->CreateBr(outBlock);
				outPHI->addIncoming(MakeUInt(1, 1), outTrueBlock);

				builder->SetInsertPoint(outFalseBlock);
				builder->CreateBr(outBlock);
				outPHI->addIncoming(MakeUInt(1, 0), outFalseBlock);
			}

			BasicBlock* refValueBlock = nullptr, * intBlock = nullptr, * floatBlock = nullptr, * primitiveIntBlock = nullptr, * primitiveFloatBlock = nullptr, * primitiveBoolBlock = nullptr;

			builder->SetInsertPoint(origBlock);
			if (NomCastStats)
			{
				builder->CreateCall(GetIncMonotonicCastsFunction(*builder->GetInsertBlock()->getParent()->getParent()), {});
			}
			auto stackPtr = builder->CreateIntrinsic(Intrinsic::stacksave, {}, {});

			int valueCases = RefValueHeader::GenerateRefOrPrimitiveValueSwitch(builder, value, &refValueBlock, &intBlock, &floatBlock, false, &primitiveIntBlock, nullptr, &primitiveFloatBlock, nullptr, &primitiveBoolBlock, nullptr);

			SmallVector<tuple<BasicBlock*, Value*, Value*>, 8> targsInObjectSources;
			Value* vtableVar = nullptr;
			if (refValueBlock != nullptr)
			{
				builder->SetInsertPoint(refValueBlock);



				BasicBlock* classBlock = nullptr, * structuralValueBlock = nullptr;

				int vtableCases = RefValueHeader::GenerateNominalStructuralSwitch(builder, value, &vtableVar, &classBlock, &structuralValueBlock);

				//Nominal objects; just collecting blocks here for unified treatments with ints/floats/bools outside
				if (classBlock != nullptr)
				{
					targsInObjectSources.push_back(make_tuple(classBlock, *value, vtableVar));
				}

				//STRUCTURAL VALUE - note difference to inlined version above: there we know we have a class type, here we can get any type
				if (structuralValueBlock != nullptr)
				{
					builder->SetInsertPoint(structuralValueBlock);
					BasicBlock* outBlocks[2] = { outTrueBlock, outFalseBlock };
					auto substStack = GenerateEnvSubstitutions(builder, env, outBlocks, 2);
					BasicBlock* rightClassTypeBlock = nullptr, * rightInstanceTypeBlock = nullptr;
					Value* rightPHI = nullptr;
					Value* rightSubstPHI = nullptr;
					RTTypeHead::GenerateTypeKindSwitchRecurse(builder, type, substStack, &rightPHI, &rightSubstPHI, &rightClassTypeBlock, &(outBlocks[0]), &(outBlocks[1]), &(outBlocks[1]), &rightInstanceTypeBlock, &(outBlocks[0]), nullptr, outBlocks[1]);

					if (rightClassTypeBlock)
					{
						builder->SetInsertPoint(rightClassTypeBlock);
						auto iface = RTClassType::GenerateReadClassDescriptorLink(builder, rightPHI);
						StructuralValueHeader::GenerateMonotonicStructuralCast(builder, fun, outBlocks[0], outBlocks[1], value, rightPHI, iface, RTClassType::GetTypeArgumentsPtr(builder, rightPHI), rightSubstPHI);
					}

					if (rightInstanceTypeBlock)
					{
						builder->SetInsertPoint(rightInstanceTypeBlock);
						auto iface = RTInstanceType::GenerateReadClassDescriptorLink(builder, rightPHI);
						StructuralValueHeader::GenerateMonotonicStructuralCast(builder, fun, outBlocks[0], outBlocks[1], value, rightPHI, iface, RTInstanceType::GetTypeArgumentsPtr(builder, rightPHI), rightSubstPHI);
					}
				}
			}

			if (intBlock != nullptr)
			{
				builder->SetInsertPoint(intBlock);
				targsInObjectSources.push_back(make_tuple(intBlock, *value, (Value*)ConstantExpr::getPointerCast(NomIntClass::GetInstance()->GetLLVMElement(*fun->getParent()), RTVTable::GetLLVMType()->getPointerTo())));
			}

			if (floatBlock != nullptr)
			{
				builder->SetInsertPoint(floatBlock);
				targsInObjectSources.push_back(make_tuple(floatBlock, *value, (Value*)ConstantExpr::getPointerCast(NomFloatClass::GetInstance()->GetLLVMElement(*fun->getParent()), RTVTable::GetLLVMType()->getPointerTo())));
			}

			if (primitiveIntBlock != nullptr)
			{
				builder->SetInsertPoint(primitiveIntBlock);
				targsInObjectSources.push_back(make_tuple(primitiveIntBlock, *value, (Value*)ConstantExpr::getPointerCast(NomIntClass::GetInstance()->GetLLVMElement(*fun->getParent()), RTVTable::GetLLVMType()->getPointerTo())));
			}

			if (primitiveFloatBlock != nullptr)
			{
				builder->SetInsertPoint(primitiveFloatBlock);
				targsInObjectSources.push_back(make_tuple(primitiveFloatBlock, *value, (Value*)ConstantExpr::getPointerCast(NomFloatClass::GetInstance()->GetLLVMElement(*fun->getParent()), RTVTable::GetLLVMType()->getPointerTo())));
			}

			if (primitiveBoolBlock != nullptr)
			{
				builder->SetInsertPoint(primitiveBoolBlock);
				targsInObjectSources.push_back(make_tuple(primitiveBoolBlock, *value, (Value*)ConstantExpr::getPointerCast(NomBoolClass::GetInstance()->GetLLVMElement(*fun->getParent()), RTVTable::GetLLVMType()->getPointerTo())));
			}

			int nominalSubtypingCases = targsInObjectSources.size();
			if (nominalSubtypingCases > 0)
			{
				BasicBlock* nominalSubtypingCheck = BasicBlock::Create(LLVMCONTEXT, "nominalSubtypingCheck", fun);
				Value* adjustFunction = nullptr;
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
					auto ifaceCast = RTClass::GetInterfaceReference(builder,std::get<2>(tpl));

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
				Value* instanceType = builder->CreateAlloca(RTInstanceType::GetLLVMType(), MakeInt32(1));
				RTInstanceType::CreateInitialization(builder, *fun->getParent(), instanceType, MakeInt<size_t>(0), ConstantPointerNull::get(POINTERTYPE), ifacePtr, typeArgsPtr);
				BasicBlock* outBlocks[2] = { outTrueBlock,outFalseBlock };
				auto envSubstitutions = GenerateEnvSubstitutions(builder, env, outBlocks, 2);
				instanceType = builder->CreateGEP(instanceType, { MakeInt32(0), MakeInt32(RTInstanceTypeFields::Head) });
				auto subtypingResult = builder->CreateCall(RTSubtyping::Instance().GetLLVMElement(*fun->getParent()), { instanceType, type, ConstantPointerNull::get(RTSubtyping::TypeArgumentListStackType()->getPointerTo()), envSubstitutions });
				subtypingResult->setCallingConv(NOMCC);
				auto subtypingFailed = builder->CreateICmpEQ(subtypingResult, MakeUInt(2, 0));
				builder->CreateIntrinsic(Intrinsic::expect, { inttype(1) }, { subtypingFailed, MakeUInt(1,0) });
				builder->CreateCondBr(subtypingFailed, outBlocks[1], outBlocks[0], GetLikelySecondBranchMetadata());
			}

			builder->SetInsertPoint(outBlock);
			builder->CreateIntrinsic(Intrinsic::stackrestore, {}, { stackPtr });

			return outPHI;
		}

		uint64_t RTCast::nextCastSiteID()
		{
			static uint64_t csid = 1; return csid++;
		}


		llvm::Value* RTCast::GenerateCast(NomBuilder& builder, CompileEnv* env, NomValue value, NomTypeRef type)
		{
			BasicBlock* origBlock = builder->GetInsertBlock();
			Function* fun = origBlock->getParent();
			if (value.GetNomType()->IsSubtype(type, false))
			{
				return MakeInt(1, (uint64_t)1);
			}
			if (NomCastStats)
			{
				builder->CreateCall(GetIncCastsFunction(*fun->getParent()), {});
			}
			switch (value.GetNomType()->GetKind())
			{
			case TypeKind::TKBottom:
				return MakeInt(1, (uint64_t)1); //this means we're in dead code, as actual values can't have this type
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
					return tpeq;
				}
				//else, fall through (note, if class types don't fall through anymore, their case needs to be moved)
			case TypeKind::TKClass:
				//todo: optimize by drilling down to how far the statically known type and target type match, for now, fall through and do everything dynamically
			default:
				switch (type->GetKind())
				{
				case TypeKind::TKBottom:
					return MakeInt(1, (uint64_t)0);
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
					auto resultPHI = builder->CreatePHI(BOOLTYPE, 2, "subtypingResult");
					resultPHI->addIncoming(MakeInt(1, (uint64_t)1), origBlock);
					resultPHI->addIncoming(castresult, notNullBlock);
					builder->CreateIntrinsic(Intrinsic::expect, { inttype(1) }, { resultPHI,MakeUInt(1,1) });
					if (NomCastStats)
					{
						builder->CreateCall(GetIncCastTimeFunction(*fun->getParent()), { castTimeStamp });
					}
					return resultPHI;
				}
				case TypeKind::TKClass:
				{
					Value* castTimeStamp = nullptr;
					if (NomCastStats)
					{
						castTimeStamp = builder->CreateCall(GetGetTimestampFunction(*fun->getParent()), {});
					}
					auto itype = (NomClassTypeRef)type;
					auto monoCastResult = GenerateMonotonicCast(builder, env, value, itype);
					builder->CreateIntrinsic(Intrinsic::expect, { inttype(1) }, { monoCastResult,MakeUInt(1,1) });
					if (NomCastStats)
					{
						builder->CreateCall(GetIncCastTimeFunction(*fun->getParent()), { castTimeStamp });
					}
					return monoCastResult;
				}
				case TypeKind::TKVariable:
				{
					Value* castTimeStamp = nullptr;
					if (NomCastStats)
					{
						castTimeStamp = builder->CreateCall(GetGetTimestampFunction(*fun->getParent()), {});
					}
					auto monoCastResult = GenerateMonotonicCast(builder, env, value, env->GetTypeArgument(builder, ((NomTypeVarRef)type)->GetIndex()));
					builder->CreateIntrinsic(Intrinsic::expect, { inttype(1) }, { monoCastResult,MakeUInt(1,1) });
					if (NomCastStats)
					{
						builder->CreateCall(GetIncCastTimeFunction(*fun->getParent()), { castTimeStamp });
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
