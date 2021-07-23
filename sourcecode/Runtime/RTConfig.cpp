#include "RTConfig.h"
#include <iostream>
#include <string.h>

std::string NomRuntimePath = "";
int NomBlameLevel = 0;
int NomOptLevel = 2;
int NomDebugPrintLevel = -1;
bool NomVerbose = false;
bool NomCastStats = false;
int NomStatsLevel = 0;
bool NomTimings = false;
int NomFunctionTimingLevel = 0;
int NomLambdaOptimizationLevel = 1;
int IMTsize = 13;
bool PreferTypeArgumentsInVTables = false;
int NomWarmupRuns = 0;
std::string NomMainClassName = "";
std::string NomPath = ".";
std::vector<std::string> NomDebugFunctions = std::vector<std::string>();
std::vector<std::string> NomApplicationArgs = std::vector<std::string>();
bool NomRuntimeStopAtEnd = false;
//using namespace llvm;
using namespace std;
namespace Nom
{
	namespace Runtime
	{
		void parseArguments(int argc, char** args)
		{
			NomRuntimePath = args[0];
			int argpos = 1;
			while (argpos < argc)
			{
				if (args[argpos][0] == '-')
				{
					switch (args[argpos][1])
					{
					case 'd':
						if (args[argpos][2] == '\0')
						{
							NomDebugPrintLevel = 2;
							break;
						}
						if (args[argpos][3] == '\0')
						{
							switch (args[argpos][2])
							{
							case '0':
								NomDebugPrintLevel = 0;
								break;
							case '1':
								NomDebugPrintLevel = 1;
								break;
							case '2':
								NomDebugPrintLevel = 2;
								break;
							case '3':
								NomDebugPrintLevel = 3;
								break;
							default:
								std::cout << "\nERROR: Invalid debug print level!\n";
								throw new std::exception();
							}
						}
						else if (args[argpos][2] == '+')
						{
							std::string arg = std::string(args[argpos]);
							NomDebugFunctions.push_back(arg.substr(3));
						}
						break;
					case 'p':
						if (args[argpos][2] == '\0')
						{
							if (argpos + 1 < argc)
							{
								argpos++;
								NomPath = args[argpos];
							}
						}
						break;
					case 'v':
						NomVerbose = true;
						break;
					case 'o':
						if (args[argpos][2] == '\0')
						{
							NomOptLevel = 2;
						}
						else if (args[argpos][3] == '\0')
						{
							switch (args[argpos][2])
							{
							case '0':
								NomOptLevel = 0;
								break;
							case '1':
								NomOptLevel = 1;
								break;
							case '2':
								NomOptLevel = 2;
								break;
							case '3':
								NomOptLevel = 3;
								break;
							default:
								std::cout << "\nERROR: Invalid optimization level!\n";
								throw new std::exception();
							}
						}
						else
						{
							std::cout << "\nERROR: Invalid optimization level!\n";
							throw new std::exception();
						}
						break;
					case 'b':
						if (args[argpos][2] == '\0')
						{
							NomBlameLevel = 1;
						}
						else if (args[argpos][3] == '\0')
						{
							switch (args[argpos][2])
							{
							case '0':
								NomBlameLevel = 0;
								break;
							case '1':
								NomBlameLevel = 1;
								break;
								//case '2':
								//	NomBlameLevel = 2;
								//	break;
								//case '3':
								//	NomBlameLevel = 3;
								//	break;
							default:
								std::cout << "\nERROR: Invalid blame level!\n";
								throw new std::exception();
							}
						}
						else
						{
							std::cout << "\nERROR: Invalid blame level!\n";
							throw new std::exception();
						}
						break;
					case 's':
						if (args[argpos][2] == '\0')
						{
							NomCastStats = true;
							NomTimings = true;
						}
						else if (args[argpos][3] == '\0')
						{
							switch (args[argpos][2])
							{
							case '1':
								NomCastStats = true;
								NomTimings = true;
								NomStatsLevel = 1;
								break;
							case '2':
								NomCastStats = true;
								NomTimings = true;
								NomStatsLevel = 2;
								break;
							case '3':
								NomCastStats = true;
								NomTimings = true;
								NomStatsLevel = 3;
								break;
							case '4':
								NomCastStats = true;
								NomTimings = true;
								NomStatsLevel = 4;
								break;
							}
						}
						else
						{
							std::cout << "\nERROR: Invalid stats level!\n";
							throw new std::exception();
						}
						break;
					case 't':
						NomTimings = true;
						break;
					case 'f':
						if (args[argpos][2] == '\0')
						{
							NomFunctionTimingLevel = 1;
						}
						else if (args[argpos][3] == '\0')
						{
							switch (args[argpos][2])
							{
							case '1'://load timings
								NomFunctionTimingLevel = 1;
								break;

							case '2':
								NomFunctionTimingLevel = 1;
								break;

							case '3'://actual function timings
								NomFunctionTimingLevel = 3;
								break;
							}
						}
						else
						{
							std::cout << "\nERROR: Invalid function timing level!\n";
							throw new std::exception();
						}
						break;
					case 'w':
						NomWarmupRuns = 1;
						if (strlen(args[argpos]) > 2)
						{
							char* curarg = args[argpos] + 2;
							NomWarmupRuns=atoi(curarg);
						}
						break;
					case '-':
						if (strncmp(args[argpos], "--nolambdaopt",20)==0)
						{
							NomLambdaOptimizationLevel = 0;
						}
						else if (strncmp(args[argpos], "--imtsize", 12) == 0)
						{
							argpos++;
							if (argpos < argc)
							{
								int imtsize = atoi(args[argpos]);
								if (imtsize > 0 && imtsize < 128)
								{
									IMTsize = imtsize;
								}
								else
								{
									std::cout << "ERROR: Invalid IMT size parameter - must be between 1 and 127!\n";
									throw new std::exception();
								}
							}
						}
						else if (strncmp(args[argpos], "--vttargs", 12) == 0)
						{
							PreferTypeArgumentsInVTables = true;
						}
						else if (strncmp(args[argpos], "--stopatend", 12) == 0)
						{
							NomRuntimeStopAtEnd = true;
						}
						else if (strncmp(args[argpos], "--version", 12) == 0)
						{
							std::cout << "MonNom Runtime Version 0.2\n";
						}
						break;
					default:
						std::cout << "\nERROR: Invalid runtime flag!\n";
						throw new std::exception();
					}
				}
				else
				{
					//if (NomMainClassName.empty())
					//{
					NomMainClassName = args[argpos];
					argpos++;
					while (argpos < argc)
					{
						NomApplicationArgs.push_back(args[argpos]);
						argpos++;
					}
					break;
					//}
					//else
					//{
						//NomPath = args[argpos];
					//}
				}
				argpos++;
			}
		}
	}
}