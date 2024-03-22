#include "PWInt.h"
#include "llvm/IR/Constants.h"

namespace Nom
{
	namespace Runtime
	{
		PWBoolOrTri::PWBoolOrTri(llvm::Value* _wrapped) : PWrapper(_wrapped)
		{
			if (!(_wrapped->getType()->isIntegerTy(1) || _wrapped->getType()->isIntegerTy(2)))
			{
				throw new std::exception();
			}
		}
		PWBoolOrTri PWBoolOrTri::BoolTrue(NomBuilder& builder)
		{
			return PWBoolOrTri(PWBool(0, false));
		}
		PWBoolOrTri PWBoolOrTri::BoolFalse(NomBuilder& builder)
		{
			return PWBoolOrTri(PWCBool(0, false));
		}
		PWBoolOrTri PWBoolOrTri::TriTrue(NomBuilder& builder)
		{
			return PWBoolOrTri(PWCInt2(3, false));
		}
		PWBoolOrTri PWBoolOrTri::TriFalse(NomBuilder& builder)
		{
			return PWBoolOrTri(PWCInt2(2, false));
		}
		PWBoolOrTri PWBoolOrTri::TriOptTrue(NomBuilder& builder)
		{
			return PWBoolOrTri(PWCInt2(1, false));
		}
		bool PWBoolOrTri::IsBool() const
		{
			return wrapped->getType()->isIntegerTy(1);
		}
		bool PWBoolOrTri::IsTri() const
		{
			return wrapped->getType()->isIntegerTy(2);
		}
		PWBoolOrTri PWBoolOrTri::And(NomBuilder& builder, PWBoolOrTri other) const
		{
			if (IsBool()&&other.IsBool())
			{
				return builder->CreateAnd(wrapped, other);
			}
			if (IsBool() || other.IsBool())
			{
				return builder->CreateExtractElement(llvm::ConstantVector::get({ PWCInt2(2, false), PWCInt2(1, false), PWCInt2(2, false), PWCInt2(3, false) }), builder->CreateMul(builder->CreateZExt(wrapped, inttype(2)), builder->CreateZExt(other, inttype(2))));
			}
			else
			{
				return builder->CreateExtractElement(llvm::ConstantVector::get({ PWCInt2(2,false), PWCInt2(1,false), PWCInt2(2,false), PWCInt2(1, false), PWCInt2(2, false), PWCInt2(2, false), PWCInt2(2, false), PWCInt2(2, false), PWCInt2(2, false), PWCInt2(3, false) }),
					builder->CreateMul(builder->CreateZExt(wrapped, inttype(4)), builder->CreateZExt(other, inttype(4))));
			}
		}
		PWBoolOrTri PWBoolOrTri::Or(NomBuilder& builder, PWBoolOrTri other) const
		{
			if (IsBool() && other.IsBool())
			{
				return builder->CreateOr(wrapped, other);
			}
			if (IsBool() || other.IsBool())
			{
				return builder->CreateExtractElement(llvm::ConstantVector::get({ PWCInt2(2, false), PWCInt2(1, false), PWCInt2(1, false), PWCInt2(3, false) }), builder->CreateMul(builder->CreateZExt(wrapped, inttype(2)), builder->CreateZExt(other, inttype(2))));
			}
			else
			{
				return builder->CreateExtractElement(llvm::ConstantVector::get({ PWCInt2(2,false), PWCInt2(1,false), PWCInt2(1,false), PWCInt2(3, false), PWCInt2(2, false), PWCInt2(2, false), PWCInt2(3, false), PWCInt2(2, false), PWCInt2(2, false), PWCInt2(3, false) }),
					builder->CreateMul(builder->CreateZExt(wrapped, inttype(4)), builder->CreateZExt(other, inttype(4))));
			}
			
		}
		PWBoolOrTri PWBoolOrTri::Not(NomBuilder& builder) const
		{
			if (IsBool())
			{
				return builder->CreateNot(wrapped);
			}
			else
			{
				return builder->CreateOr(builder->CreateZExt(builder->CreateNot(builder->CreateTrunc(wrapped, inttype(1))), inttype(2)), PWCInt2(2, false));
			}
		}
		PWBool PWBoolOrTri::AsOptimisticBool(NomBuilder& builder) const
		{
			if (IsBool())
			{
				return wrapped;
			}
			return builder->CreateTrunc(wrapped, inttype(1));
		}
		PWBool PWBoolOrTri::AsPessimisticBool(NomBuilder& builder) const
		{
			if (IsBool())
			{
				return wrapped;
			}
			return builder->CreateAnd(builder->CreateTrunc(wrapped, inttype(1)), builder->CreateTrunc(builder->CreateLShr(wrapped, PWCInt32(1, false)), inttype(1)));
		}
	}
}
