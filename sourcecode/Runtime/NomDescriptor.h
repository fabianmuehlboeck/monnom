#pragma once
#include "PreparedDictionary.h"
#include <vector>

namespace Nom
{
	namespace Runtime
	{
		class NomDescriptor
		{
		protected:
			mutable PreparedDictionary * dictionary;
			//mutable std::vector<NomDescriptor*> childDescriptors;
		public:
			NomDescriptor(std::string symbolName);
			virtual ~NomDescriptor();

			//void InitializeDictionary() const
			//{
			//	dictionary->LoadContents();
			//	for (NomDescriptor* nd : childDescriptors)
			//	{
			//		nd->InitializeDictionary();
			//	}
			//	childDescriptors.clear();
			//}
		};
	}
}