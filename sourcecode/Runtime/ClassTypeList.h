#pragma once

#include <vector>
#include "NomType.h"
#include "llvm/ADT/TinyPtrVector.h"

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
			ClassTypeList(int defaultFields)
			{

			}

			~ClassTypeList()
			{

			}
		};
	}
}