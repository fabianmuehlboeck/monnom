#pragma once
#include <string>
#include <vector>
#include <unordered_map>

namespace Nom
{
	namespace Runtime
	{
		class NomNameRepository
		{
		private:
			std::unordered_map<std::string, size_t> names;
			std::vector<const std::string*> backlink;
			NomNameRepository();
		public:
			static NomNameRepository& Instance();
			static NomNameRepository& ProfilingInstance();
			~NomNameRepository();
			size_t GetNameID(const std::string& str);
			size_t GetDispatchID(size_t typeArgCount, size_t argCount);
			const std::string* GetNameFromID(size_t id);
			size_t GetMaxID();
		};
	}
}
