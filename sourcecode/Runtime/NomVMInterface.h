#pragma once

#include "Defs.h"
#include "RTTypeHead.h"
#include "RTClass.h"
#include "ObjectHeader.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"
#include <string>
#include "DLLExport.h"

extern llvm::Function * GetPrint(llvm::Module *mod);
extern llvm::Function * GetAlloc(llvm::Module *mod);

extern llvm::Function* GetNewAlloc(llvm::Module* mod);

extern llvm::Function* GetClosureAlloc(llvm::Module* mod);

extern llvm::Function* GetRecordAlloc(llvm::Module* mod);

extern void GenerateLLVMDebugPrint(llvm::IRBuilder<> &builder, llvm::Module *mod, const std::string &str);

extern "C" DLLEXPORT void InitVMInterface();

extern "C" DLLEXPORT void *CPP_NOM_GCALLOC(size_t size);

extern "C" DLLEXPORT void* CPP_NOM_NEWALLOC(size_t numfields, size_t numtargs);

extern "C" DLLEXPORT void* CPP_NOM_CLOSUREALLOC(size_t numfields, size_t numtargs);

extern "C" DLLEXPORT void* CPP_NOM_RECORDALLOC(size_t numfields, size_t numtargs);

extern "C" DLLEXPORT void* CPP_NOM_CLASSTYPEALLOC(size_t numtargs);

extern "C" DLLEXPORT void CPP_NOM_Print(uint64_t str);

