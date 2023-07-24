#include "PWType.h"
#include "PWCastFunction.h"
#include "PWNomType.h"
#include "RTTypeHead.h"
#include "CompileHelpers.h"

namespace Nom
{
	namespace Runtime
	{
		llvm::Type* PWType::GetLLVMType()
		{
			return RTTypeHead::GetLLVMType();
		}
		llvm::Type* PWType::GetWrappedLLVMType()
		{
			return NLLVMPointer(GetLLVMType());
		}
		void PWType::InitializeType(NomBuilder& builder, TypeKind kind, llvm::Value* hash, PWNomType nomtypeptr, PWCastFunction castFun)
		{
			MakeInvariantStore(builder, MakeInt(kind), RTTypeHead::GetLLVMType(), wrapped, MakeInt32(RTTypeHeadFields::Kind));
			MakeInvariantStore(builder, hash, RTTypeHead::GetLLVMType(), wrapped, MakeInt32(RTTypeHeadFields::Hash));
			MakeInvariantStore(builder, nomtypeptr, RTTypeHead::GetLLVMType(), wrapped, MakeInt32(RTTypeHeadFields::NomType));
			MakeInvariantStore(builder, castFun, RTTypeHead::GetLLVMType(), wrapped, MakeInt32(RTTypeHeadFields::CastFun));
		}
		llvm::Value* PWType::ReadKind(NomBuilder& builder)
		{
			return MakeInvariantLoad(builder, RTTypeHead::GetLLVMType(), wrapped, MakeInt32(RTTypeHeadFields::Kind), "typeKind");
		}
		llvm::Value* PWType::ReadHash(NomBuilder& builder)
		{
			return MakeInvariantLoad(builder, RTTypeHead::GetLLVMType(), wrapped, MakeInt32(RTTypeHeadFields::Hash), "typeHash");
		}
		PWCastFunction PWType::ReadCastFun(NomBuilder& builder)
		{
			return MakeInvariantLoad(builder, RTTypeHead::GetLLVMType(), wrapped, MakeInt32(RTTypeHeadFields::CastFun), "castfun");
		}
	}
}
