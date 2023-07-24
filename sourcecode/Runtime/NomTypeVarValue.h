#pragma once
PUSHDIAGSUPPRESSION
#include "llvm/IR/Value.h"
POPDIAGSUPPRESSION
#include "NomTypeDecls.h"
#include "PWType.h"

namespace Nom
{
	namespace Runtime
	{
		class NomTypeVar;
		class NomTypeVarValue
		{
		private:
			llvm::Value *val;
			NomTypeVarRef var;
		public:
			NomTypeVarValue() : val(nullptr), var(nullptr) {}
			NomTypeVarValue(llvm::Value *value, NomTypeVarRef varref) : val(value), var(varref) {}

			llvm::Value *operator*() const
			{
				return val;
			}

			llvm::Value *operator->() const
			{
				return val;
			}

			NomTypeVarRef GetVariable() const
			{
				return var;
			}

			NomTypeRef UpperBound() const;
			NomTypeRef LowerBound() const;
			operator llvm::Value*() const { return val; }
			operator NomTypeVarRef() const { return var; }
			operator PWType() const { return val; }
		};
	}
}
