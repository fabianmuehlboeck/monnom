#pragma once
#include "ParsedTP.h"
#include "NomType.h"
#include "NomClass.h"

namespace Nom {
	namespace Runtime {
		/*
		The superclass of ParsedClassType and Parsed argument, has a single virtual method
		that gets the NomTypeRef corresponding to that class. 
		*/
		class ParsedType {
		public:
			virtual NomTypeRef getType() { return nullptr; }
		};

		class ParsedArgument;
		class ParsedClassType : public ParsedType
		{
		private:
			std::string className;
			std::vector<ParsedType*> typeArguments;
			std::vector<NomTypeRef> nomTypeArguments;
		public:
			ParsedClassType(std::string inputString, std::vector<ParsedTP*> substitutionParams);
			NomTypeRef getType() override {
				const NomString cName = NomString(className);
				return (NomTypeRef)NomClass::getClass(&cName)->GetType(getTypeArgs());
			}
			std::string getClassName() { return className; }
			TypeList getTypeArgs() {
				if (nomTypeArguments.size() == 0) {
					for (ParsedType* pa : typeArguments) {
						nomTypeArguments.push_back(pa->getType());
					}
				}
				return TypeList(nomTypeArguments);
			}
			std::vector<ParsedType*> getTypeArgsVector() {
				return typeArguments;
			}
			bool isTypeParameter(std::string inputString, std::vector<ParsedTP*> substitutionParams) {
				for (ParsedTP* tp : substitutionParams) {
					if (tp->ParamName == inputString) {
						return true;
					}
				}
				return false;
			}
		};
		class ParsedArgument : public ParsedType
		{
		private:
			ParsedTP* typeParam = nullptr;
			ParsedClassType* classArg = nullptr;
		public:
			/*
			The parsed argument can either be a type parameter, or a class, which can either be a
			Nom Internal Type (e.g. Int), a Nom Loaded class, which can itself include type arguments. 
			*/
			ParsedArgument(std::string argString, std::vector<ParsedTP*> substitutionParams) {
				for (ParsedTP* tp : substitutionParams) {
					if (tp->ParamName == argString) {
						typeParam = tp;
						return;
					}
				}
				classArg = new ParsedClassType(argString,substitutionParams);
			}

			NomTypeRef getType() override {
				if (classArg == nullptr) {
					return typeParam->ParamRef->GetVariable();
				}
				else {
					return classArg->getType();
				}
			}
		};
	}
}

