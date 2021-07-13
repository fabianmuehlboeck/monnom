#include "StructHeader.h"
#include "RefValueHeader.h"
#include "RTStruct.h"
#include "CompileHelpers.h"
#include "NomVMInterface.h"
#include "NomStruct.h"
#include "NomField.h"
#include "NomDynamicType.h"
#include "RTCompileConfig.h"

using namespace llvm;
using namespace std;
namespace Nom
{
	namespace Runtime
	{
		llvm::StructType* StructHeader::GetLLVMType()
		{
			static llvm::StructType* shst = StructType::create(LLVMCONTEXT, "RT_NOM_StructHeader");
			static bool once = true;
			if (once)
			{
				once = false;
				shst->setBody(
					arrtype(TYPETYPE, 0),																//cast type args
					RefValueHeader::GetLLVMType(),														//vtable/flag
					(NomLambdaOptimizationLevel > 0 ? (llvm::Type*)POINTERTYPE : arrtype(POINTERTYPE, 0)),		//potential space for raw invoke pointer
					RTStruct::GetLLVMType()->getPointerTo(),											//struct metadata
					POINTERTYPE,																		//instance dictionary
					arrtype(TYPETYPE, 0),																//Closure type args
					arrtype(REFTYPE, 0)																	//Fields
				);
			}
			return shst;
		}

		llvm::Value* StructHeader::GenerateReadStructDescriptor(NomBuilder& builder, llvm::Value* objPointer)
		{
			return MakeLoad(builder, objPointer, GetLLVMType()->getPointerTo(), MakeInt32(StructHeaderFields::StructDescriptor), "structDescriptor");
		}

		llvm::Value* StructHeader::GenerateReadStructDictionary(NomBuilder& builder, llvm::Value* objPointer)
		{
			return MakeLoad(builder, objPointer, GetLLVMType()->getPointerTo(), MakeInt32(StructHeaderFields::InstanceDictionary), "structInstanceDictionary");
		}

		llvm::Value* StructHeader::GenerateWriteField(NomBuilder& builder, llvm::Value* thisObj, int32_t fieldindex, llvm::Value* value, bool hasRawInvoke)
		{
			return MakeStore(builder, value, builder->CreatePointerCast(thisObj, GetLLVMType()->getPointerTo()), { MakeInt32(StructHeaderFields::Fields), MakeInt32(fieldindex) });
			//return MakeStore(builder, builder->CreatePointerCast(thisObj, GetLLVMType()->getPointerTo()), value, { MakeInt32(StructHeaderFields::Fields), builder->CreateSub(MakeInt32(-((hasRawInvoke && (NomLambdaOptimizationLevel > 0)) ? 2 : 1)), MakeInt32(fieldindex)) });
		}
		llvm::Value* StructHeader::GenerateWriteField(NomBuilder& builder, llvm::Value* thisObj, llvm::Value* fieldindex, llvm::Value* value, bool hasRawInvoke)
		{
			return MakeStore(builder, value, builder->CreatePointerCast(thisObj, GetLLVMType()->getPointerTo()), { MakeInt32(StructHeaderFields::Fields), fieldindex });
			//return MakeStore(builder, builder->CreatePointerCast(thisObj, GetLLVMType()->getPointerTo()), value, { MakeInt32(StructHeaderFields::Fields), builder->CreateSub(MakeInt32(-((hasRawInvoke && (NomLambdaOptimizationLevel > 0)) ? 2 : 1)), fieldindex) });
		}

		llvm::Value* StructHeader::GenerateReadField(NomBuilder& builder, llvm::Value* thisObj, int32_t fieldindex, bool hasRawInvoke)
		{
			return MakeLoad(builder, builder->CreatePointerCast(thisObj, GetLLVMType()->getPointerTo()), { MakeInt32(StructHeaderFields::Fields), MakeInt32(fieldindex) });
			//return MakeLoad(builder, builder->CreatePointerCast(thisObj, GetLLVMType()->getPointerTo()), { MakeInt32(StructHeaderFields::Fields), builder->CreateSub(MakeInt32(-((hasRawInvoke&&(NomLambdaOptimizationLevel>0))?2:1)), MakeInt32(fieldindex)) });
		}

		llvm::Value* StructHeader::GetFieldPointer(NomBuilder& builder, llvm::Value* thisObj, llvm::Value* fieldindex, bool hasRawInvoke)
		{
			return builder->CreateGEP(builder->CreatePointerCast(thisObj, GetLLVMType()->getPointerTo()), { MakeInt32(0), MakeInt32(StructHeaderFields::Fields), fieldindex });
			//return builder->CreateGEP(builder->CreatePointerCast(thisObj, GetLLVMType()->getPointerTo()), { MakeInt32(0), MakeInt32(StructHeaderFields::Fields), builder->CreateSub(MakeInt32(-((hasRawInvoke && (NomLambdaOptimizationLevel > 0)) ? 2 : 1)), fieldindex) });
		}
		llvm::Value* StructHeader::GenerateReadField(NomBuilder& builder, llvm::Value* thisObj, llvm::Value* fieldindex, bool hasRawInvoke)
		{
			return MakeLoad(builder, builder->CreatePointerCast(thisObj, GetLLVMType()->getPointerTo()), { MakeInt32(StructHeaderFields::Fields), fieldindex });
			//return MakeLoad(builder, builder->CreatePointerCast(thisObj, GetLLVMType()->getPointerTo()), { MakeInt32(StructHeaderFields::Fields), builder->CreateSub(MakeInt32(-((hasRawInvoke && (NomLambdaOptimizationLevel > 0)) ? 2 : 1)), fieldindex) });
		}
		llvm::Value* StructHeader::GenerateReadTypeArgument(NomBuilder& builder, llvm::Value* thisObj, int32_t argindex, bool hasRawInvoke)
		{
			auto rtstructdesc = GenerateReadStructDescriptor(builder, thisObj);
			return MakeLoad(builder, builder->CreateGEP(builder->CreatePointerCast(thisObj, GetLLVMType()->getPointerTo()), { MakeInt32(0), MakeInt32(StructHeaderFields::TypeArgs), builder->CreateSub(builder->CreateAdd(RTStruct::GenerateReadFieldCount(builder, rtstructdesc), RTStruct::GenerateReadTypeArgCount(builder, rtstructdesc)), MakeInt32(argindex + 1)) }));
		}
		llvm::Value* StructHeader::GenerateWriteTypeArgument(NomBuilder& builder, llvm::Value* thisObj, int32_t argindex, llvm::Value* targ, bool hasRawInvoke)
		{
			auto rtstructdesc = GenerateReadStructDescriptor(builder, thisObj);
			return MakeStore(builder, targ, builder->CreateGEP(builder->CreatePointerCast(thisObj, GetLLVMType()->getPointerTo()), { MakeInt32(0), MakeInt32(StructHeaderFields::TypeArgs), builder->CreateSub(builder->CreateZExtOrTrunc(builder->CreateAdd(RTStruct::GenerateReadFieldCount(builder, rtstructdesc), RTStruct::GenerateReadTypeArgCount(builder, rtstructdesc)), numtype(int32_t)), MakeInt32(argindex + 1)) }));
		}
		llvm::Value* StructHeader::GeneratePointerToTypeArguments(NomBuilder& builder, llvm::Value* thisObj, bool hasRawInvoke)
		{
			auto rtstructdesc = GenerateReadStructDescriptor(builder, thisObj);
			return builder->CreateGEP(builder->CreatePointerCast(thisObj, GetLLVMType()->getPointerTo()), { MakeInt32(0), MakeInt32(StructHeaderFields::TypeArgs), builder->CreateAdd(RTStruct::GenerateReadFieldCount(builder, rtstructdesc), RTStruct::GenerateReadTypeArgCount(builder,rtstructdesc)) });
			//return MakeLoad(builder, builder->CreateGEP(builder->CreatePointerCast(thisObj, GetLLVMType()->getPointerTo()), { MakeInt32(0), MakeInt32(StructHeaderFields::TypeArgs), builder->CreateSub(MakeInt32(-((hasRawInvoke && (NomLambdaOptimizationLevel > 0)) ? 2 : 1)), builder->CreateAdd(RTStruct::GenerateReadFieldCount(builder, rtstructdesc), RTStruct::GenerateReadTypeArgCount(builder,rtstructdesc))) }));
		}
		llvm::Value* StructHeader::GenerateWriteCastTypeArgument(NomBuilder& builder, llvm::Value* thisObj, llvm::Value* argindex, llvm::Value* targ)
		{
			return MakeStore(builder, targ, builder->CreateGEP(builder->CreatePointerCast(thisObj, GetLLVMType()->getPointerTo()), { MakeInt32(0), MakeInt32(StructHeaderFields::CastTypeArgs), builder->CreateSub(MakeInt32(-1),argindex) }));
		}
		llvm::Value* StructHeader::GenerateWriteCastTypeArgumentCMPXCHG(NomBuilder& builder, llvm::Value* thisObj, llvm::Value* argindex, llvm::Value* targ)
		{
			auto argPtr = builder->CreateGEP(builder->CreatePointerCast(thisObj, GetLLVMType()->getPointerTo()), { MakeInt32(0), MakeInt32(StructHeaderFields::CastTypeArgs), builder->CreateSub(MakeInt32(-1),argindex) });
			return builder->CreateAtomicCmpXchg(argPtr, ConstantPointerNull::get(TYPETYPE), targ, AtomicOrdering::AcquireRelease, AtomicOrdering::Acquire);
		}

		llvm::Value* StructHeader::GeneratePointerToCastTypeArguments(NomBuilder& builder, llvm::Value* thisObj)
		{
			return builder->CreateGEP(builder->CreatePointerCast(thisObj, GetLLVMType()->getPointerTo()), { MakeInt32(0), MakeInt32(StructHeaderFields::CastTypeArgs), MakeInt32(0) });
		}

		void StructHeader::GenerateConstructorCode(NomBuilder& builder, llvm::ArrayRef<llvm::Value*> typeArguments, StructInstantiationCompileEnv* env, llvm::Value* descriptorRef, const std::vector<NomInstruction*>* instructions)
		{
			Function* fun = builder->GetInsertBlock()->getParent();
			Function* allocfun = GetStructAlloc(fun->getParent());
			llvm::Value* extraSlots = RTStruct::GenerateReadPreallocatedSlots(builder, descriptorRef);

			llvm::Value* newmem = builder->CreatePointerCast(builder->CreateCall(allocfun, { MakeInt<size_t>(env->Struct->Fields.size()), MakeInt<size_t>(typeArguments.size()), extraSlots }), GetLLVMType()->getPointerTo());

			(*env)[0] = NomValue(newmem, &NomDynamicType::Instance());

			for (auto instr : *instructions)
			{
				instr->Compile(builder, env, 0);
			}

			for (auto field : env->Struct->Fields)
			{
				GenerateWriteField(builder, newmem, field->Index, EnsurePacked(builder, (*env)[field->ValueRegister]), NomLambdaOptimizationLevel > 0 && env->Struct->GetHasRawInvoke());
			}

			int argIndex = 0;
			for (auto targ : typeArguments)
			{
				GenerateWriteTypeArgument(builder, newmem, argIndex, targ, NomLambdaOptimizationLevel > 0 && env->Struct->GetHasRawInvoke());
				argIndex++;
			}
			MakeStore(builder, descriptorRef, newmem, MakeInt32(StructHeaderFields::StructDescriptor));

			llvm::Value* instanceKind = MakeInt((unsigned char)RTDescriptorKind::Struct);

			auto refValHeader = builder->CreateGEP(newmem, { MakeInt32(0), MakeInt32(StructHeaderFields::RefValueHeader) }, "RefValHeader");

			auto argSlotCount = builder->CreateIntToPtr(builder->CreateShl(EnsureIntegerSize(builder, extraSlots, bitsin(intptr_t)), MakeInt<intptr_t>(bitsin(intptr_t) / 2)), POINTERTYPE);

			RefValueHeader::GenerateInitializerCode(builder, refValHeader, argSlotCount, instanceKind);
			builder->CreateRet(refValHeader);
		}
	}
}