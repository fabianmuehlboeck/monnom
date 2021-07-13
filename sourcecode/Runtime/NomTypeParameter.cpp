#include "NomTypeParameter.h"
#include "NomType.h"
#include "NomConstants.h"
#include "NomMemberContext.h"

namespace Nom
{
	namespace Runtime
	{
		NomTypeParameterLoaded::NomTypeParameterLoaded(const NomMemberContext* parent, int index, ConstantID upperBound, ConstantID lowerBound) : NomTypeParameter(parent,index), upperBoundID(upperBound), lowerBoundID(lowerBound)
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
		int NomTypeParameter::GetIndex() const {
			if (parent == nullptr) { return index; }
			else { return index + parent->GetTypeParametersStart(); }
		}
	}
}