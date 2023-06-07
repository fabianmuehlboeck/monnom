#pragma once
#include "PWrapper.h"
#include "NomBuilder.h"
#include "llvm/IR/Constants.h"

namespace Nom
{
	namespace Runtime
	{

		template <int S> class PWInt : public PWrapper
		{
		public:
			PWInt(int64_t value) : PWrapper(llvm::ConstantInt::get(inttype(S), value))
			{

			}
			PWInt(uint64_t value, bool issigned) : PWrapper(llvm::ConstantInt::get(inttype(S), value, issigned))
			{

			}
			PWInt(llvm::Value* wrapped) : PWrapper(wrapped)
			{
				if (!wrapped->getType()->isIntegerTy(S))
				{
					throw S;
				}
			}
			template <int X> PWInt<X> Resize(NomBuilder& builder)
			{
				return PWInt<X>(builder->CreateSExtOrTrunc(wrapped, inttype(X), "conv"));
			}
			PWInt<S> Add(NomBuilder& builder, PWInt<S> other, llvm::Twine& name = "sum")
			{
				return PWInt<S>(builder->CreateAdd(wrapped, other, name))
			}
			PWInt<S> Subtract(NomBuilder& builder, PWInt<S> other, llvm::Twine& name = "diff")
			{
				return PWInt<S>(builder->CreateSub(wrapped, other, name))
			}
			PWInt<S> Add1(NomBuilder& builder, llvm::Twine& name = "inc")
			{
				return PWInt<S>(builder->CreateAdd(wrapped, MakeIntLike(wrapped, 1), name));
			}
			PWInt<S> Sub1(NomBuilder& builder, llvm::Twine& name = "dec")
			{
				return PWInt<S>(builder->CreateSun(wrapped, MakeIntLike(wrapped, 1), name));
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
	}
}