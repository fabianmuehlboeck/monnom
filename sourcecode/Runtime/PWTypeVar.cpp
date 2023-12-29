#include "PWTypeVar.h"
#include "CompileHelpers.h"
#include "XRTTypeVar.h"
#include "PWSubstStack.h"
#include "NomType.h"

namespace Nom
{
	namespace Runtime
	{
		llvm::Type* PWTypeVar::GetLLVMType()
		{
			return XRTTypeVar::GetLLVMType();
		}
		llvm::Type* PWTypeVar::GetWrappedLLVMType()
		{
			return NLLVMPointer(GetLLVMType());
		}
		PWInt32 PWTypeVar::ReadIndex(NomBuilder& builder)
		{
			return PWInt64(MakeInvariantLoad(builder, GetLLVMType(), wrapped, MakeInt32(XRTTypeVarFields::Index), "typeVarIndex")).Resize<32>(builder);
		}
		PWBool PWTypeVar::IsEq(NomBuilder& builder, PWType other, PWSubstStack ownSubst, PWSubstStack otherSubst)
		{
			auto varIndex = ReadIndex(builder);
			PWSubstStack newStack;
			PWType newType = ownSubst.Pop(builder, varIndex, &newStack);
			return newType.IsEq(builder, other, newStack, otherSubst);
		}
		PWBool PWTypeVar::IsEq(NomBuilder& builder, NomTypeRef other, PWSubstStack ownSubst, PWSubstStack otherSubst)
		{
			auto varIndex = ReadIndex(builder);
			PWSubstStack newStack;
			PWType newType = ownSubst.Pop(builder, varIndex, &newStack);
			return newType.IsEq(builder, other, newStack, otherSubst);
		}
		PWBool PWTypeVar::IsSubtype(NomBuilder& builder, PWType other, PWSubstStack ownSubst, PWSubstStack otherSubst, bool optimistic)
		{
			auto varIndex = ReadIndex(builder);
			PWSubstStack newStack;
			PWType newType = ownSubst.Pop(builder, varIndex, &newStack);
			return newType.IsSubtype(builder, other, newStack, otherSubst, optimistic);
		}
		PWBool PWTypeVar::IsSubtype(NomBuilder& builder, NomTypeRef other, PWSubstStack ownSubst, PWSubstStack otherSubst, bool optimistic)
		{
			auto varIndex = ReadIndex(builder);
			PWSubstStack newStack;
			PWType newType = ownSubst.Pop(builder, varIndex, &newStack);
			return newType.IsSubtype(builder, other, newStack, otherSubst, optimistic);
		}
		PWBool PWTypeVar::IsSupertype(NomBuilder& builder, PWType other, PWSubstStack ownSubst, PWSubstStack otherSubst, bool optimistic)
		{
			auto varIndex = ReadIndex(builder);
			PWSubstStack newStack;
			PWType newType = ownSubst.Pop(builder, varIndex, &newStack);
			return newType.IsSupertype(builder, other, newStack, otherSubst, optimistic);
		}
		PWBool PWTypeVar::IsSupertype(NomBuilder& builder, NomTypeRef other, PWSubstStack ownSubst, PWSubstStack otherSubst, bool optimistic)
		{
			auto varIndex = ReadIndex(builder);
			PWSubstStack newStack;
			PWType newType = ownSubst.Pop(builder, varIndex, &newStack);
			return newType.IsSupertype(builder, other, newStack, otherSubst, optimistic);
		}
	}
}
