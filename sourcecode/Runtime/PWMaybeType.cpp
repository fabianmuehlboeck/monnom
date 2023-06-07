#include "PWTypeVar.h"
#include "CompileHelpers.h"
#include "RTMaybeType.h"
#include "PWMaybeType.h"

namespace Nom
{
	namespace Runtime
	{
		llvm::Type* PWMaybeType::GetLLVMType()
		{
			return RTMaybeType::GetLLVMType();
		}
		PWType PWMaybeType::ReadPotentialType(NomBuilder& builder)
		{
			return PWType(MakeLoad(builder, GetLLVMType(), wrapped, MakeInt<RTMaybeTypeFields>(RTMaybeTypeFields::PotentialType)));
		}
	}
}