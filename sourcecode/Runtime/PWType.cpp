#include "PWType.h"
#include "PWCastFunction.h"
#include "PWNomType.h"
#include "RTTypeHead.h"
#include "CompileHelpers.h"
#include "PWSubstStack.h"
#include "NomType.h"
#include "RTTypeEq.h"

using namespace llvm;
using namespace std;
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
		PWBool PWType::IsEq(NomBuilder& builder, PWType other, PWSubstStack ownSubst, PWSubstStack otherSubst, bool optimistic)
		{

		}
		PWBool PWType::IsEq(NomBuilder& builder, NomTypeRef other, PWSubstStack ownSubst, PWSubstStack otherSubst, bool optimistic)
		{
			BasicBlock* varUnfoldBlock=BasicBlock::Create()
			auto kind = ReadKind(builder);
			auto isTypeVar = builder->CreateICmpEQ(kind, MakeIntLike(kind, static_cast<uint64_t>(TypeKind::TKVariable)));
			builder->CreateCondBr(isTypeVar, )
			switch (other->GetKind())
			{
				case TypeKind::TKClass:
				case TypeKind::TKInstance:
				{
					break;
				}
				case TypeKind::TKBottom:
				{
					break;
				}
				case TypeKind::TKTop:
				{
					break;
				}
				case TypeKind::TKLambda:
				case TypeKind::TKRecord:
				case TypeKind::TKPartialApp:
				case TypeKind::TKDynamic:
				{
					break;
				}
				case TypeKind::TKMaybe:
				{
					break;
				}
				case TypeKind::TKVariable:
				{
					break;
				}
			}
		}
	}
}
