#include "PWSuperInstance.h"
#include "NomInterface.h"
#include "CompileHelpers.h"
#include "PWType.h"
#include "PWInterface.h"

namespace Nom
{
	namespace Runtime
	{
		llvm::Type* PWSuperInstance::GetLLVMType()
		{
			return SuperInstanceEntryType();
		}
		llvm::Type* PWSuperInstance::GetWrappedLLVMType()
		{
			return NLLVMPointer(GetLLVMType());
		}
		PWSuperInstance PWSuperInstance::GetEntry(NomBuilder& builder, int32_t index)
		{
			return GetEntry(builder, MakeInt32(index));
		}
		PWSuperInstance PWSuperInstance::GetEntry(NomBuilder& builder, llvm::Value* index)
		{
			return builder->CreateGEP(GetLLVMType(), wrapped, index);
		}
		PWTypeArr PWSuperInstance::GetTypeArgs(NomBuilder& builder, llvm::AtomicOrdering ordering)
		{
			return PWTypeArr(MakeInvariantLoad(builder, GetLLVMType(), wrapped, MakeInt32(SuperInstanceEntryFields::TypeArgs), "superTypeArgs", ordering));
		}
		PWInterface PWSuperInstance::GetInterface(NomBuilder& builder, llvm::AtomicOrdering ordering)
		{
			return PWInterface(MakeInvariantLoad(builder, GetLLVMType(), wrapped, MakeInt32(SuperInstanceEntryFields::Class), "superInterface", ordering));
		}
	}
}
