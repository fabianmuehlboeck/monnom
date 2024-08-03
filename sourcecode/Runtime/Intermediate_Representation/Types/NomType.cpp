#include "../../Intermediate_Representation/Types/NomType.h"
#include "../../Runtime_Data/Types/RTTypeHead.h"
#include "../../Intermediate_Representation/Types/NomTopType.h"
#include "../../Intermediate_Representation/Types/NomBottomType.h"
#include "../../Common/TypeList.h"
#include "../../Intermediate_Representation/Types/NomDynamicType.h"

namespace Nom
{
	namespace Runtime
	{


		const NomTopType * const NomType::Anything = NomTopType::Instance();
		const NomBottomType* const NomType::Nothing = NomBottomType::Instance();
		const NomDynamicType* const NomType::Dynamic = &NomDynamicType::Instance();
		NomTypeRef NomType::AnythingRef = NomType::Anything;
		NomTypeRef NomType::NothingRef = NomType::Nothing;
		NomTypeRef NomType::DynamicRef = NomType::Dynamic;

		NomTypeRef JoinTypes(NomTypeRef left, NomTypeRef right)
		{
			if (left->IsSubtype(right))
			{
				return right;
			}
			if (right->IsSubtype(left))
			{
				return left;
			}
			return NomType::AnythingRef;
		}

		bool NomType::PointwiseSubtype(TypeList l, TypeList r, bool optimistic)
		{
			size_t size = l.size();
			if (size != r.size())
			{
				return false;
			}
			for (int i = size - 1; i >= 0; i--)
			{
				if (!l[i]->IsSubtype(r[i], optimistic))
				{
					return false;
				}
			}
			return true;
		}

		NomType::NomType()
		{

		}


		NomType::~NomType()
		{
		}

	}
}
