#pragma once
#include <string>
#include <vector>
#include "RTCompileConfig.h"
#include <iostream>

extern std::string NomRuntimePath;
extern int NomOptLevel;
extern int NomBlameLevel;
extern bool NomVerbose;
extern int NomDebugPrintLevel;
extern bool NomCastStats;
extern int NomStatsLevel;
extern bool NomTimings;
extern int NomFunctionTimingLevel;
extern std::string NomMainClassName;
extern std::string NomPath;
extern std::vector<std::string> NomDebugFunctions;
extern std::vector<std::string> NomApplicationArgs;
extern bool NomRuntimeStopAtEnd;
extern unsigned int NomWarmupRuns;
extern bool RTConfig_AdditionalOptPasses;
extern std::ostream * RT_debugout;

namespace Nom
{
	namespace Runtime
	{
		void parseArguments(int argc, char** args);
	}
}
