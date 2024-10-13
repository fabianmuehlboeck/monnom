#pragma once
#include <iostream>
#include "ParsedArgument.h"
#include "ParsedMethodType.h"
namespace Nom {
	namespace Runtime {
		class ClassMethodParser
		{
		private:
			ParsedClassType* classType;
			ParsedMethodType* methodType;
			std::vector<NomTypeRef> cmTypeArgs;
		public:
			/*
			Parses the calling class and method that is called from an input string, performs
			environment type argument substitutions. 
			*/
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
				classType = nullptr;
				return;
			}
			/*
			Gets the combined type arguments of the class and method. Useful for static method calls.
			*/
			TypeList getTypeArgs() {
				if (cmTypeArgs.size() == 0) {
					for (ParsedType* pa : classType->getTypeArgsVector()) {
						cmTypeArgs.push_back(pa->getType());
					}
					for (ParsedType* pa : methodType->getTypeArgsVector()) {
						cmTypeArgs.push_back(pa->getType());
					}
				}
				return TypeList(cmTypeArgs);
			}
			TypeList getArgTypes() {
				return methodType->getArgTypes();
			}

			/*
			Gets the instantiation of a NomStaticMethod using the contained class and method
			*/
			NomInstantiationRef<const NomStaticMethod> getStaticMethodInstantiation() {
				const NomString mName = NomString(methodType->getMethodName());
				const NomString cName = NomString(classType->getClassName());

				NomInstantiationRef<const NomStaticMethod> nir = NomClass::getClass(&cName)->GetStaticMethod(&mName, getTypeArgs(), methodType->getArgTypes());
				return nir;
			}

			/*
			Gets the instantiation of a Nom Instance Method using the contained class and method.
			If the method is unnamed, then an empty named method will be instantiated, i.e. a raw invoke.
			*/
			NomInstantiationRef<const NomMethod> getInstanceMethodInstantiation() {
				if (classType != nullptr) {
					const NomString mName = NomString(methodType->getMethodName());
					const NomString cName = NomString(classType->getClassName());


					NomInterface* iface = NomInterface::GetInterface(&cName);

					NomSubstitutionContextList nscl1(classType->getTypeArgs());
					NomInstantiationRef<const NomMethod> method = iface->GetMethod(&nscl1, &mName, methodType->getTypeArgs(), methodType->getArgTypes());
					return method;
				}
				else {
					const NomString cName = NomString(methodType->getClassName());
					const NomString mName = NomString("");
					NomInterface* iface = NomInterface::GetInterface(&cName);
					NomSubstitutionContextList nscl1(methodType->getTypeArgs());
					NomInstantiationRef<const NomMethod> method = iface->GetMethod(&nscl1, &mName, methodType->getTypeArgs(), methodType->getArgTypes());
					return method;
				}
			}		
			/*
			Gets the instantiation of a Nom Class, used for constructor calls.
			*/
			NomInstantiationRef<NomClass> getConstructorClassInstantiation() {
				const NomString cName = NomString(methodType->getClassName());
				NomInstantiationRef<NomClass> nir = NomInstantiationRef(NomClass::getClass(&cName), methodType->getTypeArgs());
				return nir;
			}
			/*
			Gets the type of the class that is calling the method. Useful for finding the receiver type.
			*/
			NomClassTypeRef getClassType() {
				if (classType != nullptr) {
					const NomString cName = NomString(classType->getClassName());
					return NomClass::getClass(&cName)->GetType(classType->getTypeArgs());
				}
				else {
					const NomString cName = NomString(methodType->getClassName());
					return NomClass::getClass(&cName)->GetType(methodType->getTypeArgs());
				}
			}
		};
	}
}

