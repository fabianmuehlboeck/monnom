#pragma once
#include "RTValuePtr.h"
#include "RTValueVisitor.h"
#include "NomTypeDecls.h"

namespace Nom
{
	namespace Runtime
	{
		class PWFloat;
		class PWPacked;
		class PWVMPtr;
		class PWObject;
		class PWLambda;
		class PWPartialApp;
		class PWRecord;
		class PWStructVal;
		class PWType;
		class CompileEnv;
		template <typename V> class RTPWValue;
		class RTValue
		{
		protected:
			RTValue() {}
		public:
			virtual ~RTValue() {}
			virtual NomTypeRef GetNomType() const = 0;
			virtual bool IsFunctionCall() const = 0;
			virtual const RTPWValuePtr<PWInt64> AsRawInt(NomBuilder& builder, RTValuePtr orig = nullptr, bool check = false) const = 0;
			virtual const RTPWValuePtr<PWFloat> AsRawFloat(NomBuilder& builder, RTValuePtr orig = nullptr, bool check = false) const = 0;
			virtual const RTPWValuePtr<PWBool> AsRawBool(NomBuilder& builder, RTValuePtr orig = nullptr, bool check = false) const = 0;
			virtual const RTPWValuePtr<PWRefValue> AsRefValue(NomBuilder& builder, RTValuePtr orig = nullptr) const = 0;
			virtual const RTPWValuePtr<PWPacked> AsPackedValue(NomBuilder& builder, RTValuePtr orig = nullptr) const = 0;
			virtual const RTPWValuePtr<PWVMPtr> AsVMPtr(NomBuilder& builder, RTValuePtr orig = nullptr) const = 0;
			virtual const RTPWValuePtr<PWObject> AsObject(NomBuilder& builder, RTValuePtr orig = nullptr, bool check = false) const = 0;
			virtual const RTPWValuePtr<PWLambda> AsLambda(NomBuilder& builder, RTValuePtr orig = nullptr, bool check = false) const = 0;
			virtual const RTPWValuePtr<PWPartialApp> AsPartialApp(NomBuilder& builder, RTValuePtr orig = nullptr, bool check = false) const = 0;
			virtual const RTPWValuePtr<PWRecord> AsRecord(NomBuilder& builder, RTValuePtr orig = nullptr, bool check = false) const = 0;
			virtual const RTPWValuePtr<PWStructVal> AsStructVal(NomBuilder& builder, RTValuePtr orig = nullptr, bool check = false) const = 0;

			static RTValuePtr GetValue(NomBuilder& builder, llvm::Value* value, NomTypeRef type = nullptr, bool isfc = false);
			virtual RTValuePtr ForNomType(NomBuilder& builder, NomTypeRef type, bool check = false) const;
			virtual RTValuePtr ForLLVMType(NomBuilder& builder, llvm::Type* type, bool check = false) const;
			virtual RTValuePtr EnsureType(NomBuilder& builder, CompileEnv* env, NomTypeRef nomtype, llvm::Type* llvmtype) const = 0;

			virtual operator llvm::Value* () const = 0;
			virtual llvm::Type* getType() const = 0;

			virtual void Visit(RTValueVisitor visitor) const = 0;

			virtual PWBool InstanceOf(NomBuilder& builder, CompileEnv* env, PWType type, bool tryCast = true, bool optimistic = false) const = 0;
			virtual PWBool InstanceOf(NomBuilder& builder, CompileEnv* env, NomTypeRef type, bool tryCast = true, bool optimistic = false) const = 0;

			virtual unsigned int GenerateRefOrPrimitiveValueSwitch([[maybe_unused]] NomBuilder& builder, [[maybe_unused]] std::function<void(NomBuilder&, RTPWValuePtr<PWRefValue>)> onRefValue, [[maybe_unused]] std::function<void(NomBuilder&, RTPWValuePtr<PWPacked>)> onPackedInt = nullptr, [[maybe_unused]] std::function<void(NomBuilder&, RTPWValuePtr<PWPacked>)> onPackedFloat = nullptr, [[maybe_unused]] std::function<void(NomBuilder&, RTPWValuePtr<PWInt64>)> onPrimitiveInt = nullptr, [[maybe_unused]] std::function<void(NomBuilder&, RTPWValuePtr<PWFloat>)> onPrimitiveFloat = nullptr, [[maybe_unused]] std::function<void(NomBuilder&, RTPWValuePtr<PWBool>)> onPrimitiveBool = nullptr, [[maybe_unused]] uint64_t refWeight = 100, [[maybe_unused]] uint64_t intWeight = 50, [[maybe_unused]] uint64_t floatWeight = 30) const { throw new std::exception(); }
			virtual unsigned int GenerateRefOrPrimitiveValueSwitchUnpackPrimitives([[maybe_unused]] NomBuilder& builder, [[maybe_unused]] std::function<void(NomBuilder&, RTPWValuePtr<PWRefValue>)> onRefValue, [[maybe_unused]] std::function<void(NomBuilder&, RTPWValuePtr<PWInt64>)> onPrimitiveInt = nullptr, [[maybe_unused]] std::function<void(NomBuilder&, RTPWValuePtr<PWFloat>)> onPrimitiveFloat = nullptr, [[maybe_unused]] std::function<void(NomBuilder&, RTPWValuePtr<PWBool>)> onPrimitiveBool = nullptr, [[maybe_unused]] bool unboxObjects = false, [[maybe_unused]] uint64_t refWeight = 100, [[maybe_unused]] uint64_t intWeight = 50, [[maybe_unused]] uint64_t floatWeight = 30, [[maybe_unused]] uint64_t boolWeight = 10) const { throw new std::exception(); }
		};

		RTValuePtr EnsureRTValueType(NomBuilder& builder, CompileEnv* env, NomTypeRef targetType, llvm::Type* targetllvmtype, const RTValue* value);

		template <typename V> class RTPWValue : public RTValue
		{
		private:
			const bool isfc;
		public:
			const V value;
		protected:
			RTPWValue(const V _val, bool isFunctionCall = false) : RTValue(), isfc(isFunctionCall), value(_val)
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
			virtual bool IsFunctionCall() const override
			{
				return isfc;
			}
			operator const V() const
			{
				return value;
			}
			virtual operator llvm::Value* () const override
			{
				return value.wrapped;
			}
			virtual llvm::Type* getType() const override
			{
				return value.wrapped->getType();
			}
			virtual RTValuePtr EnsureType(NomBuilder& builder, CompileEnv* env, NomTypeRef nomtype, llvm::Type* llvmtype) const override
			{
				return EnsureRTValueType(builder, env, nomtype, llvmtype, this);
			}
		};

		template <typename V, typename T> class RTPWTypedValue : public RTPWValue<V>
		{
		public:
			T const type;
		protected:
			RTPWTypedValue(const V _val, T _tp, bool isfc = false) : RTPWValue<V>(_val, isfc), type(_tp)
			{
			}
		public:
			virtual ~RTPWTypedValue() override
			{

			}
			virtual NomTypeRef GetNomType() const override
			{
				return type;
			}
		};

		template <typename V, typename T, typename C> class RTConvValue : public C
		{
		private:
			RTValuePtr parent;
		protected:
			RTConvValue(const V _val, T _tp, RTValuePtr _parent, bool isfc = false) : C(_val, _tp, isfc), parent(_parent)
			{

			}
		public:
			static const RTPWValuePtr<V> Get(NomBuilder& builder, const V _val, T _tp, RTValuePtr _parent)
			{
				return (new(builder.Malloc(sizeof(RTConvValue<V, T, C>))) RTConvValue<V, T, C>(_val, _tp, _parent));
			}
			virtual ~RTConvValue() override
			{

			}
			virtual const RTPWValuePtr<PWInt64> AsRawInt(NomBuilder& builder, RTValuePtr orig = nullptr, bool check = false) const override
			{
				if (std::is_same<V, PWInt64>::value)
				{
					return static_cast<const RTPWValue<PWInt64> *>(static_cast<const void *>(this));
				}
				else
				{
					return parent->AsRawInt(builder, orig.Coalesce(this), check);
				}
			}
			virtual const RTPWValuePtr<PWFloat> AsRawFloat(NomBuilder& builder, RTValuePtr orig = nullptr, bool check = false) const override
			{
				if (std::is_same<V, PWFloat>::value)
				{
					return static_cast<const RTPWValue<PWFloat> *>(static_cast<const void*>(this));
				}
				else
				{
					return parent->AsRawFloat(builder, orig.Coalesce(this), check);
				}
			}
			virtual const RTPWValuePtr<PWBool> AsRawBool(NomBuilder& builder, RTValuePtr orig = nullptr, bool check = false) const override
			{
				if (std::is_same<V, PWBool>::value)
				{
					return static_cast<const RTPWValue<PWBool> *>(static_cast<const void*>(this));
				}
				else
				{
					return parent->AsRawBool(builder, orig.Coalesce(this), check);
				}
			}
			virtual const RTPWValuePtr<PWRefValue> AsRefValue(NomBuilder& builder, RTValuePtr orig = nullptr) const override
			{
				if (std::is_same<V, PWRefValue>::value)
				{
					return static_cast<const RTPWValue<PWRefValue> *>(static_cast<const void*>(this));
				}
				else
				{
					return parent->AsRefValue(builder, orig.Coalesce(this));
				}
			}
			virtual const RTPWValuePtr<PWPacked> AsPackedValue(NomBuilder& builder, RTValuePtr orig = nullptr) const override
			{
				if (std::is_same<V, PWPacked>::value)
				{
					return static_cast<const RTPWValue<PWPacked> *>(static_cast<const void*>(this));
				}
				else
				{
					return parent->AsPackedValue(builder, orig.Coalesce(this));
				}
			}
			virtual const RTPWValuePtr<PWVMPtr> AsVMPtr(NomBuilder& builder, RTValuePtr orig = nullptr) const override
			{
				if (std::is_same<V, PWVMPtr>::value)
				{
					return static_cast<const RTPWValue<PWVMPtr> *>(static_cast<const void*>(this));
				}
				else
				{
					return parent->AsVMPtr(builder, orig.Coalesce(this));
				}
			}
			virtual const RTPWValuePtr<PWObject> AsObject(NomBuilder& builder, RTValuePtr orig = nullptr, bool check = false) const override
			{
				if (std::is_same<V, PWObject>::value)
				{
					return static_cast<const RTPWValue<PWObject> *>(static_cast<const void*>(this));
				}
				else
				{
					return parent->AsObject(builder, orig.Coalesce(this), check);
				}
			}
			virtual const RTPWValuePtr<PWLambda> AsLambda(NomBuilder& builder, RTValuePtr orig = nullptr, bool check = false) const override
			{
				if (std::is_same<V, PWLambda>::value)
				{
					return static_cast<const RTPWValue<PWLambda> *>(static_cast<const void*>(this));
				}
				else
				{
					return parent->AsLambda(builder, orig.Coalesce(this), check);
				}
			}
			virtual const RTPWValuePtr<PWPartialApp> AsPartialApp(NomBuilder& builder, RTValuePtr orig = nullptr, bool check = false) const override
			{
				if (std::is_same<V, PWPartialApp>::value)
				{
					return static_cast<const RTPWValue<PWPartialApp> *>(static_cast<const void*>(this));
				}
				else
				{
					return parent->AsPartialApp(builder, orig.Coalesce(this), check);
				}
			}
			virtual const RTPWValuePtr<PWRecord> AsRecord(NomBuilder& builder, RTValuePtr orig = nullptr, bool check = false) const override
			{
				if (std::is_same<V, PWRecord>::value)
				{
					return static_cast<const RTPWValue<PWRecord> *>(static_cast<const void*>(this));
				}
				else
				{
					return parent.AsRecord(builder, orig.Coalesce(this), check);
				}
			}
			virtual const RTPWValuePtr<PWStructVal> AsStructVal(NomBuilder& builder, RTValuePtr orig = nullptr, bool check = false) const override
			{
				if (std::is_same<V, PWStructVal>::value)
				{
					return static_cast<const RTPWValue<PWStructVal> *>(static_cast<const void*>(this));
				}
				else
				{
					return parent->AsStructVal(builder, orig.Coalesce(this), check);
				}
			}
		};
	}
}
