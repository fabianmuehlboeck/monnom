#include "RTMaybeType.h"
#include "RTTypeHead.h"
#include "llvm/IR/Constant.h"
#include "Defs.h"
#include "CompileHelpers.h"
#include "NullClass.h"
#include "Metadata.h"
#include "CallingConvConf.h"

using namespace llvm;
namespace Nom
{
	namespace Runtime
	{
		llvm::StructType* RTMaybeType::GetLLVMType()
		{
			static llvm::StructType* llvmtype = llvm::StructType::create(LLVMCONTEXT, "LLVMRTMaybeType");
			static bool dontrepeat = true;
			if (dontrepeat)
			{
				dontrepeat = false;
				llvmtype->setBody(
					RTTypeHead::GetLLVMType(), //header
					RTTypeHead::GetLLVMType()->getPointerTo() //potentialType
				);
			}
			return llvmtype;
		}
		llvm::Constant* GetMaybeTypeCastFunction(llvm::Module& mod)
		{
			static llvm::Module* mainModule = nullptr;
			GlobalValue::LinkageTypes linkage = GlobalValue::LinkageTypes::ExternalLinkage;
			if (mainModule == nullptr)
			{
				mainModule = &mod;
			}
			else if(mainModule!=&mod)
			{
				GlobalValue::LinkageTypes linkage = GlobalValue::LinkageTypes::AvailableExternallyLinkage;
			}
			auto fun = mod.getFunction("MONNOM_RT_TYPECAST_MAYBE");
			if (fun == nullptr)
			{
				fun = Function::Create(GetCastFunctionType(), linkage, "MONNOM_RT_TYPECAST_MAYBE", mod);
				fun->setCallingConv(NOMCC);
				BasicBlock* startBlock = BasicBlock::Create(LLVMCONTEXT, "", fun);
				BasicBlock* successBlock = BasicBlock::Create(LLVMCONTEXT, "success", fun);
				BasicBlock* recurseBlock = BasicBlock::Create(LLVMCONTEXT, "recurse", fun);
				auto nullobj = NomNullObject::GetInstance()->GetLLVMElement(mod);
				auto argiter = fun->arg_begin();
				auto self = argiter;
				argiter++;
				auto castValue = argiter;
				NomBuilder builder;

				builder->SetInsertPoint(startBlock);
				auto isNullObj = CreatePointerEq(builder, castValue, nullobj, "isNull");
				builder->CreateIntrinsic(Intrinsic::expect, { inttype(1) }, { isNullObj, MakeUInt(1,1) });
				builder->CreateCondBr(isNullObj, successBlock, recurseBlock, GetLikelySecondBranchMetadata());

				builder->SetInsertPoint(successBlock);
				builder->CreateRet(castValue);

				builder->SetInsertPoint(recurseBlock);
				auto ptype = RTMaybeType::GenerateReadPotentialType(builder, self);
				auto castfun = RTTypeHead::GenerateReadCastFun(builder, ptype);
				auto recurseCall = builder->CreateCall(GetCastFunctionType(), castfun, { ptype, castValue, ConstantPointerNull::get(POINTERTYPE), ConstantPointerNull::get(POINTERTYPE) });
				recurseCall->setCallingConv(NOMCC);
				recurseCall->setTailCallKind(CallInst::TailCallKind::TCK_MustTail);
				builder->CreateRet(recurseCall);
			}
			return fun;
		}
		llvm::Constant* RTMaybeType::GetConstant(llvm::Module& mod, const NomMaybeType* mbt)
		{
			return ConstantStruct::get(GetLLVMType(), RTTypeHead::GetConstant(TypeKind::TKMaybe, MakeInt<size_t>(mbt->GetHashCode()), mbt, GetMaybeTypeCastFunction(mod)), mbt->PotentialType->GetLLVMElement(mod));
		}
		uint64_t RTMaybeType::HeadOffset()
		{
			static const uint64_t offset = GetLLVMLayout()->getElementOffset((unsigned char)RTMaybeTypeFields::Head); return offset;
		}
		llvm::Value* RTMaybeType::GenerateReadPotentialType(NomBuilder& builder, llvm::Value* type)
		{
			return MakeLoad(builder, builder->CreatePointerCast(type, GetLLVMType()->getPointerTo()), MakeInt<RTMaybeTypeFields>(RTMaybeTypeFields::PotentialType));
		}
	}
}