#include "CastStats.h"
#include <iostream>
#include <unordered_map>
#include "NomAlloc.h"
#include "NomNameRepository.h"
PUSHDIAGSUPPRESSION
#include "llvm/Support/DynamicLibrary.h"
POPDIAGSUPPRESSION
#include <vector>

#define RBUFSIZE 500
#ifdef _WIN32
#define TIMETYPE LONGLONG
#endif

using namespace std;

static size_t castCount = 0;
static size_t monoCastCount = 0;
static size_t subtypingChecksCount = 0;
static size_t typeArgumentRecursionsCount = 0;
static size_t typeInstanceAllocationsCount = 0;
static size_t perfectCallTagTypeMatches = 0;
static size_t callTagTypeMismatches = 0;
static size_t impositionsCount = 0;
static size_t intPacksCount = 0;
static size_t intUnpacksCount = 0;
static size_t intBoxesCount = 0;
static size_t intUnboxesCount = 0;
static size_t floatPacksCount = 0;
static size_t floatUnpacksCount = 0;
static size_t floatBoxesCount = 0;
static size_t floatUnboxesCount = 0;
static size_t timeUnitsInCasts = 0;

static size_t directClassMethodCalls = 0;
static size_t interfaceMethodCalls = 0;
static size_t extendedInterfaceMethodCalls = 0;
static size_t typedRawInvokes = 0;
static size_t finalInstanceMethodCalls = 0;

static size_t staticMethodCalls = 0;

static size_t dynamicInvokes = 0;
static size_t dynamicMethodCalls = 0;

static size_t dynamicFieldLookups = 0;

static const std::string** debugFunNames;
static size_t* profileCounterArray;
static vector<size_t>* detailedProfileCounterArray;
static TIMERTYPE lasttimestamp;
#ifdef _WIN32
static TIMETYPE avgtime;
#endif
static int64_t timestampcount = 0;

static uint64_t general_allocations = 0;
static uint64_t object_allocations = 0;
static uint64_t closure_allocations = 0;
static uint64_t record_allocations = 0;
static uint64_t classtype_allocations = 0;

DLLEXPORT void RT_NOM_STATS_IncCasts()
{
	castCount++;
}

DLLEXPORT void RT_NOM_STATS_IncMonotonicCasts()
{
	monoCastCount++;
}

DLLEXPORT void RT_NOM_STATS_IncSubtypingChecks()
{
	subtypingChecksCount++;
}

DLLEXPORT void RT_NOM_STATS_IncTypeArgumentRecursions()
{
	typeArgumentRecursionsCount++;
}

DLLEXPORT void RT_NOM_STATS_IncTypeInstanceAlloactions()
{
	typeInstanceAllocationsCount++;
}

DLLEXPORT void RT_NOM_STATS_IncPerfectCallTagTypeMatches()
{
	perfectCallTagTypeMatches++;
}

DLLEXPORT void RT_NOM_STATS_IncCallTagTypeMismatches()
{
	callTagTypeMismatches++;
}

DLLEXPORT void RT_NOM_STATS_IncImpositions()
{
	impositionsCount++;
}

DLLEXPORT void RT_NOM_STATS_IncIntPacks()
{
	intPacksCount++;
}
DLLEXPORT void RT_NOM_STATS_IncIntUnpacks()
{
	intUnpacksCount++;
}
DLLEXPORT void RT_NOM_STATS_IncIntBoxes()
{
	intBoxesCount++;
}
DLLEXPORT void RT_NOM_STATS_IncIntUnboxes()
{
	intUnboxesCount++;
}
DLLEXPORT void RT_NOM_STATS_IncFloatPacks()
{
	floatPacksCount++;
}
DLLEXPORT void RT_NOM_STATS_IncFloatUnpacks()
{
	floatUnpacksCount++;
}
DLLEXPORT void RT_NOM_STATS_IncFloatBoxes()
{
	floatBoxesCount++;
}
DLLEXPORT void RT_NOM_STATS_IncFloatUnboxes()
{
	floatUnboxesCount++;
}
DLLEXPORT void RT_NOM_STATS_IncAllocations(AllocationType at)
{
	switch (at)
	{
	case AllocationType::General:
		general_allocations++;
		break;
	case AllocationType::Object:
		object_allocations++;
		break;
	case AllocationType::Lambda:
		closure_allocations++;
		break;
	case AllocationType::Record:
		record_allocations++;
		break;
	case AllocationType::ClassType:
		classtype_allocations++;
		break;
	}
}
DLLEXPORT void RT_NOM_STATS_IncProfileCounter(size_t funnameid)
{
	profileCounterArray[funnameid]++;
}

#ifdef _WIN32
static double find_timer_frequency()
{
	static double timer_frequency;
	static bool once = true;
	if (once)
	{
		once = false;
		LARGE_INTEGER ttf;
		if (!QueryPerformanceFrequency(&ttf))
		{
			throw new std::exception();
		}
		timer_frequency = static_cast<double>(ttf.QuadPart);
	}
	return timer_frequency;
}
#endif

DLLEXPORT TIMERTYPE RT_NOM_STATS_GetTimestamp()
{
#ifdef _WIN32
	LARGE_INTEGER timerVal;
	if (!QueryPerformanceCounter(&timerVal))
	{
		std::cout << "ERROR obtaining performance counter value";
		//throw new std::exception();
	}
	return timerVal;
#else
#ifdef CLOCK_THREAD_CPUTIME_ID
	struct timespec timerVal;
	if (clock_gettime(CLOCK_THREAD_CPUTIME_ID, &timerVal) != 0)
	{
		throw new std::exception();
	}
	return timerVal;
#else
	return clock();
#endif
#endif
}


DLLEXPORT void RT_NOM_STATS_IncCastTime(TIMERTYPE origTimerVal)
{
#ifdef _WIN32
	LARGE_INTEGER current;
	if (QueryPerformanceCounter(&current))
	{
		timeUnitsInCasts += static_cast<size_t>(current.QuadPart - origTimerVal.QuadPart);
	}
	else
	{
		std::cout << "ERROR obtaining performance counter value";
	}
#else
#ifdef CLOCK_THREAD_CPUTIME_ID
	struct timespec current;
	if (clock_gettime(CLOCK_THREAD_CPUTIME_ID, &current) == 0)
	{
		timeUnitsInCasts += ((current.tv_sec - origTimerVal.tv_sec) * 10000000) + ((current.tv_nsec - origTimerVal.tv_nsec) / 100);
		//double floatdiff = ((double)differenceMS) / 10000.0;
		//printf("%f Seconds\n", floatdiff);
		//return (void*)((intptr_t)(Nom::Runtime::NomJIT::Instance().getSymbolAddress("RT_NOM_VOIDOBJ")));
	}
	else
	{
		throw new std::exception();
	}
#else
	clock_t now = clock();
	timeUnitsInCasts += (long)(clock() - origTimerVal);
	//printf("%f Seconds, (%li ticks)\n", ((double)t) / CLOCKS_PER_SEC, t);
#endif
#endif
}

DLLEXPORT void RT_NOM_STATS_IncDirectClassMethodCalls()
{
	directClassMethodCalls++;
}
DLLEXPORT void RT_NOM_STATS_IncInterfaceMethodCalls()
{
	interfaceMethodCalls++;
}
DLLEXPORT void RT_NOM_STATS_IncExtendedInterfaceMethodCalls()
{
	extendedInterfaceMethodCalls++;
}
DLLEXPORT void RT_NOM_STATS_IncTypedRawInvokes()
{
	typedRawInvokes++;
}
DLLEXPORT void RT_NOM_STATS_IncFinalInstanceMethodCalls()
{
	finalInstanceMethodCalls++;
}
DLLEXPORT void RT_NOM_STATS_IncStaticMethodCalls()
{
	staticMethodCalls++;
}
DLLEXPORT void RT_NOM_STATS_IncDynamicInvokes()
{
	dynamicInvokes++;
}
DLLEXPORT void RT_NOM_STATS_IncDynamicMethodCalls()
{
	dynamicMethodCalls++;
}
DLLEXPORT void RT_NOM_STATS_IncDynamicFieldLookups()
{
	dynamicFieldLookups++;
}


static size_t debugPrint_lastIndices[4][RBUFSIZE];
static size_t debugPrint_lastStrs[4][RBUFSIZE];
static int64_t debugPrint_lastValues[4][RBUFSIZE];
static NomDebugPrintValueType debugPrint_lastValueTypes[4][RBUFSIZE];
static decltype(NomDebugPrintLevel) debugPrint_lastLevel = 0;
static int debugPrint_rbufpos[4] = { 0, 0, 0, 0 };

struct ProgramPos
{
public:
	ProgramPos() {}
	ProgramPos(size_t funname, size_t linenum, double seconds) : FunName(funname), LineNum(linenum), Seconds(seconds)
	{

	}
	size_t FunName;
	size_t LineNum;
	double Seconds;
};

[[clang::no_destroy]] static std::vector<ProgramPos> slowPositions;
static double slowestSoFar = -1;

DLLEXPORT void RT_NOM_STATS_DebugLine(size_t funnameid, size_t linenum, NomDebugPrintValueType valueType, int64_t value, decltype(NomDebugPrintLevel) level)
{
	auto& outstream = *RT_debugout;
	if (NomStatsLevel > 3)
	{
#ifdef _WIN32
		TIMERTYPE currentTime = RT_NOM_STATS_GetTimestamp();
		if (timestampcount > 0)
		{
			TIMETYPE difference = currentTime.QuadPart - lasttimestamp.QuadPart;
			if (difference > avgtime * 2 && timestampcount > 10)
			{
				double seconds = (static_cast<double>(difference - avgtime)) / find_timer_frequency();
				if (seconds > slowestSoFar || slowPositions.size() < 100)
				{
					auto iter = slowPositions.begin();
					auto posn = iter;
					bool found = false;
					for (; iter != slowPositions.end(); iter++)
					{
						auto pp = *iter;
						if (pp.FunName == funnameid && pp.LineNum == linenum)
						{
							if (pp.Seconds < seconds)
							{
								pp.Seconds = seconds;
								found = true;
							}
							found = true;
							break;
						}

						if (pp.Seconds < seconds && !found)
						{
							auto npp = ProgramPos(funnameid, linenum, seconds);
							slowPositions.insert(iter, std::move(npp));
							outstream << "SLOW: " << (*debugFunNames[funnameid]) << ":" << linenum << " - " << seconds << "s\n";
							found = true;
							break;
						}
					}
					if (!found && slowPositions.size() < 100)
					{
						auto npp = ProgramPos(funnameid, linenum, seconds);
						slowPositions.insert(iter, std::move(npp));
						outstream << "SLOW: " << (*debugFunNames[funnameid]) << ":" << linenum << " - " << seconds << "s\n";
					}
					if (slowPositions.size() > 100)
					{
						slowPositions.resize(100);
					}
				}
			}
			if (timestampcount > 1)
			{
				avgtime += difference / timestampcount;
			}
			else
			{
				avgtime = difference;
			}
		}
		timestampcount++;
#endif
	}
	if (level <= 3)
	{
		debugPrint_lastLevel = level;
		debugPrint_lastIndices[level][debugPrint_rbufpos[level]] = linenum;
		debugPrint_lastStrs[level][debugPrint_rbufpos[level]] = funnameid;
		debugPrint_lastValues[level][debugPrint_rbufpos[level]] = value;
		debugPrint_lastValueTypes[level][debugPrint_rbufpos[level]] = valueType;
		debugPrint_rbufpos[level] = (debugPrint_rbufpos[level] + 1) % RBUFSIZE;
	}
	if (NomDebugPrintLevel >= level)
	{
		outstream << *(debugFunNames[funnameid]) << ":" << std::dec << linenum << "\n";
		switch (valueType)
		{
		case NomDebugPrintValueType::Pointer:
			outstream << "   Pointer: " << std::hex << value << "\n";
			break;
		case NomDebugPrintValueType::Int:
			outstream << "   Int: " << std::dec << value << "\n";
			break;
		case NomDebugPrintValueType::Float:
			outstream << "   Float: " << std::hex << value << "\n";
			break;
		case NomDebugPrintValueType::Bool:
			outstream << "   Bool: " << std::dec << value << "\n";
			break;
		case NomDebugPrintValueType::Nothing:
			break;
		}
		outstream.flush();
	}
	if (NomStatsLevel > 1)
	{
		RT_NOM_STATS_IncProfileCounter(funnameid);
		if (NomStatsLevel > 2)
		{
			vector<size_t>& sizevec = detailedProfileCounterArray[funnameid];
			if (sizevec.size() < linenum + 1)
			{
				sizevec.resize(linenum + 10, 0);
			}
			sizevec[linenum]++;
		}
	}
	if (NomStatsLevel > 3)
	{
		lasttimestamp = RT_NOM_STATS_GetTimestamp();
	}
	return;
}
DLLEXPORT int RT_NOM_STATS_Print(int level)
{
	auto& outstream = *RT_debugout;
	int rbufpos = debugPrint_rbufpos[level];
	int target = rbufpos - 1;
	if (target < 0)
	{
		target = RBUFSIZE - 1;
	}
	while (rbufpos != target)
	{
		auto funnameid = debugPrint_lastStrs[level][rbufpos];
		if (funnameid == 0)
		{
			rbufpos = (rbufpos + 1) % RBUFSIZE;
			continue;
		}
		auto linenum = debugPrint_lastIndices[level][rbufpos];
		auto value = debugPrint_lastValues[level][rbufpos];
		auto valueType = debugPrint_lastValueTypes[level][rbufpos];
		std::cout << *(debugFunNames[funnameid]) << ":" << std::dec << linenum << "\n";
		switch (valueType)
		{
		case NomDebugPrintValueType::Pointer:
			outstream << "   Pointer: " << std::hex << value << "\n";
			break;
		case NomDebugPrintValueType::Int:
			outstream << "   Int: " << std::dec << value << "\n";
			break;
		case NomDebugPrintValueType::Float:
			outstream << "   Float: " << std::hex << value << "\n";
			break;
		case NomDebugPrintValueType::Bool:
			outstream << "   Bool: " << std::dec << value << "\n";
			break;
		case NomDebugPrintValueType::Nothing:
			break;
		}
		rbufpos = (rbufpos + 1) % RBUFSIZE;
	}
	{
		auto funnameid = debugPrint_lastStrs[level][rbufpos];
		if (funnameid != 0)
		{
			rbufpos = (rbufpos + 1) % RBUFSIZE;
			auto linenum = debugPrint_lastIndices[level][rbufpos];
			auto value = debugPrint_lastValues[level][rbufpos];
			auto valueType = debugPrint_lastValueTypes[level][rbufpos];
			std::cout << *(debugFunNames[funnameid]) << ":" << std::dec << linenum << "\n";
			switch (valueType)
			{
			case NomDebugPrintValueType::Pointer:
				outstream << "   Pointer: " << std::hex << value << "\n";
				break;
			case NomDebugPrintValueType::Int:
				outstream << "   Int: " << std::dec << value << "\n";
				break;
			case NomDebugPrintValueType::Float:
				outstream << "   Float: " << std::hex << value << "\n";
				break;
			case NomDebugPrintValueType::Bool:
				outstream << "   Bool: " << std::dec << value << "\n";
				break;
			case NomDebugPrintValueType::Nothing:
				break;
			}
		}
	}
	outstream << "------------------------------------------------------\n";
	outstream.flush();
	return rbufpos;
}

using namespace llvm;
using namespace std;
namespace Nom
{
	namespace Runtime
	{
		void InitializeProfileCounter()
		{
			llvm::sys::DynamicLibrary::AddSymbol("RT_NOM_STATS_DebugLine", reinterpret_cast<void*>(& RT_NOM_STATS_DebugLine));
			auto maxid = NomNameRepository::ProfilingInstance().GetMaxID();
			debugFunNames = makenmalloc(const std::string*, (maxid + 1));
			profileCounterArray = makenmalloc(size_t, (maxid + 1));
			if (NomStatsLevel >= 3)
			{
				detailedProfileCounterArray = new vector<size_t>[maxid + 1];
			}
			for (decltype(maxid) i = 1; i <= maxid; i++)
			{
				debugFunNames[i] = NomNameRepository::ProfilingInstance().GetNameFromID(i);
				profileCounterArray[i] = 0;

			}
		}
		void PrintCastStats()
		{
			auto& outstream = *RT_debugout;
			outstream << "\n\nCasting Stats:\n----------------------";
			outstream << "\n#Total Casts: " << std::dec << castCount;
			outstream << "\n#Monotonic Casts: " << std::dec << monoCastCount;
			outstream << "\n#Subtyping Checks: " << std::dec << subtypingChecksCount;
			outstream << "\n#Type-Arg Recursions: " << std::dec << typeArgumentRecursionsCount;
			outstream << "\n#Type Instance Allocations: " << std::dec << typeInstanceAllocationsCount;
			outstream << "\n#Perfect Call Tag Matches: " << std::dec << perfectCallTagTypeMatches;
			outstream << "\n#Call Tag Mismatches: " << std::dec << callTagTypeMismatches;
			outstream << "\n#Impositions: " << std::dec << impositionsCount;
			outstream << "\n#Int Packs: " << std::dec << intPacksCount;
			outstream << "\n#Int Unpacks: " << std::dec << intUnpacksCount;
			outstream << "\n#Int Boxes: " << std::dec << intBoxesCount;
			outstream << "\n#Int Unboxes: " << std::dec << intUnboxesCount;
			outstream << "\n#Float Packs: " << std::dec << floatPacksCount;
			outstream << "\n#Float Unpacks: " << std::dec << floatUnpacksCount;
			outstream << "\n#Float Boxes: " << std::dec << floatBoxesCount;
			outstream << "\n#Float Unboxes: " << std::dec << floatUnboxesCount;
			outstream << "\n\nAllocation Stats:\n----------------------";
			outstream << "\n#General Allocations: " << std::dec << general_allocations;
			outstream << "\n#Object Allocations: " << std::dec << object_allocations;
			outstream << "\n#Closure Allocations: " << std::dec << closure_allocations;
			outstream << "\n#Record Allocations: " << std::dec << record_allocations;
			outstream << "\n#Class Type Allocations: " << std::dec << classtype_allocations;
			outstream << "\n#Total Allocations: " << std::dec << (general_allocations + object_allocations + closure_allocations + record_allocations + classtype_allocations);
			outstream << "\n\nCall/Lookup Stats:\n----------------------";
			outstream << "\n#Direct Class Method Calls: " << std::dec << directClassMethodCalls;
			outstream << "\n#Interface Method Calls: " << std::dec << (interfaceMethodCalls + extendedInterfaceMethodCalls);
			outstream << "\n  #Interface Method Calls with Extended Argument Arrays: " << std::dec << extendedInterfaceMethodCalls;
			outstream << "\n#Typed Raw Invocations: " << std::dec << typedRawInvokes;
			outstream << "\n#Final Instance Method Calls: " << std::dec << finalInstanceMethodCalls;
			outstream << "\n#Static Method Calls: " << std::dec << staticMethodCalls;
			outstream << "\n#Dynamic Invokes: " << std::dec << dynamicInvokes;
			outstream << "\n#Dynamic Method Calls: " << std::dec << dynamicMethodCalls;
			outstream << "\n#Dynamic Field Lookups: " << std::dec << dynamicFieldLookups;

#ifdef _WIN32
			double castSeconds = static_cast<double>(timeUnitsInCasts) / find_timer_frequency();
#else
#ifdef CLOCK_THREAD_CPUTIME_ID
			double castSeconds = static_cast<double>(timeUnitsInCasts) / 10000000.0;
#else
			double castSeconds = static_cast<double>(timeUnitsInCasts) / CLOCKS_PER_SEC;
#endif
#endif
			outstream << "\nTime spent in casts: " << std::dec << castSeconds << " Seconds";

			outstream << "\n";

			if (NomStatsLevel > 1)
			{
				outstream << "\nProfile Summary:\n----------------------\n\n";
				auto maxid = NomNameRepository::ProfilingInstance().GetMaxID();
				for (decltype(maxid) i = 1; i <= maxid; i++)
				{
					outstream << (*debugFunNames[i]) << ": " << std::dec << profileCounterArray[i] << "\n";
				}

				if (NomStatsLevel > 3)
				{
					outstream << "\n\nSlow Timings:\n----------------------\n\n";
					for (auto pp : slowPositions)
					{
						outstream << (*debugFunNames[pp.FunName]) << ":" << pp.LineNum << " - " << pp.Seconds << "s\n";
					}
				}

				if (NomStatsLevel > 2)
				{
					outstream << "\n\nProfile Data:\n----------------------\n\n";
					for (decltype(maxid) i = 1; i <= maxid; i++)
					{
						outstream << (*debugFunNames[i]) << ": " << std::dec << profileCounterArray[i] << "\n";
						vector<size_t>& dpca = detailedProfileCounterArray[i];
						if (dpca.size() > 0)
						{
							for (std::vector<size_t>::size_type j = 0; j < dpca.size(); j++)
							{
								if (dpca[j] > 0)
								{
									outstream << "    " << j << ": " << dpca[j] << "\n";
								}
							}
						}
					}
				}
			}
		}
		llvm::Function* GetIncCastsFunction(llvm::Module& mod)
		{
			auto fun = mod.getFunction("RT_NOM_STATS_IncCasts");
			if (fun == nullptr)
			{
				fun = Function::Create(FunctionType::get(Type::getVoidTy(LLVMCONTEXT), false), GlobalValue::LinkageTypes::ExternalLinkage, "RT_NOM_STATS_IncCasts", &mod);
			}
			return fun;
		}
		llvm::Function* GetIncMonotonicCastsFunction(llvm::Module& mod)
		{
			auto fun = mod.getFunction("RT_NOM_STATS_IncMonotonicCasts");
			if (fun == nullptr)
			{
				fun = Function::Create(FunctionType::get(Type::getVoidTy(LLVMCONTEXT), false), GlobalValue::LinkageTypes::ExternalLinkage, "RT_NOM_STATS_IncMonotonicCasts", &mod);
			}
			return fun;
		}
		llvm::Function* GetIncSubtypingChecksFunction(llvm::Module& mod)
		{
			auto fun = mod.getFunction("RT_NOM_STATS_IncSubtypingChecks");
			if (fun == nullptr)
			{
				fun = Function::Create(FunctionType::get(Type::getVoidTy(LLVMCONTEXT), false), GlobalValue::LinkageTypes::ExternalLinkage, "RT_NOM_STATS_IncSubtypingChecks", &mod);
			}
			return fun;
		}
		llvm::Function* GetIncTypeArgumentRecursionsFunction(llvm::Module& mod)
		{
			auto fun = mod.getFunction("RT_NOM_STATS_IncTypeArgumentRecursions");
			if (fun == nullptr)
			{
				fun = Function::Create(FunctionType::get(Type::getVoidTy(LLVMCONTEXT), false), GlobalValue::LinkageTypes::ExternalLinkage, "RT_NOM_STATS_IncTypeArgumentRecursions", &mod);
			}
			return fun;
		}
		llvm::Function* GetIncTypeInstanceAlloactionsFunction(llvm::Module& mod)
		{
			auto fun = mod.getFunction("RT_NOM_STATS_IncTypeInstanceAlloactions");
			if (fun == nullptr)
			{
				fun = Function::Create(FunctionType::get(Type::getVoidTy(LLVMCONTEXT), false), GlobalValue::LinkageTypes::ExternalLinkage, "RT_NOM_STATS_IncTypeInstanceAlloactions", &mod);
			}
			return fun;
		}
		llvm::Function* GetIncPerfectCallTagTypeMatchesFunction(llvm::Module& mod)
		{
			auto fun = mod.getFunction("RT_NOM_STATS_IncPerfectCallTagTypeMatches");
			if (fun == nullptr)
			{
				fun = Function::Create(FunctionType::get(Type::getVoidTy(LLVMCONTEXT), false), GlobalValue::LinkageTypes::ExternalLinkage, "RT_NOM_STATS_IncPerfectCallTagTypeMatches", &mod);
			}
			return fun;
		}
		llvm::Function* GetIncCallTagTypeMismatchesFunction(llvm::Module& mod)
		{
			auto fun = mod.getFunction("RT_NOM_STATS_IncCallTagTypeMismatches");
			if (fun == nullptr)
			{
				fun = Function::Create(FunctionType::get(Type::getVoidTy(LLVMCONTEXT), false), GlobalValue::LinkageTypes::ExternalLinkage, "RT_NOM_STATS_IncCallTagTypeMismatches", &mod);
			}
			return fun;
		}
		llvm::Function* GetIncImpositionsFunction(llvm::Module& mod)
		{
			auto fun = mod.getFunction("RT_NOM_STATS_IncImpositions");
			if (fun == nullptr)
			{
				fun = Function::Create(FunctionType::get(Type::getVoidTy(LLVMCONTEXT), false), GlobalValue::LinkageTypes::ExternalLinkage, "RT_NOM_STATS_IncImpositions", &mod);
			}
			return fun;
		}
		llvm::Function* GetIncIntPacksFunction(llvm::Module& mod)
		{
			auto fun = mod.getFunction("RT_NOM_STATS_IncIntPacks");
			if (fun == nullptr)
			{
				fun = Function::Create(FunctionType::get(Type::getVoidTy(LLVMCONTEXT), false), GlobalValue::LinkageTypes::ExternalLinkage, "RT_NOM_STATS_IncIntPacks", &mod);
			}
			return fun;
		}
		llvm::Function* GetIncIntUnpacksFunction(llvm::Module& mod)
		{
			auto fun = mod.getFunction("RT_NOM_STATS_IncIntUnpacks");
			if (fun == nullptr)
			{
				fun = Function::Create(FunctionType::get(Type::getVoidTy(LLVMCONTEXT), false), GlobalValue::LinkageTypes::ExternalLinkage, "RT_NOM_STATS_IncIntUnpacks", &mod);
			}
			return fun;
		}
		llvm::Function* GetIncIntBoxesFunction(llvm::Module& mod)
		{
			auto fun = mod.getFunction("RT_NOM_STATS_IncIntBoxes");
			if (fun == nullptr)
			{
				fun = Function::Create(FunctionType::get(Type::getVoidTy(LLVMCONTEXT), false), GlobalValue::LinkageTypes::ExternalLinkage, "RT_NOM_STATS_IncIntBoxes", &mod);
			}
			return fun;
		}
		llvm::Function* GetIncIntUnboxesFunction(llvm::Module& mod)
		{
			auto fun = mod.getFunction("RT_NOM_STATS_IncIntUnboxes");
			if (fun == nullptr)
			{
				fun = Function::Create(FunctionType::get(Type::getVoidTy(LLVMCONTEXT), false), GlobalValue::LinkageTypes::ExternalLinkage, "RT_NOM_STATS_IncIntUnboxes", &mod);
			}
			return fun;
		}
		llvm::Function* GetIncFloatPacksFunction(llvm::Module& mod)
		{
			auto fun = mod.getFunction("RT_NOM_STATS_IncFloatPacks");
			if (fun == nullptr)
			{
				fun = Function::Create(FunctionType::get(Type::getVoidTy(LLVMCONTEXT), false), GlobalValue::LinkageTypes::ExternalLinkage, "RT_NOM_STATS_IncFloatPacks", &mod);
			}
			return fun;
		}
		llvm::Function* GetIncFloatUnpacksFunction(llvm::Module& mod)
		{
			auto fun = mod.getFunction("RT_NOM_STATS_IncFloatUnpacks");
			if (fun == nullptr)
			{
				fun = Function::Create(FunctionType::get(Type::getVoidTy(LLVMCONTEXT), false), GlobalValue::LinkageTypes::ExternalLinkage, "RT_NOM_STATS_IncFloatUnpacks", &mod);
			}
			return fun;
		}
		llvm::Function* GetIncFloatBoxesFunction(llvm::Module& mod)
		{
			auto fun = mod.getFunction("RT_NOM_STATS_IncFloatBoxes");
			if (fun == nullptr)
			{
				fun = Function::Create(FunctionType::get(Type::getVoidTy(LLVMCONTEXT), false), GlobalValue::LinkageTypes::ExternalLinkage, "RT_NOM_STATS_IncFloatBoxes", &mod);
			}
			return fun;
		}
		llvm::Function* GetIncFloatUnboxesFunction(llvm::Module& mod)
		{
			auto fun = mod.getFunction("RT_NOM_STATS_IncFloatUnboxes");
			if (fun == nullptr)
			{
				fun = Function::Create(FunctionType::get(Type::getVoidTy(LLVMCONTEXT), false), GlobalValue::LinkageTypes::ExternalLinkage, "RT_NOM_STATS_IncFloatUnboxes", &mod);
			}
			return fun;
		}
		llvm::Function* GetGetTimestampFunction(llvm::Module& mod)
		{
			auto fun = mod.getFunction("RT_NOM_STATS_GetTimestamp");
			if (fun == nullptr)
			{
				fun = Function::Create(FunctionType::get(inttype(bitsin(TIMERTYPE)), false), GlobalValue::LinkageTypes::ExternalLinkage, "RT_NOM_STATS_GetTimestamp", &mod);
			}
			return fun;
		}
		llvm::Function* GetIncCastTimeFunction(llvm::Module& mod)
		{
			auto fun = mod.getFunction("RT_NOM_STATS_IncCastTime");
			if (fun == nullptr)
			{
				fun = Function::Create(FunctionType::get(Type::getVoidTy(LLVMCONTEXT), { inttype(bitsin(TIMERTYPE)) }, false), GlobalValue::LinkageTypes::ExternalLinkage, "RT_NOM_STATS_IncCastTime", &mod);
			}
			return fun;
		}

		llvm::Function* GetDebugFunction(llvm::Module& mod)
		{
			auto fun = mod.getFunction("RT_NOM_STATS_DebugLine");
			if (fun == nullptr)
			{
				fun = Function::Create(FunctionType::get(Type::getVoidTy(LLVMCONTEXT), { numtype(size_t), numtype(size_t), numtype(NomDebugPrintValueType), numtype(int64_t), numtype(decltype(NomDebugPrintLevel)) }, false), GlobalValue::LinkageTypes::ExternalLinkage, "RT_NOM_STATS_DebugLine", &mod);
			}
			return fun;
		}
		llvm::Function* GetIncAllocationsFunction(llvm::Module& mod)
		{
			auto fun = mod.getFunction("RT_NOM_STATS_IncAllocations");
			if (fun == nullptr)
			{
				fun = Function::Create(FunctionType::get(Type::getVoidTy(LLVMCONTEXT), { inttype(bitsin(AllocationType)) }, false), GlobalValue::LinkageTypes::ExternalLinkage, "RT_NOM_STATS_IncAllocations", &mod);
			}
			return fun;
		}

		llvm::Function* GetIncDirectClassMethodCalls(llvm::Module& mod)
		{
			auto fun = mod.getFunction("RT_NOM_STATS_IncDirectClassMethodCalls");
			if (fun == nullptr)
			{
				fun = Function::Create(FunctionType::get(Type::getVoidTy(LLVMCONTEXT), false), GlobalValue::LinkageTypes::ExternalLinkage, "RT_NOM_STATS_IncDirectClassMethodCalls", &mod);
			}
			return fun;
		}
		llvm::Function* GetIncInterfaceMethodCalls(llvm::Module& mod)
		{
			auto fun = mod.getFunction("RT_NOM_STATS_IncInterfaceMethodCalls");
			if (fun == nullptr)
			{
				fun = Function::Create(FunctionType::get(Type::getVoidTy(LLVMCONTEXT), false), GlobalValue::LinkageTypes::ExternalLinkage, "RT_NOM_STATS_IncInterfaceMethodCalls", &mod);
			}
			return fun;
		}
		llvm::Function* GetIncExtendedInterfaceMethodCalls(llvm::Module& mod)
		{
			auto fun = mod.getFunction("RT_NOM_STATS_IncExtendedInterfaceMethodCalls");
			if (fun == nullptr)
			{
				fun = Function::Create(FunctionType::get(Type::getVoidTy(LLVMCONTEXT), false), GlobalValue::LinkageTypes::ExternalLinkage, "RT_NOM_STATS_IncExtendedInterfaceMethodCalls", &mod);
			}
			return fun;
		}
		llvm::Function* GetIncTypedRawInvokes(llvm::Module& mod)
		{
			auto fun = mod.getFunction("RT_NOM_STATS_IncTypedRawInvokes");
			if (fun == nullptr)
			{
				fun = Function::Create(FunctionType::get(Type::getVoidTy(LLVMCONTEXT), false), GlobalValue::LinkageTypes::ExternalLinkage, "RT_NOM_STATS_IncTypedRawInvokes", &mod);
			}
			return fun;
		}
		llvm::Function* GetIncFinalInstanceMethodCalls(llvm::Module& mod)
		{
			auto fun = mod.getFunction("RT_NOM_STATS_IncFinalInstanceMethodCalls");
			if (fun == nullptr)
			{
				fun = Function::Create(FunctionType::get(Type::getVoidTy(LLVMCONTEXT), false), GlobalValue::LinkageTypes::ExternalLinkage, "RT_NOM_STATS_IncFinalInstanceMethodCalls", &mod);
			}
			return fun;
		}
		llvm::Function* GetIncStaticMethodCalls(llvm::Module& mod)
		{
			auto fun = mod.getFunction("RT_NOM_STATS_IncStaticMethodCalls");
			if (fun == nullptr)
			{
				fun = Function::Create(FunctionType::get(Type::getVoidTy(LLVMCONTEXT), false), GlobalValue::LinkageTypes::ExternalLinkage, "RT_NOM_STATS_IncStaticMethodCalls", &mod);
			}
			return fun;
		}
		llvm::Function* GetIncDynamicInvokes(llvm::Module& mod)
		{
			auto fun = mod.getFunction("RT_NOM_STATS_IncDynamicInvokes");
			if (fun == nullptr)
			{
				fun = Function::Create(FunctionType::get(Type::getVoidTy(LLVMCONTEXT), false), GlobalValue::LinkageTypes::ExternalLinkage, "RT_NOM_STATS_IncDynamicInvokes", &mod);
			}
			return fun;
		}
		llvm::Function* GetIncDynamicMethodCalls(llvm::Module& mod)
		{
			auto fun = mod.getFunction("RT_NOM_STATS_IncDynamicMethodCalls");
			if (fun == nullptr)
			{
				fun = Function::Create(FunctionType::get(Type::getVoidTy(LLVMCONTEXT), false), GlobalValue::LinkageTypes::ExternalLinkage, "RT_NOM_STATS_IncDynamicMethodCalls", &mod);
			}
			return fun;
		}
		llvm::Function* GetIncDynamicFieldLookups(llvm::Module& mod)
		{
			auto fun = mod.getFunction("RT_NOM_STATS_IncDynamicFieldLookups");
			if (fun == nullptr)
			{
				fun = Function::Create(FunctionType::get(Type::getVoidTy(LLVMCONTEXT), false), GlobalValue::LinkageTypes::ExternalLinkage, "RT_NOM_STATS_IncDynamicFieldLookups", &mod);
			}
			return fun;
		}
	}
}
