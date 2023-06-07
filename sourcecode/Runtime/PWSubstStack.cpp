#include "PWSubstStack.h"
#include "PWType.h"
#include "PWTypeVar.h"
#include "RTSubstStack.h"
#include "CompileHelpers.h"

namespace Nom
{
	namespace Runtime
	{
		llvm::Type* PWSubstStack::GetLLVMType()
		{
			return RTSubstStack::GetLLVMType();
		}
		PWType PWSubstStack::Pop(NomBuilder& builder, PWTypeVar var, PWSubstStack* newStackAddr)
		{
			if (newStackAddr != nullptr)
			{
				*newStackAddr = PWSubstStack(MakeInvariantLoad(builder, GetLLVMType(), wrapped, MakeInt32(TypeArgumentListStackFields::Next)));
			}
			auto typeList = MakeInvariantLoad(builder, GetLLVMType(), wrapped, MakeInt32(TypeArgumentListStackFields::Types));
			return MakeInvariantLoad(builder, TYPETYPE, builder->CreateGEP(TYPETYPE, typeList, builder->CreateSub(MakeInt32(-1), var.ReadIndex(builder))));
		}
	}
}