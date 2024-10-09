#pragma once
#include "NomVMInterface.h"
#include "RTConfig.h"
#include <time.h>
#ifdef _WIN32
#include <Windows.h>
#define TIMERTYPE LARGE_INTEGER
#else 
#ifdef CLOCK_THREAD_CPUTIME_ID
#define TIMERTYPE struct timespec
#else
#define TIMERTYPE clock_t
#endif
#endif

enum class AllocationType { General, Object, Lambda, Record, ClassType };

extern "C" DLLEXPORT void RT_NOM_STATS_IncCasts();
extern "C" DLLEXPORT void RT_NOM_STATS_IncMonotonicCasts();
extern "C" DLLEXPORT void RT_NOM_STATS_IncSubtypingChecks();
extern "C" DLLEXPORT void RT_NOM_STATS_IncTypeArgumentRecursions();
extern "C" DLLEXPORT void RT_NOM_STATS_IncTypeInstanceAlloactions();
extern "C" DLLEXPORT void RT_NOM_STATS_IncPerfectCallTagTypeMatches();
extern "C" DLLEXPORT void RT_NOM_STATS_IncCallTagTypeMismatches();
extern "C" DLLEXPORT void RT_NOM_STATS_IncImpositions();
extern "C" DLLEXPORT void RT_NOM_STATS_IncIntPacks();
extern "C" DLLEXPORT void RT_NOM_STATS_IncIntUnpacks();
extern "C" DLLEXPORT void RT_NOM_STATS_IncIntBoxes();
extern "C" DLLEXPORT void RT_NOM_STATS_IncIntUnboxes();
extern "C" DLLEXPORT void RT_NOM_STATS_IncFloatPacks();
extern "C" DLLEXPORT void RT_NOM_STATS_IncFloatUnpacks();
extern "C" DLLEXPORT void RT_NOM_STATS_IncFloatBoxes();
extern "C" DLLEXPORT void RT_NOM_STATS_IncFloatUnboxes();
extern "C" DLLEXPORT void RT_NOM_STATS_IncAllocations(AllocationType at = AllocationType::General);
extern "C" DLLEXPORT void RT_NOM_STATS_IncProfileCounter(size_t funnameid);
extern "C" DLLEXPORT TIMERTYPE RT_NOM_STATS_GetTimestamp();
extern "C" DLLEXPORT void RT_NOM_STATS_IncCastTime(TIMERTYPE origTimerVal);

extern "C" DLLEXPORT void RT_NOM_STATS_IncDirectClassMethodCalls();
extern "C" DLLEXPORT void RT_NOM_STATS_IncInterfaceMethodCalls();
extern "C" DLLEXPORT void RT_NOM_STATS_IncExtendedInterfaceMethodCalls();
extern "C" DLLEXPORT void RT_NOM_STATS_IncTypedRawInvokes();
extern "C" DLLEXPORT void RT_NOM_STATS_IncFinalInstanceMethodCalls();
extern "C" DLLEXPORT void RT_NOM_STATS_IncStaticMethodCalls();
extern "C" DLLEXPORT void RT_NOM_STATS_IncDynamicInvokes();
extern "C" DLLEXPORT void RT_NOM_STATS_IncDynamicMethodCalls();
extern "C" DLLEXPORT void RT_NOM_STATS_IncDynamicFieldLookups();
extern "C" DLLEXPORT void RT_NOM_STATS_IncCFunctionCalls();


enum class NomDebugPrintValueType : unsigned char { Nothing = 0, Pointer = 1, Int = 2, Float = 3, Bool = 4 };
extern "C" DLLEXPORT void RT_NOM_STATS_DebugLine(size_t funnameid, size_t linenum, NomDebugPrintValueType valueType, int64_t value, decltype(NomDebugPrintLevel) level);

extern "C" DLLEXPORT int RT_NOM_STATS_Print(int level);

namespace Nom
{
	namespace Runtime
	{
		llvm::Function* GetIncCastsFunction(llvm::Module& mod);
		llvm::Function* GetIncMonotonicCastsFunction(llvm::Module& mod);
		llvm::Function* GetIncSubtypingChecksFunction(llvm::Module& mod);
		llvm::Function* GetIncTypeArgumentRecursionsFunction(llvm::Module& mod);
		llvm::Function* GetIncTypeInstanceAlloactionsFunction(llvm::Module& mod);
		llvm::Function* GetIncPerfectCallTagTypeMatchesFunction(llvm::Module& mod);
		llvm::Function* GetIncCallTagTypeMismatchesFunction(llvm::Module& mod);
		llvm::Function* GetIncImpositionsFunction(llvm::Module& mod);
		llvm::Function* GetIncIntPacksFunction(llvm::Module& mod);
		llvm::Function* GetIncIntUnpacksFunction(llvm::Module& mod);
		llvm::Function* GetIncIntBoxesFunction(llvm::Module& mod);
		llvm::Function* GetIncIntUnboxesFunction(llvm::Module& mod);
		llvm::Function* GetIncFloatPacksFunction(llvm::Module& mod);
		llvm::Function* GetIncFloatUnpacksFunction(llvm::Module& mod);
		llvm::Function* GetIncFloatBoxesFunction(llvm::Module& mod);
		llvm::Function* GetIncFloatUnboxesFunction(llvm::Module& mod);
		llvm::Function* GetGetTimestampFunction(llvm::Module& mod);
		llvm::Function* GetIncCastTimeFunction(llvm::Module& mod);
		llvm::Function* GetDebugFunction(llvm::Module& mod);
		llvm::Function* GetIncAllocationsFunction(llvm::Module& mod);

		llvm::Function* GetIncDirectClassMethodCalls(llvm::Module& mod);
		llvm::Function* GetIncInterfaceMethodCalls(llvm::Module& mod);
		llvm::Function* GetIncExtendedInterfaceMethodCalls(llvm::Module& mod);
		llvm::Function* GetIncTypedRawInvokes(llvm::Module& mod);
		llvm::Function* GetIncFinalInstanceMethodCalls(llvm::Module& mod);
		llvm::Function* GetIncStaticMethodCalls(llvm::Module& mod);
		llvm::Function* GetIncDynamicInvokes(llvm::Module& mod);
		llvm::Function* GetIncDynamicMethodCalls(llvm::Module& mod);
		llvm::Function* GetIncDynamicFieldLookups(llvm::Module& mod);
		llvm::Function* GetIncCFunctionCalls(llvm::Module& mod);

		void InitializeProfileCounter();
		void PrintCastStats();
	}
}
