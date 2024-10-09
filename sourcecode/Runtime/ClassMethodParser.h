#pragma once
#include <iostream>
#include "ParsedArgument.h"
#include "ParsedMethodType.h"
namespace Nom {
	namespace Runtime {
		class ClassMethodParser
		{
		public:
			ParsedClassType* classType;
			ParsedMethodType* methodType;
			std::vector<NomTypeRef> cmTypeArgs;

			ClassMethodParser(char inputString[], std::vector<ParsedTP*> substitutionParams) {
				int i = 0;
				while (inputString[i] != '\0') {
					if (inputString[i] == '.') {
						std::string fullString(inputString);
						std::string classString = fullString.substr(0, i);
						std::string methodString = fullString.substr(i+1, fullString.size());
						classType = new ParsedClassType(classString, substitutionParams);
						methodType = new ParsedMethodType(methodString, substitutionParams);
						return;
					}
					i++;
				}
				std::string methodString1 = std::string(inputString);
				methodType = new ParsedMethodType(methodString1,substitutionParams);
				return;
			}
			TypeList getTypeArgs() {
				if (cmTypeArgs.size() == 0) {
					for (ParsedType* pa : classType->typeArguments) {
						cmTypeArgs.push_back(pa->getType());
					}
					for (ParsedType* pa : methodType->typeArguments) {
						cmTypeArgs.push_back(pa->getType());
					}
				}
				return TypeList(cmTypeArgs);
			}
			NomInstantiationRef<const NomStaticMethod> getStaticMethodInstantiation() {
				const NomString mName = NomString(methodType->methodName);
				const NomString cName = NomString(classType->className);

				NomInstantiationRef<const NomStaticMethod> nir = NomClass::getClass(&cName)->GetStaticMethod(&mName, getTypeArgs(), methodType->getArgTypes());
				return nir;
			}
			NomInstantiationRef<const NomMethod> getInstanceMethodInstantiation() {
				const NomString mName = NomString(methodType->methodName);
				const NomString cName = NomString(classType->className);


				NomInterface* iface = NomInterface::GetInterface(&cName);

				NomSubstitutionContextList nscl1(classType->getTypeArguments());
				NomInstantiationRef<const NomMethod> method = iface->GetMethod(&nscl1, &mName, methodType->getTypeArguments(), methodType->getArgTypes());
				return method;
			}
		};
	}
}

