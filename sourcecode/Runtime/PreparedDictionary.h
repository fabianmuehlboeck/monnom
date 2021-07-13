#pragma once
#include <map>
#include "RTDictionary.h"
#include <vector>

namespace Nom
{
	namespace Runtime
	{
		class PreparedDictionary
		{
		private:
			std::vector<DICTKEYTYPE> entryKeys;
			static std::vector<PreparedDictionary*> &inPreparation();
			bool InPreparation = false;
		public:
			Dicttype Dictionary;
			const std::string SymbolName;
			PreparedDictionary(std::string &symbolName);
			~PreparedDictionary();
			void LoadContents();
			void EnsurePreparation();
			void AddEntryKey(const DICTKEYTYPE& key);
			static void LoadDictionaryContents();
		};
	}
}