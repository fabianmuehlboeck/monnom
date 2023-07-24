#include "RTTypeVar.h"
#include "NomTypeVar.h"
#include "CompileHelpers.h"
PUSHDIAGSUPPRESSION
#include "llvm/IR/Constant.h"
POPDIAGSUPPRESSION
#include "PWTypeVar.h"

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
					numtype(size_t), //Index
					RTTypeHead::GetLLVMType()->getPointerTo(), //Lower Bound
					RTTypeHead::GetLLVMType()->getPointerTo() //Upper Bound
				);
			}
			return llvmtype;
		}
		llvm::Constant * RTTypeVar::GetConstant(size_t index, llvm::Constant * lowerBound, llvm::Constant * upperBound, const NomTypeVar * type)
		{
			return llvm::ConstantStruct::get(GetLLVMType(), RTTypeHead::GetConstant(TypeKind::TKVariable, MakeInt(type->GetHashCode()), type, ConstantPointerNull::get(GetCastFunctionType()->getPointerTo())), MakeInt(index), lowerBound, upperBound);
		}

		llvm::Value* RTTypeVar::GenerateLoadIndex(NomBuilder& builder, llvm::Value* type)
		{
			return PWTypeVar(type).ReadIndex(builder);
		}
	}
}
