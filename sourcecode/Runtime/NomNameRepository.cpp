#include "NomNameRepository.h"

namespace Nom
{
	namespace Runtime
	{
		NomNameRepository::NomNameRepository()
		{
		}
		NomNameRepository& NomNameRepository::Instance()
		{
			static NomNameRepository nnr;
			return nnr;
		}
		NomNameRepository& NomNameRepository::ProfilingInstance()
		{
			static NomNameRepository nnr;
			return nnr;
		}
		NomNameRepository::~NomNameRepository()
		{
		}
		size_t NomNameRepository::GetNameID(const std::string& str)
		{
			auto result = names.find(str);
			if (result != names.end())
			{
				return result->second;
			}
			size_t nextid = names.size()+1;
			names.emplace(str, nextid);
			backlink.push_back(&(names.find(str)->first));
			return nextid;
		}
		size_t NomNameRepository::GetDispatchID(int32_t typeArgCount, int32_t argCount)
		{
			std::string name = "@@" + std::to_string(typeArgCount) + "^" + std::to_string(argCount) + "&";
			return GetNameID(name);
		}
		const std::string* NomNameRepository::GetNameFromID(size_t id)
		{
			return backlink[id-1];
		}
		size_t NomNameRepository::GetMaxID()
		{
			return names.size();
		}
	}
}