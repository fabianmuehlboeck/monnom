#include "NomValue.h"
#include "IntClass.h"
#include "FloatClass.h"
#include "NomClassType.h"
#include "BoolClass.h"
#include "NomDynamicType.h"

namespace Nom
{
	namespace Runtime
	{
		NomTypeRef GetIntClassType()
		{
			return NomIntClass::GetInstance()->GetType();
		}
		NomTypeRef GetFloatClassType()
		{
			return NomFloatClass::GetInstance()->GetType();
		}
		NomTypeRef GetBoolClassType()
		{
			return NomBoolClass::GetInstance()->GetType();
		}
		NomTypeRef GetDynamicType()
		{
			return NomDynamicType::DynamicRef;
		}
		NomTypeRef GetDynOrPrimitiveType(llvm::Value* val)
		{
			llvm::Type* valType = val->getType();
			if (valType->isIntegerTy(INTTYPE->getIntegerBitWidth()))
			{
				return GetIntClassType();
			}
			if (valType->isDoubleTy())
			{
				return GetFloatClassType();
			}
			if (valType->isIntegerTy(BOOLTYPE->getIntegerBitWidth()))
			{
				return GetBoolClassType();
			}
			return GetDynamicType();
		}
		void __NomValueContainerDebugCheck(NomTypeRef type, llvm::Type* valType)
		{
			if (type->IsSubtype(GetIntClassType()))
			{
				if (valType->isFloatingPointTy() || valType->isDoubleTy())
				{
					throw new std::exception();
				}
			}
			else if (type->IsSubtype(GetFloatClassType()))
			{
				if (valType->isIntegerTy(64))
				{
					throw new std::exception();
				}
			}
		}
	}
}