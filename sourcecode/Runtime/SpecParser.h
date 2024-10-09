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
		public:

			std::ifstream specFile;
			char readStream[255] = "";
			std::vector<GeneratedFunction*> functions;
			GeneratedFunction* currentFunction = nullptr; 
			CallParser* callParser = nullptr;
			ClassMethodParser* cmParser = nullptr;
			ParsedArgument* returnArg = nullptr;
			std::string currentName = "";

			SpecParser() {
				int bCount = 0;
				int currentchar = 0;
				specFile.open("..\\spec.txt");
				if (specFile.is_open()) {
					char nextChar;
					while (specFile) {
						nextChar = specFile.get();
						if (nextChar == '(' || nextChar == '<') {
							bCount++;
						}
						if (nextChar == ')' || nextChar == '>') {
							bCount--;
						}

						if (nextChar == ':' && currentName == "") {
							currentName.assign(readStream);
							std::cout << readStream;
							memset(readStream, 0, sizeof readStream);
							currentchar = 0;
							continue;
						}

						if (nextChar == ' ' || nextChar == ':') {
							if (strcmp(readStream, "") == 0) continue;
							if (bCount > 0) {
								if (nextChar == ':' && callParser == nullptr) {
									readStream[currentchar] = nextChar;
									currentchar++;
								}
								continue;
							}
							if (callParser == nullptr){
								callParser = new CallParser(readStream);
							}
							else if (cmParser == nullptr) {
								cmParser = new ClassMethodParser(readStream,callParser->typeParams);
							}
							else if (returnArg == nullptr) {
								returnArg = new ParsedArgument(std::string(readStream), callParser->typeParams);
							}
							memset(readStream, 0, sizeof readStream);
							currentchar = 0;
							continue;
						}
						if (nextChar == '\n' || specFile.eof()) {
							if (callParser == nullptr) continue;
							if (cmParser == nullptr && strcmp(readStream, "") != 0) {
								cmParser = new ClassMethodParser(readStream, callParser->typeParams);
							}
							else if (returnArg == nullptr && strcmp(readStream, "") != 0) {
								returnArg = new ParsedArgument(std::string(readStream), callParser->typeParams);
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
		};
	}
}

