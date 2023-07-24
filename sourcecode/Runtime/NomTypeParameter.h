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
			const size_t index;
			NomTypeParameter(const NomMemberContext* _parent, size_t _index) : variable(this), parent(_parent), index(_index)
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
			size_t GetDirectIndex() { return index; }
			size_t GetIndex() const;
			NomTypeVarRef GetVariable() const
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
			NomTypeParameterInternal(const NomMemberContext* _parent, size_t _index, NomTypeRef _upperBound, NomTypeRef _lowerBound) : NomTypeParameter(_parent, _index), upperBound(_upperBound), lowerBound(_lowerBound)
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
			NomTypeParameterLoaded(const NomMemberContext* _parent, size_t _index, ConstantID _upperBound, ConstantID _lowerBound);
			virtual ~NomTypeParameterLoaded() override = default;

			virtual NomTypeRef GetUpperBound() const override;
			virtual NomTypeRef GetLowerBound() const override;
			virtual size_t GetHashCode() const override
			{
				return this->GetIndex() + (static_cast<size_t>(lowerBoundID) * 31 + static_cast<size_t>(upperBoundID)) * 31;
			}
		};
	}
}
