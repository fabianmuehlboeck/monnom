#include "NomTypeParameter.h"
#include "NomType.h"
#include "NomConstants.h"
#include "NomMemberContext.h"

namespace Nom
{
	namespace Runtime
	{
		NomTypeParameterLoaded::NomTypeParameterLoaded(const NomMemberContext* _parent, size_t _index, ConstantID _upperBound, ConstantID _lowerBound) : NomTypeParameter(_parent,_index), upperBoundID(_upperBound), lowerBoundID(_lowerBound)
		{
		}
		NomTypeRef NomTypeParameterLoaded::GetUpperBound() const
		{
			if (upperBound == nullptr)
			{
				if (upperBoundID == 0)
				{
					upperBound = NomType::AnythingRef;
				}
				else
				{
					NomSubstitutionContextMemberContext nscmc(parent);
					upperBound = NomConstants::GetType(&nscmc, upperBoundID);
				}
			}
			return upperBound;
		}
		NomTypeRef NomTypeParameterLoaded::GetLowerBound() const
		{
			if (lowerBound == nullptr)
			{
				if (lowerBoundID == 0)
				{
					lowerBound = NomType::AnythingRef;
				}
				else
				{
					NomSubstitutionContextMemberContext nscmc(parent);
					lowerBound = NomConstants::GetType(&nscmc, lowerBoundID);
				}
			}
			return lowerBound;
		}
		size_t NomTypeParameter::GetIndex() const {
			if (parent == nullptr) { return index; }
			else { return index + parent->GetTypeParametersStart(); }
		}
	}
}
