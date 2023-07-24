#include "PWTypeVar.h"
#include "CompileHelpers.h"
#include "RTClassType.h"
#include "PWClassType.h"
#include "PWInterface.h"

namespace Nom
{
	namespace Runtime
	{
		llvm::Type* PWClassType::GetLLVMType()
		{
			return RTClassType::GetLLVMType();
		}
		llvm::Type* PWClassType::GetWrappedLLVMType()
		{
			return NLLVMPointer(GetLLVMType());
		}
		PWInterface PWClassType::ReadClassDescriptorLink(NomBuilder& builder)
		{
			return MakeInvariantLoad(builder, GetLLVMType(), wrapped, MakeInt32(RTClassTypeFields::Class), "class");
		}
		PWTypeArr PWClassType::TypeArgumentsPointer(NomBuilder& builder)
		{
			return builder->CreateGEP(GetLLVMType(), wrapped, { MakeInt32(0), MakeInt32(RTClassTypeFields::TypeArgs), MakeInt32(0) }, "typeArgs");
		}
	}
}
