#pragma once
#include <iostream>
#include "ParsedArgument.h"

namespace Nom {
	namespace Runtime {
		class ParsedMethodType
		{
		public:
			std::string methodName;
			std::vector<ParsedArgument*> typeArguments;
			std::vector<ParsedArgument*> arguments;
			std::vector<NomTypeRef> nomTypeArguments;
			std::vector<NomTypeRef> nomArgumentTypes;

			std::string getConstructorClassName() {
				return methodName + "_" + std::to_string(typeArguments.size());
			}
			NomClassTypeRef getConstructorClassType() {
				const NomString cName = NomString(getConstructorClassName());
				return NomClass::getClass(&cName)->GetType(getTypeArguments());
			}
			NomInstantiationRef<NomClass> getConstructorClassInstantiation() {
				const NomString cName = NomString(getConstructorClassName());
				NomInstantiationRef<NomClass> nir = NomInstantiationRef(NomClass::getClass(&cName), getTypeArguments());
				return nir;
			}
			TypeList getArgTypes() {
				if (nomArgumentTypes.size() == 0) {
					for (ParsedArgument* pa : arguments) {
						nomArgumentTypes.push_back(pa->getType());
					}
				}
				return TypeList(nomArgumentTypes);
			}

			 TypeList getTypeArguments() {
				 if (nomTypeArguments.size() == 0) {
					for (ParsedArgument* pa : typeArguments) {
						nomTypeArguments.push_back(pa->getType());
					}
				 }
				 return TypeList(nomTypeArguments);
			 }

			ParsedMethodType(std::string inputString, std::vector<ParsedTP*> substitutionParams) {
				int i = 0;
				int firstSquarePos = -1, secondSquarePos = -1;
				int firstBracketPos = -1, secondBracketPos = -1;
				int openSquareCount = 0;
				int openBracketCount = 0;
				while (inputString[i] != '\0') {
					if (inputString[i] == '<') {
						openSquareCount++;
						if (firstSquarePos == -1 && firstBracketPos == -1) {
							firstSquarePos = i;
						}
					}
					else if (inputString[i] == '>') {
						openSquareCount--;
						if (secondSquarePos == -1 && firstSquarePos != -1 && openSquareCount == 0) {
							secondSquarePos = i;
						}
					}
					else if (inputString[i] == '(') {
						openBracketCount++;
						if (firstBracketPos == -1) {
							firstBracketPos = i;
						}
					}
					else if (inputString[i] == ')') {
						openBracketCount--;
						if (secondBracketPos == -1 && openBracketCount == 0) {
							secondBracketPos = i;
						}
					}
					i++;
				}
				if (firstSquarePos == -1) {
					methodName = inputString.substr(0,firstBracketPos);
					typeArguments = {};
				}
				else {
					std::string fullString(inputString);
					methodName = fullString.substr(0, firstSquarePos);
					std::string typeArgString = fullString.substr(firstSquarePos + 1, secondSquarePos - firstSquarePos - 1);
					firstSquarePos = -1;
					for (i = 0; i < typeArgString.size(); i++) {
						if (typeArgString[i] == ',') {
							std::string typeArg1 = typeArgString.substr(firstSquarePos + 1, i - firstSquarePos - 1);
							ParsedArgument* ta = new ParsedArgument(typeArg1, substitutionParams);
							typeArguments.push_back(ta);
							firstSquarePos = i;
						}
					}
					std::string typeArg = typeArgString.substr(firstSquarePos + 1, typeArgString.size());
					ParsedArgument* ta = new ParsedArgument(typeArg, substitutionParams);
					typeArguments.push_back(ta);
				}
				if (firstBracketPos == -1) {
					arguments = {};
				}
				else {
					std::string fullString(inputString);
					std::string argString = fullString.substr(firstBracketPos + 1, secondBracketPos - firstBracketPos - 1);
					firstBracketPos = -1;
					if (argString == "") {
						arguments = {};
						return;
					}
					for (i = 0; i < argString.size(); i++) {
						if (argString[i] == ',') {
							std::string arg1 = argString.substr(firstBracketPos + 1, i - firstBracketPos - 1);
							ParsedArgument* ta = new ParsedArgument(arg1, substitutionParams);
							arguments.push_back(ta);
							firstBracketPos = i;
						}
					}
					std::string arg = argString.substr(firstBracketPos + 1, argString.size());
					ParsedArgument* ta = new ParsedArgument(arg, substitutionParams);
					arguments.push_back(ta);
				}
			}
		};
	}
}

