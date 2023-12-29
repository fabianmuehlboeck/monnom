#pragma once
#include "NomTypeDecls.h"
PUSHDIAGSUPPRESSION
#include "llvm/IR/Value.h"
POPDIAGSUPPRESSION

namespace Nom
{
	namespace Runtime
	{
		template<typename T> class RTPWValue;
		class RTValue;
		class RTValuePtr
		{
		public:
			const RTValue* value;
			RTValuePtr(const RTValue* _value = nullptr) : value(_value)
			{

			}
			operator const RTValue* () const
			{
				return value;
			}
			const RTValue* operator->() const
			{
				return value;
			}
			operator llvm::Value* () const;
			NomTypeRef GetNomType() const;
			llvm::Type* getType() const;
			RTValuePtr Coalesce(RTValuePtr other) const;
			bool IsEmpty() const
			{
				return value == nullptr;
			}
		};

		template<typename T> class RTPWValuePtr : public RTValuePtr
		{
		public:
			RTPWValuePtr(const RTPWValue<T>* _value = nullptr) : RTValuePtr(_value)
			{

			}
			const RTPWValue<T>* operator->() const
			{
				return static_cast<const RTPWValue<T>*>(value);
			}
			operator RTPWValue<T>* () const
			{
				return static_cast<const RTPWValue<T>*>(value);
			}
		};
	}
}
