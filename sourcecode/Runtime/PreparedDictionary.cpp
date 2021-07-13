#include "PreparedDictionary.h"
#include "NomJIT.h"
#include "llvm/Support/Error.h"
#include <iostream>
#include "NomNameRepository.h"
#include "RTConfig.h"

namespace Nom
{
	namespace Runtime
	{
		std::vector<PreparedDictionary*>& PreparedDictionary::inPreparation()
		{
			static std::vector<PreparedDictionary*> list; return list;
		}
		PreparedDictionary::PreparedDictionary(std::string& symbolName) : SymbolName(symbolName)
		{
		}
		PreparedDictionary::~PreparedDictionary()
		{
			entryKeys.clear();
		}
		void PreparedDictionary::LoadContents()
		{
			auto addr = (void**)NomJIT::Instance().lookup(SymbolName)->getAddress();
			for (auto key : entryKeys)
			{
				if (NomVerbose)
				{
					std::cout << "\n" << std::hex << ((intptr_t)this) << "(" << ((intptr_t)&(this->Dictionary)) << ")->";
					std::cout << std::dec << key;
					std::cout << " : " << *(NomNameRepository::Instance().GetNameFromID(key)) << ":" << SymbolName << ":";
					std::cout << std::hex << ((intptr_t)* addr);
					std::cout << "\n";
				}
				this->Dictionary[key]= *addr;
				addr++;
			}
			entryKeys.clear();
		}
		void PreparedDictionary::EnsurePreparation()
		{
			if (!InPreparation)
			{
				InPreparation = true;
				inPreparation().push_back(this);
			}
		}
		void PreparedDictionary::AddEntryKey(const DICTKEYTYPE& key)
		{
			entryKeys.push_back(key);
		}
		void PreparedDictionary::LoadDictionaryContents()
		{
			for (auto pd : inPreparation())
			{
				pd->LoadContents();
			}
			inPreparation().clear();
		}
	}
}