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
#include "llvm/Support/DynamicLibrary.h"
#include "VoidClass.h"
#include "NomClassType.h"

namespace Nom
{
	namespace Runtime
	{
#ifdef _WIN32
		double find_timer_frequency() noexcept(false)
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
				timer_frequency = (double)(ttf.QuadPart);
			}
			return timer_frequency;
		}
#endif
	}
}

extern "C" DLLEXPORT void* LIB_NOM_Timer_PrintDifference_1(void* timer) noexcept(false)
{
#ifdef _WIN32
	char* valueptr = ((char*)timer) + sizeof(LARGE_INTEGER);
	LARGE_INTEGER value = *((LARGE_INTEGER*)valueptr);
	LARGE_INTEGER current;
	LARGE_INTEGER difference;
	if (QueryPerformanceCounter(&current))
	{
		difference.QuadPart = current.QuadPart - value.QuadPart;
		double floatdiff = ((double)(difference.QuadPart)) / Nom::Runtime::find_timer_frequency();
		printf("%f Seconds\n", floatdiff);
		return (void*)((intptr_t)(Nom::Runtime::GetVoidObject()));
	}
	else
	{
		throw new std::exception();
		//ThrowException(&stackframe, "Could not retrieve thread timings!");
	}
#else
#ifdef CLOCK_THREAD_CPUTIME_ID
	struct timespec current;
	struct timespec* myvalptr = (struct timespec*)(((char*)timer) + sizeof(intptr_t));
	if (clock_gettime(CLOCK_THREAD_CPUTIME_ID, &current) == 0)
	{
		long differenceMS = ((current.tv_sec - myvalptr->tv_sec) * 10000) + ((current.tv_nsec - myvalptr->tv_nsec) / 100000);
		double floatdiff = ((double)differenceMS) / 10000.0;
		printf("%f Seconds\n", floatdiff);
		return (void*)((intptr_t)(Nom::Runtime::GetVoidObject()));
	}
	else
	{
		throw new std::exception();
	}
#else
	clock_t* myclock = (clock_t*)(((char*)timer) + sizeof(intptr_t));
	clock_t now = clock();
	long t = (long)(clock() - (*myclock);
	printf("%f Seconds, (%li ticks)\n", ((double)t) / CLOCKS_PER_SEC, t);
	return (void*)((intptr_t)(Nom::Runtime::GetVoidObject()));
#endif
#endif
}

extern "C" DLLEXPORT void* LIB_NOM_Timer_Constructor_0(void* timer) noexcept(false)
{
#ifdef _WIN32
	char* valueptr = ((char*)timer) + sizeof(LARGE_INTEGER);
	if (!QueryPerformanceCounter((LARGE_INTEGER*)valueptr))
	{
		throw new std::exception();
	}
#else
#ifdef CLOCK_THREAD_CPUTIME_ID
	struct timespec* myvalptr = (struct timespec*)(((char*)timer) + sizeof(intptr_t));
	if (clock_gettime(CLOCK_THREAD_CPUTIME_ID, myvalptr) != 0)
	{
		throw new std::exception();
	}
#else
	clock_t now = clock();
	clock_t* myvalptr = (clock_t*)(((char*)timer) + sizeof(intptr_t));
	*myvalptr = now;
#endif
#endif
	return timer;
}

namespace Nom
{
	namespace Runtime
	{
		NomTimerClass::NomTimerClass() : NomInterface("Timer_0"), NomClassInternal(new NomString("Timer_0"))
			//NomClass(NomConstants::AddString(NomString("Timer")), 0, NomConstants::AddSuperClass(NomConstants::AddClass(NomConstants::AddString(NomString("std")), NomConstants::AddString(NomString("Object"))), 0), 0, nullptr)
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

			//this->compiled = true;
			////this->preprocessed = true;

			//ConstantID voidStringID = NomConstants::AddString("Void");
			//ConstantID stdStringID = NomConstants::AddString("std");
			//ConstantID voidClassID = NomConstants::AddClass(stdStringID, voidStringID);
			//ConstantID voidClassTypeID = NomConstants::AddClassType(voidClassID, 0);
			//NomMethod* print = AddMethod("PrintDifference", "LIB_NOM_Timer_PrintDifference_1", 0, voidClassTypeID, 0, 0, true, true, true);
			////MethodTable = NomObjectClass::GetInstance()->MethodTable;
			////print->SetOffset(-1-MethodTable.size());
			////MethodTable.push_back(new NomMethodTableEntry(print, print->GetLLVMFunctionType(), print->GetOffset()));

			//NomConstructor* constructor = new NomConstructor(this, "LIB_NOM_Timer_Constructor", "LIB_NOM_Timer_Constructor_0", 0, 0, 0, true, true);
			//Constructors.push_back(constructor);
			////this->PreprocessInheritance();


			llvm::sys::DynamicLibrary::AddSymbol("LIB_NOM_Timer_Constructor_0", (void*)&LIB_NOM_Timer_Constructor_0);
			llvm::sys::DynamicLibrary::AddSymbol("LIB_NOM_Timer_PrintDifference_1", (void*)&LIB_NOM_Timer_PrintDifference_1);

		}
		NomTimerClass* NomTimerClass::GetInstance()
		{
			static NomTimerClass ntc;
			static bool once = true;
			if (once)
			{
				once = false;
				NomVoidClass::GetInstance();
				NomObjectClass::GetInstance();
				//ntc.PreprocessInheritance();
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
