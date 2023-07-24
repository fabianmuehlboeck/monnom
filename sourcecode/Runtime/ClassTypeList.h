#pragma once
PUSHDIAGSUPPRESSION
#include "llvm/ADT/TinyPtrVector.h"
POPDIAGSUPPRESSION

#include <vector>
#include "NomType.h"

namespace Nom
{
	namespace Runtime
	{
		class NomClassType;

		class ClassTypeList
		{
		private:
			std::vector<NomClassTypeRef> types;
		public:
			std::vector<NomClassTypeRef> * operator->()
			{
				return &types;
			}
			const std::vector<NomClassTypeRef> * operator->() const
			{
				return &types;
			}

			std::vector<NomClassTypeRef> &operator*()
			{
				return types;
			}

			ClassTypeList()
			{
			}
			ClassTypeList([[maybe_unused]]  int defaultFields)
			{

			}

			~ClassTypeList()
			{

			}
		};
	}
}
