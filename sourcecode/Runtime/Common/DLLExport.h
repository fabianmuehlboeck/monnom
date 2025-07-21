#pragma once
//#ifdef LLVM_ON_WIN32
#ifdef _WIN32
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT
//#define DLLEXPORT __attribute__((visibility("default"))) /*__cdecl*/
#endif