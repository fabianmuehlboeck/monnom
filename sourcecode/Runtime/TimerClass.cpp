#include "TimerClass.h"
#include "ObjectClass.h"
#include "NomConstructor.h"
#include "NomJIT.h"
#include "DLLExport.h"
#include <time.h>
#ifdef _WIN32
#include <Windows.h>
#endif
#include "NomMethodTableEntry.h"
PUSHDIAGSUPPRESSION
#include "llvm/Support/DynamicLibrary.h"
POPDIAGSUPPRESSION
#include "VoidClass.h"
#include "NomClassType.h"
#include "Runtime.h"

namespace Nom
{
	namespace Runtime
	{
#ifdef _WIN32
		static double find_timer_frequency() noexcept(false)
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
	}
}

extern "C" DLLEXPORT void* LIB_NOM_Timer_PrintDifference_1(void* timer) noexcept(false)
{
#ifdef _WIN32
	char* valueptr = (reinterpret_cast<char*>(timer)) + sizeof(LARGE_INTEGER);
	LARGE_INTEGER value = *(reinterpret_cast<LARGE_INTEGER*>(valueptr));
	LARGE_INTEGER current;
	LARGE_INTEGER difference;
	if (QueryPerformanceCounter(&current))
	{
		difference.QuadPart = current.QuadPart - value.QuadPart;
		double floatdiff = (static_cast<double>(difference.QuadPart)) / Nom::Runtime::find_timer_frequency();
		if (!isInWarmup())
		{
			printf("%f Seconds\n", floatdiff);
		}
		return (Nom::Runtime::GetVoidObject());
	}
	else
	{
		throw new std::exception();
		//ThrowException(&stackframe, "Could not retrieve thread timings!");
	}
#else
#ifdef CLOCK_THREAD_CPUTIME_ID
	struct timespec current;
	struct timespec* myvalptr = reinterpret_cast<struct timespec*>((reinterpret_cast<char*>(timer)) + sizeof(intptr_t));
	if (clock_gettime(CLOCK_THREAD_CPUTIME_ID, &current) == 0)
	{
		long differenceMS = ((current.tv_sec - myvalptr->tv_sec) * 10000) + ((current.tv_nsec - myvalptr->tv_nsec) / 100000);
		double floatdiff = (static_cast<double>(differenceMS)) / 10000.0;
		if (!isInWarmup())
		{
			printf("%f Seconds\n", floatdiff);
		}
		return (Nom::Runtime::GetVoidObject());
	}
	else
	{
		throw new std::exception();
	}
#else
	clock_t* myclock = reinterpret_cast<clock_t*>((reinterpret_cast<char*>(timer)) + sizeof(intptr_t));
	clock_t now = clock();
	long t = (long)(clock() - (*myclock);
	auto tdiff = ((double)t) / CLOCKS_PER_SEC;
	if (!isInWarmup())
	{
		printf("%f Seconds, (%li ticks)\n", tdiff, t);
	}
	return (Nom::Runtime::GetVoidObject());
#endif
#endif
}

extern "C" DLLEXPORT void* LIB_NOM_Timer_Constructor_0(void* timer) noexcept(false)
{
#ifdef _WIN32
	char* valueptr = (reinterpret_cast<char*>(timer)) + sizeof(LARGE_INTEGER);
	if (!QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(valueptr)))
	{
		throw new std::exception();
	}
#else
#ifdef CLOCK_THREAD_CPUTIME_ID
	struct timespec* myvalptr = reinterpret_cast<struct timespec*>((reinterpret_cast<char*>(timer)) + sizeof(intptr_t));
	if (clock_gettime(CLOCK_THREAD_CPUTIME_ID, myvalptr) != 0)
	{
		throw new std::exception();
	}
#else
	clock_t now = clock();
	clock_t* myvalptr = reinterpret_cast<clock_t*>((reinterpret_cast<char*>(timer)) + sizeof(intptr_t));
	*myvalptr = now;
#endif
#endif
	return timer;
}

namespace Nom
{
	namespace Runtime
	{
		NomTimerClass::NomTimerClass() : NomInterface(), NomClassInternal(new NomString("Timer_0"))
		{
			this->SetDirectTypeParameters();
			this->SetSuperClass();
			this->SetSuperInterfaces();

			NomMethodInternal* printDifference = new NomMethodInternal(this, "PrintDifference", "LIB_NOM_Timer_PrintDifference_1", true);
			printDifference->SetDirectTypeParameters();
			printDifference->SetArgumentTypes();
			printDifference->SetReturnType(NomVoidClass::GetInstance()->GetType());

			this->AddMethod(printDifference);

			NomConstructorInternal* constructor = new NomConstructorInternal("LIB_NOM_Timer_Constructor_0", this);
			constructor->SetDirectTypeParameters();
			constructor->SetArgumentTypes();

			this->AddConstructor(constructor);

			llvm::sys::DynamicLibrary::AddSymbol("LIB_NOM_Timer_Constructor_0", reinterpret_cast<void*>(& LIB_NOM_Timer_Constructor_0));
			llvm::sys::DynamicLibrary::AddSymbol("LIB_NOM_Timer_PrintDifference_1", reinterpret_cast<void*>(&LIB_NOM_Timer_PrintDifference_1));

		}
		NomTimerClass* NomTimerClass::GetInstance()
		{
			[[clang::no_destroy]] static NomTimerClass ntc;
			static bool once = true;
			if (once)
			{
				once = false;
				NomVoidClass::GetInstance();
				NomObjectClass::GetInstance();
			}
			return &ntc;
		}
		NomTimerClass::~NomTimerClass() {}
		size_t NomTimerClass::GetFieldCount() const
		{
#ifdef _WIN32
			size_t ret = sizeof(LARGE_INTEGER) / sizeof(intptr_t);
			if (sizeof(LARGE_INTEGER) % sizeof(intptr_t) != 0)
			{
				ret += 1;
			}
			return ret;
#else
#ifdef CLOCK_THREAD_CPUTIME_ID
			size_t ret = sizeof(struct timespec) / sizeof(intptr_t);
			if (sizeof(struct timespec) % sizeof(intptr_t) != 0)
			{
				ret += 1;
			}
			return ret;
#else
			size_t ret = sizeof(clock_t) / sizeof(intptr_t);
			if (sizeof(clock_t) % sizeof(intptr_t) != 0)
			{
				ret += 1;
			}
			return ret;
#endif
#endif
		}
	}
}
