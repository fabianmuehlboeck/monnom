#include "RTValue.h"
#include "RTValuePtr.h"

namespace Nom
{
	namespace Runtime
	{
		RTValuePtr::operator llvm::Value* () const
		{
			return value->operator llvm::Value * ();
		}
		NomTypeRef RTValuePtr::GetNomType() const
		{
			return value->GetNomType();
		}
		llvm::Type* RTValuePtr::getType() const
		{
			return value->operator llvm::Value * ()->getType();
		}
		RTValuePtr RTValuePtr::Coalesce(RTValuePtr other) const
		{
			if (value == nullptr)
			{
				return other;
			}
			return *this;
		}
	}
}
