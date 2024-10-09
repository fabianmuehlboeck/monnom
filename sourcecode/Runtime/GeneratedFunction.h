#pragma once
#include<iostream>
#include "NomCLib.h"
#include "ClassMethodParser.h"
#include "CallParser.h"


namespace Nom {
	namespace Runtime {
		class GeneratedFunction
		{
		public:
			std::string CMethodName;
			std::string className;
			std::string callMethodName;
			NomCLib* generatedLibrary;

			GeneratedFunction(std::string name, CallParser* callParser, ClassMethodParser* cmParser, ParsedArgument* returnArg) {
				CMethodName = name;
				if (callParser->callType == "construct") {
					generatedLibrary = new NomCLibConstructor(name, cmParser->methodType->getConstructorClassInstantiation(), cmParser->methodType->getArgTypes(), callParser->getTypeArgs(), cmParser->methodType->getConstructorClassType());
				}
				else if (callParser->callType == "call_static") {
					generatedLibrary = new NomCLibStatic(name, cmParser->getStaticMethodInstantiation(), cmParser->methodType->getArgTypes(), callParser->getTypeArgs(), returnArg->getType());
				}
				else if (callParser->callType == "call_instance") {
					generatedLibrary = new NomCLibInstance(name, cmParser->getInstanceMethodInstantiation(), cmParser->classType->getClassType(), cmParser->methodType->getArgTypes(), callParser->getTypeArgs(), returnArg->getType());
				}
			}
			GeneratedFunction(std::string name) {
				CMethodName = name;
			}

			llvm::ArrayRef<NomTypeRef> argTypes;
			llvm::ArrayRef<NomTypeParameterRef> typeArgs;
			NomTypeRef returnType;

			
		};

		class GeneratedConstructorCall : public GeneratedFunction
		{
		public:
			GeneratedConstructorCall(std::string name): GeneratedFunction(name){}
		};
		class GeneratedStaticCall : public GeneratedFunction
		{
		public:
			GeneratedStaticCall(std::string name) : GeneratedFunction(name) {}
		};
		class GeneratedInstanceCall : public GeneratedFunction
		{
		public:
			GeneratedInstanceCall(std::string name) : GeneratedFunction(name) {}
		};
	}
}

