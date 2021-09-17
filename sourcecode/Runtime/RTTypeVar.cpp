#include "RTTypeVar.h"
#include "NomTypeVar.h"
#include "CompileHelpers.h"
#include "llvm/IR/Constant.h"

using namespace llvm;
using namespace std;
namespace Nom
{
	namespace Runtime
	{
		llvm::StructType * RTTypeVar::GetLLVMType()
		{
			static llvm::StructType *llvmtype = llvm::StructType::create(LLVMCONTEXT, "LLVMRTTypeVar");
			static bool dontrepeat = true;
			if (dontrepeat)
			{
				dontrepeat = false;
				llvmtype->setBody(
					RTTypeHead::GetLLVMType(), //Header
					numtype(int), //Index
					RTTypeHead::GetLLVMType()->getPointerTo(), //Lower Bound
					RTTypeHead::GetLLVMType()->getPointerTo() //Upper Bound
				);
			}
			return llvmtype;
		}
		llvm::Constant * RTTypeVar::GetConstant(int index, llvm::Constant * lowerBound, llvm::Constant * upperBound, const NomTypeVar * type)
		{
			return llvm::ConstantStruct::get(GetLLVMType(), RTTypeHead::GetConstant(TypeKind::TKVariable, MakeInt(type->GetHashCode()), type, ConstantPointerNull::get(GetCastFunctionType()->getPointerTo())), MakeInt(index), lowerBound, upperBound);
		}
		int32_t RTTypeVar::HeadOffset()
		{
			static const int32_t offset = GetLLVMLayout()->getElementOffset((unsigned char)RTTypeVarFields::Head); return offset;
		}


		int32_t RTTypeVar::LowerBoundOffset()
		{
			static const int32_t offset = GetLLVMLayout()->getElementOffset((unsigned char)RTTypeVarFields::LowerBound); return offset;
		}
		int32_t RTTypeVar::UpperBoundOffset()
		{
			static const int32_t offset = GetLLVMLayout()->getElementOffset((unsigned char)RTTypeVarFields::UpperBound); return offset;
		}
		llvm::Value * RTTypeVar::FromHead(NomBuilder& builder, llvm::Value * head)
		{
			if ((unsigned char)RTTypeVarFields::Head == 0 || HeadOffset() == 0)
			{
				return builder->CreatePointerCast(head, GetLLVMType()->getPointerTo());
			}
			return builder->CreateIntToPtr(
				builder->CreateSub(builder->CreatePtrToInt(head, numtype(intptr_t)), MakeInt<intptr_t>(HeadOffset())),
				GetLLVMType()->getPointerTo()
			);
		}
		llvm::Value* RTTypeVar::GenerateLoadIndex(NomBuilder& builder, llvm::Value* type)
		{
			return MakeLoad(builder, type, GetLLVMType()->getPointerTo(), MakeInt32(RTTypeVarFields::Index), "typeVarIndex");
		}
	}
}