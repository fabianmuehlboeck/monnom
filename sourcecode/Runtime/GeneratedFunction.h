#pragma once
#include<iostream>
#include "NomCLib.h"
#include "ClassMethodParser.h"
#include "CallParser.h"


namespace Nom {
	namespace Runtime {
		class GeneratedFunction
		{
		private:
			NomCLib* generatedLibrary;
		public:
			GeneratedFunction(std::string name, CallParser* callParser, ClassMethodParser* cmParser, ParsedArgument* returnArg) {
				if (callParser->getCallType() == "construct") {
					generatedLibrary = new NomCLibConstructor(name, cmParser->getConstructorClassInstantiation(), cmParser->getArgTypes(), callParser->getTypeArgs(), cmParser->getClassType());
				}
				else if (callParser->getCallType() == "call_static") {
					generatedLibrary = new NomCLibStatic(name, cmParser->getStaticMethodInstantiation(), cmParser->getArgTypes(), callParser->getTypeArgs(), returnArg->getType());
				}
				else if (callParser->getCallType() == "call_instance") {
					generatedLibrary = new NomCLibInstance(name, cmParser->getInstanceMethodInstantiation(), cmParser->getClassType(), cmParser->getArgTypes(), callParser->getTypeArgs(), returnArg->getType());
				}
			}	
			~GeneratedFunction() {
			}
			/*
			Uses the Available Externally Interface to get the llvm function of the generated library.
			In this case, since this function does not exist in the module already, it creates it
			*/
			void generateLLVMFunction(llvm::Module& mod) {
				generatedLibrary->GetLLVMElement(mod);
			}
		};
	}
}

