#include "FunctionTimings.h"
PUSHDIAGSUPPRESSION
#include "llvm/IR/Function.h"
POPDIAGSUPPRESSION
#include "CastStats.h"
#include "NomNameRepository.h"
#include "NomAlloc.h"
#include <vector>
#include <iostream>

static size_t timingBufferPos = 0;
static const std::string** ftdebugFunNames;
#ifdef _WIN32
static LONGLONG* functionTimingsArray;
#endif

struct FunctionTiming
{
public:
	FunctionTiming* Parent;
	size_t FunID;
	TIMERTYPE LastStartTime;
	TIMERTYPE AccTime;
};

static FunctionTiming* ftbuffer;

static FunctionTiming* timingAlloc()
{
	return ftbuffer++;
}
static void freeTimingAlloc()
{
	ftbuffer--;
}

[[clang::no_destroy]] static FunctionTiming baseFunction;
static FunctionTiming* currentFunction = &baseFunction;

DLLEXPORT void NOM_RT_FT_EnterFunction(size_t funid)
{
	FunctionTiming& ft = *timingAlloc();
	auto timestamp = RT_NOM_STATS_GetTimestamp();
	ft.FunID = funid;
	ft.LastStartTime = timestamp;
	ft.Parent = currentFunction;
#ifdef _WIN32
	ft.AccTime.QuadPart = 0;
	currentFunction->AccTime.QuadPart += timestamp.QuadPart - ft.LastStartTime.QuadPart;
#endif
	currentFunction = &ft;
}
DLLEXPORT void NOM_RT_FT_LeaveFunction()
{
	auto timestamp = RT_NOM_STATS_GetTimestamp();
#ifdef _WIN32
	LONGLONG difference = currentFunction->AccTime.QuadPart + (timestamp.QuadPart - currentFunction->LastStartTime.QuadPart);
	functionTimingsArray[currentFunction->FunID] += difference;
#endif
	currentFunction = currentFunction->Parent;
	currentFunction->LastStartTime = timestamp;
	freeTimingAlloc();
}

#ifdef _WIN32
static double find_timer_frequency()
{
	static double timer_frequency;
	static bool once = true;
	if (once)
	{
		once = false;
#ifdef _WIN32
		LARGE_INTEGER ttf;
		if (!QueryPerformanceFrequency(&ttf))
		{
			throw new std::exception();
		}
		timer_frequency = static_cast<double>(ttf.QuadPart);
#else
		timer_frequency = 0;
#endif
	}
	return timer_frequency;
}
#endif


namespace Nom
{
	namespace Runtime
	{
		llvm::Function* GetEnterFunctionFunction(llvm::Module& mod)
		{
			auto fun = mod.getFunction("NOM_RT_FT_EnterFunction");
			if (fun == nullptr)
			{
				fun = Function::Create(FunctionType::get(Type::getVoidTy(LLVMCONTEXT), { numtype(size_t) }, false), GlobalValue::LinkageTypes::ExternalLinkage, "NOM_RT_FT_EnterFunction", &mod);
			}
			return fun;
		}
		llvm::Function* GetLeaveFunctionFunction(llvm::Module& mod)
		{
			auto fun = mod.getFunction("NOM_RT_FT_LeaveFunction");
			if (fun == nullptr)
			{
				fun = Function::Create(FunctionType::get(Type::getVoidTy(LLVMCONTEXT), false), GlobalValue::LinkageTypes::ExternalLinkage, "NOM_RT_FT_LeaveFunction", &mod);
			}
			return fun;
		}
		void InitFunctionTimings()
		{
			auto maxid = NomNameRepository::ProfilingInstance().GetMaxID();
			ftdebugFunNames = makenmalloc(const std::string*, (maxid + 1));
#ifdef _WIN32
			functionTimingsArray = makenmalloc(LONGLONG, (maxid + 1));
#endif
			for (decltype(maxid) i = 1; i <= maxid; i++)
			{
				ftdebugFunNames[i] = NomNameRepository::ProfilingInstance().GetNameFromID(i);
#ifdef _WIN32
				functionTimingsArray[i] = 0;
#endif
			}
			ftbuffer = makenmalloc(FunctionTiming, 1000000);
		}

		void PrintFunctionTimings()
		{
			auto maxid = NomNameRepository::ProfilingInstance().GetMaxID();
#ifdef _WIN32
			double frequency = find_timer_frequency();
#endif

			std::cout << "\n\nFunction Timings:\n----------------------\n";
			std::cout.precision(15);
			for (decltype(maxid) i = 1; i <= maxid; i++)
			{
#ifdef _WIN32
				if (functionTimingsArray[i] > 0)
				{
					std::cout << (*ftdebugFunNames[i]) << ": " << std::fixed << ((static_cast<double>(functionTimingsArray[i])) / frequency) << " seconds\n";
				}
#endif
			}
		}
	}
}
