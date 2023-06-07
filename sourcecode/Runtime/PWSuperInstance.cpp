#include "PWSuperInstance.h"
#include "NomInterface.h"
#include "CompileHelpers.h"

namespace Nom
{
	namespace Runtime
	{
		llvm::Type* PWSuperInstance::GetLLVMType()
		{
			return SuperInstanceEntryType();
		}
		PWSuperInstance PWSuperInstance::GetEntry(NomBuilder& builder, int32_t index)
		{
			return GetEntry(builder, MakeInt32(index));
		}
		PWSuperInstance PWSuperInstance::GetEntry(NomBuilder& builder, llvm::Value* index)
		{
			return builder->CreateGEP(GetLLVMType(), wrapped, index);
		}
	}
}