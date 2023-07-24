#pragma once
#include "PWrapper.h"
#include "NomBuilder.h"
PUSHDIAGSUPPRESSION
#include "llvm/IR/Constants.h"
#include "llvm/ADT/Twine.h"
POPDIAGSUPPRESSION
#include "Defs.h"

namespace Nom
{
	namespace Runtime
	{
		template <int S> class PWCInt;
		template <int S, typename V=llvm::Value*> class PWInt : public PWrapperT<V>
		{
		public:
			static llvm::Type* GetLLVMType()
			{
				return inttype(S);
			}
			static llvm::Type* GetWrappedLLVMType()
			{
				return inttype(S);
			}
			PWInt(int64_t value) : PWrapperT<V>(llvm::ConstantInt::get(inttype(S), bit_cast<uint64_t, int64_t>(value)))
			{

			}
			PWInt(uint64_t value, bool issigned=false) : PWrapperT<V>(llvm::ConstantInt::get(inttype(S), value, issigned))
			{

			}
			PWInt(V _wrapped) : PWrapperT<V>(_wrapped)
			{
				if (!_wrapped->getType()->isIntegerTy(S))
				{
					throw S;
				}
			}
			operator llvm::ArrayRef<llvm::Value*>()
			{
				return llvm::ArrayRef<llvm::Value*>(this->wrapped);
			}
			PWInt<S> Add(NomBuilder& builder, const PWInt<S> other, llvm::Twine name = llvm::Twine("sum")) const
			{
				return PWInt<S>(builder->CreateAdd(this->wrapped, other, name));
			}
			PWInt<S> Subtract(NomBuilder& builder, const PWInt<S> other, llvm::Twine name = llvm::Twine("diff")) const
			{
				return PWInt<S>(builder->CreateSub(this->wrapped, other, name));
			}
			PWInt<S> Mult(NomBuilder& builder, const PWInt<S> other, llvm::Twine name = llvm::Twine("prod")) const
			{
				return PWInt<S>(builder->CreateMul(this->wrapped, other, name));
			}
			PWInt<S> Add1(NomBuilder& builder, llvm::Twine name = llvm::Twine("inc")) const
			{
				return PWInt<S>(builder->CreateAdd(this->wrapped, MakeIntLike(this->wrapped, 1), name));
			}
			PWInt<S> Sub1(NomBuilder& builder, llvm::Twine name = llvm::Twine("dec")) const
			{
				return PWInt<S>(builder->CreateSub(this->wrapped, MakeIntLike(this->wrapped, 1), name));
			}
			PWInt<S> Mult(NomBuilder& builder, const PWInt<S> other) const
			{
				return PWInt<S>(builder->CreateMul(this->wrapped, other.wrapped));
			}
			PWInt<S> AShr(NomBuilder& builder, const PWInt<S> other) const
			{
				return PWInt<S>(builder->CreateAShr(this->wrapped, other.wrapped, "", false));
			}
			PWInt<S> Neg(NomBuilder& builder, llvm::Twine name = llvm::Twine("neg")) const
			{
				return PWInt<S>(builder->CreateNeg(this->wrapped, name));
			}
			template <int X> PWInt<X> Resize(NomBuilder& builder) const
			{
				if (X == S)
				{
					return PWInt<X>(this->wrapped);
				}
				return PWInt<X>(builder->CreateSExtOrTrunc(this->wrapped, inttype(X), "conv"));
			}
		};
		template <int S> class PWCInt : public PWInt<S, llvm::Constant*>
		{
		public:
			PWCInt(int64_t value) : PWInt<S,llvm::Constant*>(value) {}
			PWCInt(uint64_t value, bool issigned=false) : PWInt<S, llvm::Constant*>(value, issigned) {}
			PWCInt(llvm::Constant* _wrapped) : PWInt<S, llvm::Constant*>(_wrapped) {}
			operator llvm::ArrayRef<llvm::Constant*>()
			{
				return llvm::ArrayRef<llvm::Value*>(this->wrapped);
			}
			operator PWInt<S>()
			{
				return this->wrapped;
			}
			PWCInt<S> Add(const PWCInt<S> other) const
			{
				return PWCInt<S>(llvm::ConstantExpr::getAdd((this->wrapped), (other.wrapped)));
			}
			PWCInt<S> Subtract(const PWCInt<S> other) const
			{
				return PWCInt<S>(llvm::ConstantExpr::getSub((this->wrapped), (other.wrapped)));
			}
			PWCInt<S> Add1() const
			{
				return PWCInt<S>(llvm::ConstantExpr::getAdd((this->wrapped), MakeIntLike(this->wrapped, 1)));
			}
			PWCInt<S> Sub1() const
			{
				return PWCInt<S>(llvm::ConstantExpr::getSub((this->wrapped), MakeIntLike(this->wrapped, 1)));
			}
			PWCInt<S> Mult(const PWCInt<S> other) const
			{
				return PWCInt<S>(llvm::ConstantExpr::getMul((this->wrapped), (other.wrapped)));
			}
			PWCInt<S> AShr(const PWCInt<S> other) const
			{
				return PWCInt<S>(llvm::ConstantExpr::getAShr((this->wrapped), (other.wrapped), false));
			}
			PWCInt<S> Neg() const
			{
				return PWCInt<S>(llvm::ConstantExpr::getNeg((this->wrapped)));
			}
			template <int X> PWCInt<X> Resize() const
			{
				if (X == S)
				{
					return PWCInt<X>((this->wrapped));
				}
				return PWCInt<X>(llvm::ConstantExpr::getSExtOrTrunc((this->wrapped), inttype(X)));
			}
		};

		typedef PWInt<1> PWBool;
		typedef PWInt<2> PWInt2;
		typedef PWInt<3> PWInt3;
		typedef PWInt<4> PWInt4;
		typedef PWInt<8> PWInt8;
		typedef PWInt<16> PWInt16;
		typedef PWInt<32> PWInt32;
		typedef PWInt<62> PWInt62;
		typedef PWInt<64> PWInt64;
		typedef PWCInt<1> PWCBool;
		typedef PWCInt<2> PWCInt2;
		typedef PWCInt<3> PWCInt3;
		typedef PWCInt<4> PWCInt4;
		typedef PWCInt<8> PWCInt8;
		typedef PWCInt<16> PWCInt16;
		typedef PWCInt<32> PWCInt32;
		typedef PWCInt<62> PWCInt62;
		typedef PWCInt<64> PWCInt64;

		static inline PWCInt32 MakePWInt32(int64_t v) { return PWCInt32(v); }
		static inline PWCInt32 MakePWUInt32(uint64_t v) { return PWCInt32(v); }
		static inline PWCInt64 MakePWInt64(int64_t v) { return PWCInt64(v); }
		static inline PWCInt64 MakePWUInt64(uint64_t v) { return PWCInt64(v); }
	}
}
