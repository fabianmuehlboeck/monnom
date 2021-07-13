#include "NomDescriptor.h"
#include "NomAlloc.h"

namespace Nom
{
	namespace Runtime
	{
		NomDescriptor::NomDescriptor(std::string symbolName) : dictionary(new (gcalloc_uncollectable(sizeof(PreparedDictionary))) PreparedDictionary(symbolName))
		{
		}
		NomDescriptor::~NomDescriptor()
		{
		}
	}
}
