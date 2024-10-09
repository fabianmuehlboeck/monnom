#pragma once
#include "ParsedTP.h"
#include "NomType.h"
#include "NomClass.h"

namespace Nom {
	namespace Runtime {
		class ParsedType {
		public:
			virtual NomTypeRef getType(){}
		};
		class ParsedArgument;
		class ParsedClassType : public ParsedType
		{
		public:
			std::string className;
			std::vector<ParsedType*> typeArguments;
			std::vector<NomTypeRef> nomTypeArguments;
			ParsedClassType(std::string inputString, std::vector<ParsedTP*> substitutionParams);
			NomTypeRef getType() override {
				const NomString cName = NomString(className);
				return (NomTypeRef)NomClass::getClass(&cName)->GetType(getTypeArguments());
			}
			TypeList getTypeArguments() {
				if (nomTypeArguments.size() == 0) {
					for (ParsedType* pa : typeArguments) {
						nomTypeArguments.push_back(pa->getType());
					}
				}
				return TypeList(nomTypeArguments);
			}
			NomClassTypeRef getClassType() {
				const NomString cName = NomString(className);
				return NomClass::getClass(&cName)->GetType(getTypeArguments());
			}
			void pushRuntimeTypeArgs(std::vector<NomTypeRef> paramsVector) {
				
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
		public:
			ParsedTP* typeParam = nullptr;
			ParsedClassType* classArg = nullptr;

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

