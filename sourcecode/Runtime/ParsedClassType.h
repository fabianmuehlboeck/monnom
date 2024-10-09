#pragma once
#include "NomType.h"
#include "NomClass.h"
#include <iostream>
#include <vector>

namespace Nom {
	namespace Runtime {
		class ParsedArgument;
		class ParsedClassType
		{
		public:
			std::string className;
			std::vector<ParsedArgument*> typeArguments;
			ParsedClassType(std::string inputString);
			
		};
	}
}

