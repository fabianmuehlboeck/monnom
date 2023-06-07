#include "PWType.h"
#include "PWCastFunction.h"
#include "PWNomType.h"
#include "RTTypeHead.h"
#include "CompileHelpers.h"

namespace Nom
{
	namespace Runtime
	{
		void PWType::InitializeType(NomBuilder& builder, TypeKind kind, llvm::Value* hash, PWNomType nomtypeptr, PWCastFunction castFun)
		{
			MakeInvariantStore(builder, MakeInt((unsigned char)kind), RTTypeHead::GetLLVMType(), wrapped, MakeInt32((unsigned char)RTTypeHeadFields::Kind));
			MakeInvariantStore(builder, hash, RTTypeHead::GetLLVMType(), wrapped, MakeInt32((unsigned char)RTTypeHeadFields::Hash));
			MakeInvariantStore(builder, nomtypeptr, RTTypeHead::GetLLVMType(), wrapped, MakeInt32((unsigned char)RTTypeHeadFields::NomType));
			MakeInvariantStore(builder, castFun, RTTypeHead::GetLLVMType(), wrapped, MakeInt32((unsigned char)RTTypeHeadFields::CastFun));
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