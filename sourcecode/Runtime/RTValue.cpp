#include "RTValue.h"
#include "RTRawBool.h"
#include "RTRawFloat.h"
#include "RTRawInt.h"
#include "RTObject.h"
#include "RTPackedValue.h"
#include "RTRefValue.h"
#include "RTLambdaValue.h"
#include "RTRecordValue.h"
#include "RTPartialAppValue.h"
#include "NomMaybeType.h"
#include "NomTypeVar.h"
#include "NomClassType.h"
#include "ObjectClass.h"
#include "instructions/CastInstruction.h"
#include "PWRecord.h"
#include "PWLambda.h"
#include "PWObject.h"
#include "PWFloat.h"
#include "PWInt.h"
#include "PWPartialApp.h"
#include "PWRefValue.h"
#include "PWStructVal.h"
#include "PWPacked.h"

namespace Nom
{
	namespace Runtime
	{
		RTValuePtr RTValue::GetValue(NomBuilder& builder, llvm::Value* value, NomTypeRef type, bool isfc)
		{
			if (value->getType()->isIntegerTy(1))
			{
				return RTRawBool::Get(builder, value, isfc);
			}
			if (value->getType()->isIntegerTy(64))
			{
				return RTRawInt::Get(builder, value, isfc);
			}
			if (value->getType()->isDoubleTy())
			{
				return RTRawFloat::Get(builder, value, isfc);
			}
			if (type == nullptr)
			{
				return RTPackedValue::Get(builder, value, NomType::DynamicRef, isfc);
			}
			if (type->GetKind() == TypeKind::TKLambda)
			{
				return RTLambdaValue::Get(builder, value, type, isfc);
			}
			if (type->GetKind() == TypeKind::TKRecord)
			{
				return RTRecordValue::Get(builder, value, type, isfc);
			}
			if (type->GetKind() == TypeKind::TKPartialApp)
			{
				return RTPartialAppValue::Get(builder, value, type, isfc);
			}
			if (GetIntClassType()->IsSubtype(type, true) || GetBoolClassType()->IsSubtype(type, true) || GetFloatClassType()->IsSubtype(type, true))
			{
				return RTPackedValue::Get(builder, value, type, isfc);
			}
			NomTypeRef ntr = type;
			while (ntr->GetKind() == TypeKind::TKMaybe || ntr->GetKind() == TypeKind::TKVariable)
			{
				if (ntr->GetKind() == TypeKind::TKMaybe)
				{
					NomMaybeTypeRef nmt = static_cast<NomMaybeTypeRef>(ntr);
					ntr = nmt->PotentialType;
				}
				else if (ntr->GetKind() == TypeKind::TKVariable)
				{
					NomTypeVarRef ntv = static_cast<NomTypeVarRef>(ntr);
					ntr = ntv->GetUpperBound();
				}
			}
			if (ntr->GetKind() == TypeKind::TKClass)
			{
				NomClassTypeRef nct = static_cast<NomClassTypeRef>(ntr);
				if (!nct->Named->IsInterface())
				{
					if (nct->Named != NomObjectClass::GetInstance())
					{
						return RTObject::Get(builder, value, type, isfc);
					}
				}
			}
			return RTRefValue::Get(builder, value, type, isfc);
		}
		RTValuePtr RTValue::ForLLVMType(NomBuilder& builder, llvm::Type* type, bool check) const
		{
			if (type->isIntegerTy(1))
			{
				return AsRawBool(builder, nullptr, check);
			}
			if (type->isIntegerTy(64))
			{
				return AsRawInt(builder, nullptr, check);
			}
			if (type->isDoubleTy())
			{
				return AsRawFloat(builder, nullptr, check);
			}
			return AsPackedValue(builder, nullptr);
		}

		RTValuePtr RTValue::ForNomType(NomBuilder& builder, NomTypeRef type, bool check) const
		{
			if (type->IsSubtype(GetIntClassType(), false))
			{
				return AsRawInt(builder, nullptr, check);
			}
			if (type->IsSubtype(GetBoolClassType(), false))
			{
				return AsRawBool(builder, nullptr, check);
			}
			if (type->IsSubtype(GetFloatClassType(), false))
			{
				return AsRawFloat(builder, nullptr, check);
			}
			if (GetIntClassType()->IsSubtype(type, true) || GetBoolClassType()->IsSubtype(type, true) || GetFloatClassType()->IsSubtype(type, true))
			{
				return AsPackedValue(builder, nullptr);
			}
			if (type->GetKind() == TypeKind::TKLambda)
			{
				return AsLambda(builder, nullptr, check);
			}
			if (type->GetKind() == TypeKind::TKPartialApp)
			{
				return AsPartialApp(builder, nullptr, check);
			}
			if (type->GetKind() == TypeKind::TKRecord)
			{
				return AsRecord(builder, nullptr, check);
			}
			if (type->GetKind() == TypeKind::TKDynamic)
			{
				return AsPackedValue(builder, nullptr);
			}
			NomTypeRef ntr = type;
			while (ntr->GetKind() == TypeKind::TKMaybe || ntr->GetKind() == TypeKind::TKVariable)
			{
				if (ntr->GetKind() == TypeKind::TKMaybe)
				{
					NomMaybeTypeRef nmt = static_cast<NomMaybeTypeRef>(ntr);
					ntr = nmt->PotentialType;
				}
				else if (ntr->GetKind() == TypeKind::TKVariable)
				{
					NomTypeVarRef ntv = static_cast<NomTypeVarRef>(ntr);
					ntr = ntv->GetUpperBound();
				}
			}
			if (ntr->GetKind() == TypeKind::TKClass)
			{
				NomClassTypeRef nct = static_cast<NomClassTypeRef>(ntr);
				if (!nct->Named->IsInterface())
				{
					if (nct->Named != NomObjectClass::GetInstance())
					{
						return AsObject(builder, nullptr, check);
					}
				}
			}
			return AsRefValue(builder, nullptr);
		}
		RTValuePtr EnsureRTValueType(NomBuilder& builder, CompileEnv *env, NomTypeRef targetType, llvm::Type* targetllvmtype, const RTValue * value)
		{
			if (targetType == GetBoolClassType())
			{
				if (targetllvmtype->isIntegerTy(1))
				{
					return value->AsRawBool(builder, nullptr, true);
				}
				else if (targetllvmtype == REFTYPE && value->GetNomType()->IsSubtype(GetBoolClassType()))
				{
					return value->AsPackedValue(builder);
				}
				else
				{
					RTValuePtr nv = CastInstruction::MakeCast(builder, env, value, targetType);
					return nv->AsPackedValue(builder);
				}
			}
			else if (targetType == GetIntClassType())
			{
				if (targetllvmtype->isIntegerTy(static_cast<unsigned int>(INTTYPE->getPrimitiveSizeInBits())))
				{
					return value->AsRawInt(builder, nullptr, true);
				}
				else if (targetllvmtype == REFTYPE && value->GetNomType()->IsSubtype(GetIntClassType()))
				{
					return value->AsPackedValue(builder);
				}
				else
				{
					RTValuePtr nv = CastInstruction::MakeCast(builder, env, value, targetType);
					return nv->AsPackedValue(builder);
				}
			}
			else if (targetType == GetFloatClassType())
			{
				if (targetllvmtype->isDoubleTy())
				{
					return value->AsRawFloat(builder, nullptr, true);
				}
				else if (targetllvmtype == REFTYPE && value->GetNomType()->IsSubtype(GetFloatClassType()))
				{
					return value->AsPackedValue(builder);
				}
				else
				{
					RTValuePtr nv = CastInstruction::MakeCast(builder, env, value, targetType);
					return nv->AsPackedValue(builder);
				}
			}
			else
			{
				RTValuePtr nv = CastInstruction::MakeCast(builder, env, value, targetType);
				return nv->AsPackedValue(builder);
			}
		}
	}
}
