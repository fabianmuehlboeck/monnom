#pragma once
#include <fstream>
#include <iostream>
#include <vector>
#include "GeneratedFunction.h"
#include "CallParser.h"
#include "ClassMethodParser.h"
namespace Nom {
	namespace Runtime {
		class SpecParser
		{
		private:
			std::ifstream specFile;
			char readStream[255] = "";
			std::vector<GeneratedFunction*> functions;
			GeneratedFunction* currentFunction = nullptr;
			CallParser* callParser = nullptr;
			ClassMethodParser* cmParser = nullptr;
			ParsedArgument* returnArg = nullptr;
			std::string currentName = "";
		public:
			SpecParser() {
				int bCount = 0;
				int currentchar = 0;
				specFile.open("..\\spec.txt");
				if (specFile.is_open()) {
					/*
					The specification file parser, reads the file character by character and converts the String to corresponding
					Nom Runtime objects.
					*/
					char nextChar;
					while (specFile) {
						nextChar = specFile.get();
						/*
						checks if current statement is inside of a bracket. Useful for parsing spaces.
						*/
						if (nextChar == '(' || nextChar == '<') {
							bCount++;
						}
						if (nextChar == ')' || nextChar == '>') {
							bCount--;
						}

						/*
						Assigns the name of the corresponding C++ function
						*/
						if (nextChar == ':' && currentName == "") {
							currentName.assign(readStream);
							memset(readStream, 0, sizeof readStream);
							currentchar = 0;
							continue;
						}

						if (nextChar == ' ' || nextChar == ':' || nextChar == '\t') {
							/*
							Does not read space characters if it is within a bracket.
							*/
							if (strcmp(readStream, "") == 0) continue;
							if (bCount > 0) {
								if (nextChar == ':' && callParser == nullptr) {
									readStream[currentchar] = nextChar;
									currentchar++;
								}
								continue;
							}
							/*
							Call Parser is responsible for parsing the C++ function name and type parameters of the function
							*/
							if (callParser == nullptr){
								callParser = new CallParser(readStream);
							}
							/*
							Parses the MonNom class and method that the generated function calls. 
							*/
							else if (cmParser == nullptr) {
								cmParser = new ClassMethodParser(readStream, callParser->getSubstitutionTypeArgs());
							}
							/*
							Parses the return argument.
							*/
							else if (returnArg == nullptr) {
								returnArg = new ParsedArgument(std::string(readStream), callParser->getSubstitutionTypeArgs());
							}
							memset(readStream, 0, sizeof readStream);
							currentchar = 0;
							continue;
						}
						if (nextChar == '\n' || specFile.eof()) {
							/*
							At the end of the line, takes all components and generates a Nom C Library class in the RunTime
							*/
							if (callParser == nullptr) continue;
							if (cmParser == nullptr && strcmp(readStream, "") != 0) {
								cmParser = new ClassMethodParser(readStream, callParser->getSubstitutionTypeArgs());
							}
							else if (returnArg == nullptr && strcmp(readStream, "") != 0) {
								returnArg = new ParsedArgument(std::string(readStream), callParser->getSubstitutionTypeArgs());
							}
							currentFunction = new GeneratedFunction(currentName, callParser, cmParser, returnArg);
							functions.push_back(currentFunction);
							currentFunction = nullptr;
							callParser = nullptr;
							cmParser = nullptr;
							returnArg = nullptr;
							currentName = "";
							memset(readStream, 0, sizeof readStream);
							bCount = 0;
							currentchar = 0;
							continue;
						}
						readStream[currentchar] = nextChar;
						currentchar++;
					}
				}
			}
			/*
			Generates all LLVM functions from the specification file.
			*/
			void generateLLVMFunctions(llvm::Module& mod) {
				for (GeneratedFunction* func : functions) {
					func->generateLLVMFunction(mod);
				}
			}
		};
	}
}

