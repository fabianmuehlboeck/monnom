#pragma once
#include <iostream>
#include "NomTypeParameter.h"
namespace Nom {
	namespace Runtime {
		class ParsedTP
		{
		public:
			std::string ParamName;
			NomTypeParameterRef ParamRef;

			ParsedTP(std::string ParamName, int index) {
				this->ParamName = std::string(ParamName);
				ParamRef = new NomTypeParameterInternal(NULL, index, NomType::AnythingRef, NomType::NothingRef);
			}
		};
	}
}


