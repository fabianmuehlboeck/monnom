#pragma once
#include "NomTypeDecls.h"
#include "Defs.h"
#include "NomTypeVar.h"

namespace Nom
{
	namespace Runtime
	{
		class NomMemberContext;
		class NomTypeParameter
		{
		private:
			const NomTypeVar variable;
		protected:
			const NomMemberContext* const parent;
			const int index;
			NomTypeParameter(const NomMemberContext* parent, int index) : variable(this), parent(parent), index(index)
			{
			}
		public:
			NomTypeParameter(NomTypeParameter& p) = delete;
			NomTypeParameter(const NomTypeParameter& p) = delete;
			NomTypeParameter(NomTypeParameter&& p) = delete;
			NomTypeParameter(const NomTypeParameter&& p) = delete;

			virtual ~NomTypeParameter() = default;

			virtual NomTypeRef GetUpperBound() const = 0;
			virtual NomTypeRef GetLowerBound() const = 0;
			virtual size_t GetHashCode() const = 0;

			const NomMemberContext* GetParent() const { return parent; }
			int GetDirectIndex() { return index; }
			int GetIndex() const;
			const NomTypeVarRef GetVariable() const
			{
				return &variable;
			}
		};

		class NomTypeParameterInternal : public NomTypeParameter
		{
		private:
			NomTypeRef upperBound;
			NomTypeRef lowerBound;
		public:
			NomTypeParameterInternal(const NomMemberContext* parent, int index, NomTypeRef upperBound, NomTypeRef lowerBound) : NomTypeParameter(parent, index), upperBound(upperBound), lowerBound(lowerBound)
			{

			}
			virtual ~NomTypeParameterInternal() override = default;
			virtual NomTypeRef GetUpperBound() const override
			{
				return upperBound;
			}
			virtual NomTypeRef GetLowerBound() const override
			{
				return lowerBound;
			}
			virtual size_t GetHashCode() const override
			{
				return this->GetIndex() + (this->GetUpperBound()->GetHashCode() * 31 + this->GetLowerBound()->GetHashCode()) * 31;
			}
		};

		class NomTypeParameterLoaded : public NomTypeParameter
		{
		private:
			ConstantID upperBoundID;
			ConstantID lowerBoundID;
			mutable NomTypeRef upperBound = nullptr;
			mutable NomTypeRef lowerBound = nullptr;
		public:
			NomTypeParameterLoaded(const NomMemberContext* parent, int index, ConstantID upperBound, ConstantID lowerBound);
			virtual ~NomTypeParameterLoaded() override = default;

			virtual NomTypeRef GetUpperBound() const override;
			virtual NomTypeRef GetLowerBound() const override;
			virtual size_t GetHashCode() const override
			{
				return this->GetIndex() + ((size_t)lowerBoundID * 31 + (size_t)upperBoundID) * 31;
			}
		};
	}
}