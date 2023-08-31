#include "NomType.h"
#include "RTTypeHead.h"
#include "NomTopType.h"
#include "NomBottomType.h"
#include "NomDynamicType.h"

#ifdef __clang__
#pragma clang diagnostic ignored "-Wglobal-constructors"
#elif defined(__GNU__)
#pragma GCC diagnostic ignored "-Wglobal-constructors"
#elif defined(_MSC_VER)

#endif

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
			for (size_t i = 0;i < size; i++)
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
