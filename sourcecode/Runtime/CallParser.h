#pragma once
#include <iostream>
#include <vector>
#include "ParsedTP.h"
namespace Nom {
	namespace Runtime {
		class CallParser
		{
		private:
			std::string callType;
			std::vector<ParsedTP*> typeParams;
			std::vector<NomTypeParameterRef> nomTypeParams;
		public:
			/*
			Parses the function call, which consists of the type of method being called, 
			e.g. static, instance, constructor as well as the type parameters. 
			*/
			CallParser(char inputString[]) {
				int i = 0;
				int firstPos = -1, secondPos = -1;
				while(inputString[i] != '\0') {
					if (inputString[i] == '<') {
						if (firstPos == -1) {
							firstPos = i;
						}
					}
					if (inputString[i] == '>') {
						if (secondPos == -1) {
							secondPos = i;
						}
					}
					i++;
				}
				if (firstPos == -1) {
					callType.assign(inputString);
					typeParams = {};
					return;
				}
				std::string fullString(inputString);
				callType = fullString.substr(0, firstPos);
				std::string typeParamString = fullString.substr(firstPos+1, secondPos-firstPos-1);
				firstPos = -1;
				for (i = 0; i < typeParamString.size(); i++) {
					if (typeParamString[i] == ',') {
						std::string typeParam1 = typeParamString.substr(firstPos+1, i-firstPos-1);
						ParsedTP* tp = new ParsedTP(typeParam1, typeParams.size());
						typeParams.push_back(tp);
						firstPos = i;
					}
				}
				std::string typeParam = typeParamString.substr(firstPos + 1, typeParamString.size());
				ParsedTP* tp = new ParsedTP(typeParam, typeParams.size());
				typeParams.push_back(tp);
			}
			llvm::ArrayRef<NomTypeParameterRef> getTypeArgs() {
				if (nomTypeParams.size() == 0) {
					for (ParsedTP* pa : typeParams) {
						nomTypeParams.push_back(pa->ParamRef);
					}
				}
				return llvm::ArrayRef<NomTypeParameterRef>(nomTypeParams);
			}
			std::vector<ParsedTP*> getSubstitutionTypeArgs() {
				return typeParams;
			}
			std::string getCallType() { return callType; }
		};
	}
}

