#include "RTMaybeType.h"
#include "RTTypeHead.h"
#include "llvm/IR/Constant.h"
#include "Defs.h"
#include "CompileHelpers.h"

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
		llvm::Constant* RTMaybeType::GetConstant(llvm::Module& mod, const NomMaybeType* mbt)
		{
			return ConstantStruct::get(GetLLVMType(), RTTypeHead::GetConstant(TypeKind::TKMaybe, MakeInt<size_t>(mbt->GetHashCode()), mbt), mbt->PotentialType->GetLLVMElement(mod));
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