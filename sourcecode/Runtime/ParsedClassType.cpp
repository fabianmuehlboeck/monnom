#include "ParsedArgument.h"

namespace Nom {
	namespace Runtime {
		ParsedClassType::ParsedClassType(std::string inputString, std::vector<ParsedTP*> substitutionParams) {
			int i = 0;
			int firstPos = -1, secondPos = -1;
			int openCount = 0;
			while (inputString[i] != '\0') {
				if (inputString[i] == '<') {
					openCount++;
					if (firstPos == -1) {
						firstPos = i;
					}
				}
				if (inputString[i] == '>') {
					openCount--;
					if (secondPos == -1 && openCount == 0) {
						secondPos = i;
					}
				}
				i++;
			}
			if (firstPos == -1) {
				if (!isTypeParameter(inputString, substitutionParams)) {
					std::string CName = inputString + "_" + std::to_string(0);
					className.assign(CName);
					typeArguments = {};
					return;
				}
				className.assign(inputString);
				typeArguments = {};
				return;
			}
			std::string fullString(inputString);
			std::string cName = fullString.substr(0, firstPos);
			std::string typeArgString = fullString.substr(firstPos + 1, secondPos - firstPos - 1);
			firstPos = -1;
			for (i = 0; i < typeArgString.size(); i++) {
				if (typeArgString[i] == ',') {
					std::string typeArg1 = typeArgString.substr(firstPos + 1, i - firstPos - 1);
					ParsedArgument* ta = new ParsedArgument(typeArg1,substitutionParams);
					typeArguments.push_back(ta);
					firstPos = i;
				}
			}
			std::string typeArg = typeArgString.substr(firstPos + 1, typeArgString.size());
			ParsedArgument* ta = new ParsedArgument(typeArg,substitutionParams);
			typeArguments.push_back(ta);
			if (!isTypeParameter(cName, substitutionParams)) {
				className = cName + "_" + std::to_string(typeArguments.size());
			}
			else {
				className = cName;
			}
		}
	}
}