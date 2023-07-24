#pragma once
#include "NomType.h"
#include "PWInt.h"
#include "PWFloat.h"
#include "PWRefValue.h"
#include "PWVMPtr.h"
#include "PWObject.h"
#include "PWLambda.h"
#include "PWRecord.h"
#include "PWPartialApp.h"
#include "PWStructVal.h"

namespace Nom
{
	namespace Runtime
	{
		class RTValue;
		using RTValueRef = const RTValue&;
		using RTValuePtr = const RTValue*;
		template <typename V> class RTPWValue;
		class RTValue
		{
		protected:
			RTValue() {}
		public:
			virtual ~RTValue() {}
			virtual NomTypeRef GetType() const = 0;
			virtual const RTPWValue<PWInt64>* AsRawInt(NomBuilder& builder, RTValuePtr orig = nullptr, bool check = false) const = 0;
			virtual const RTPWValue<PWFloat>* AsRawFloat(NomBuilder& builder, RTValuePtr orig = nullptr, bool check = false) const = 0;
			virtual const RTPWValue<PWBool>* AsRawBool(NomBuilder& builder, RTValuePtr orig = nullptr, bool check = false) const = 0;
			virtual const RTPWValue<PWRefValue>* AsRefValue(NomBuilder& builder, RTValuePtr orig = nullptr) const = 0;
			virtual const RTPWValue<PWVMPtr>* AsVMPtr(NomBuilder& builder, RTValuePtr orig = nullptr) const = 0;
			virtual const RTPWValue<PWObject>* AsObject(NomBuilder& builder, RTValuePtr orig = nullptr, bool check = false) const = 0;
			virtual const RTPWValue<PWLambda>* AsLambda(NomBuilder& builder, RTValuePtr orig = nullptr, bool check = false) const = 0;
			virtual const RTPWValue<PWPartialApp>* AsPartialApp(NomBuilder& builder, RTValuePtr orig = nullptr, bool check = false) const = 0;
			virtual const RTPWValue<PWRecord>* AsRecord(NomBuilder& builder, RTValuePtr orig = nullptr, bool check = false) const = 0;
			virtual const RTPWValue<PWStructVal>* AsStructVal(NomBuilder& builder, RTValuePtr orig = nullptr, bool check = false) const = 0;
		};

		template <typename V> class RTPWValue : public RTValue
		{
		public:
			const V const value;
		protected:
			RTPWValue(const V _val) : RTValue(), value(_val)
			{

			}
		public:
			virtual ~RTPWValue() override
			{

			}
			const V operator->() const
			{
				return value;
			}
			operator const V() const
			{
				return value;
			}
		};

		template <typename V, typename T> class RTPWTypedValue : public RTPWValue<V>
		{
		public:
			T const type;
		protected:
			RTPWTypedValue(const V _val, T _tp) : RTPWValue(_val), type(_tp)
			{
			}
		public:
			virtual ~RTPWTypedValue() override
			{

			}
			virtual NomTypeRef GetType() const override
			{
				return type;
			}
		};

		template <typename V, typename T, typename C> class RTConvValue : public C
		{
		private:
			RTValuePtr parent;
		protected:
			RTConvValue(const V _val, T _tp, RTValuePtr _parent) : C(_val, _tp), parent(_parent)
			{

			}
		public:
			static const RTConvValue<V, T, C>* Get(NomBuilder& builder, const V _val, T _tp, RTValuePtr _parent)
			{
				return (new(NomBuilder.Malloc(sizeof(RTConvValue<V, T, C>))) RTConvValue<V, T, C>(_val, _tp, _parent));
			}
			virtual ~RTConvValue() override
			{

			}
			virtual const RTPWValue<PWInt64>* AsRawInt(NomBuilder& builder, RTValuePtr orig = nullptr, bool check = false) const override
			{
				if (std::is_same<V, PWInt64>::value)
				{
					return this;
				}
				else
				{
					return parent.AsRawInt(builder, orig == nullptr ? this : orig, check);
				}
			}
			virtual const RTPWValue<PWFloat>* AsRawFloat(NomBuilder& builder, RTValuePtr orig = nullptr, bool check = false) const override
			{
				if (std::is_same<V, PWFloat>::value)
				{
					return this;
				}
				else
				{
					return parent.AsRawFloat(builder, orig == nullptr ? this : orig, check);
				}
			}
			virtual const RTPWValue<PWBool>* AsRawBool(NomBuilder& builder, RTValuePtr orig = nullptr, bool check = false) const override
			{
				if (std::is_same<V, PWBool>::value)
				{
					return this;
				}
				else
				{
					return parent.AsRawBool(builder, orig == nullptr ? this : orig, check);
				}
			}
			virtual const RTPWValue<PWRefValue>* AsRefValue(NomBuilder& builder, RTValuePtr orig = nullptr) const override
			{
				if (std::is_same<V, PWRefValue>::value)
				{
					return this;
				}
				else
				{
					return parent.AsRefValue(builder, orig == nullptr ? this : orig);
				}
			}
			virtual const RTPWValue<PWVMPtr>* AsVMPtr(NomBuilder& builder, RTValuePtr orig = nullptr) const override
			{
				if (std::is_same<V, PWVMPtr>::value)
				{
					return this;
				}
				else
				{
					return parent.AsVMPtr(builder, orig == nullptr ? this : orig);
				}
			}
			virtual const RTPWValue<PWObject>* AsObject(NomBuilder& builder, RTValuePtr orig = nullptr, bool check = false) const override
			{
				if (std::is_same<V, PWObject>::value)
				{
					return this;
				}
				else
				{
					return parent.AsObject(builder, orig == nullptr ? this : orig, check);
				}
			}
			virtual const RTPWValue<PWLambda>* AsLambda(NomBuilder& builder, RTValuePtr orig = nullptr, bool check = false) const override
			{
				if (std::is_same<V, PWLambda>::value)
				{
					return this;
				}
				else
				{
					return parent.AsLambda(builder, orig == nullptr ? this : orig, check);
				}
			}
			virtual const RTPWValue<PWPartialApp>* AsPartialApp(NomBuilder& builder, RTValuePtr orig = nullptr, bool check = false) const override
			{
				if (std::is_same<V, PWPartialApp>::value)
				{
					return this;
				}
				else
				{
					return parent.AsPartialApp(builder, orig == nullptr ? this : orig, check);
				}
			}
			virtual const RTPWValue<PWRecord>* AsRecord(NomBuilder& builder, RTValuePtr orig = nullptr, bool check = false) const override
			{
				if (std::is_same<V, PWRecord>::value)
				{
					return this;
				}
				else
				{
					return parent.AsRecord(builder, orig == nullptr ? this : orig, check);
				}
			}
			virtual const RTPWValue<PWStructVal>* AsStructVal(NomBuilder& builder, RTValuePtr orig = nullptr, bool check = false) const override
			{
				if (std::is_same<V, PWStructVal>::value)
				{
					return this;
				}
				else
				{
					return parent.AsStructVal(builder, orig == nullptr ? this : orig, check);
				}
			}
		};
	}
}
