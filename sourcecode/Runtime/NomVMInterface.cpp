#include "NomVMInterface.h"
#include "llvm/IR/Function.h"
#include "Context.h"
#include "Defs.h"
#include "NomConstants.h"
#include "llvm/IR/DerivedTypes.h"
#include "NomString.h"
#include "llvm/Support/DynamicLibrary.h"
#include <iostream>
#include <forward_list>
#include <cinttypes>
#include "NullClass.h"
#include "NomAlloc.h"
#include "LambdaHeader.h"
#include "RTVTable.h"
#include "RTCast.h"
#include "StructHeader.h"
#include "gcinclude_config.h"
#include "CastStats.h"
#include "RTSubtyping.h"

using namespace llvm;
using namespace Nom::Runtime;

//DLLEXPORT Nom::Runtime::ObjectHeader CPP_NOM_CreateInstance(const Nom::Runtime::RTClass cls, size_t argspace, /*int argcnt, const Nom::Runtime::RTTypeHead argsarr,*/ intptr_t * fields)
//{
//	using namespace Nom::Runtime;
//	unsigned char* alloc;
//	if (fields == nullptr)
//	{
//		alloc = (unsigned char *)GC_MALLOC(cls.getSize()+argspace);
//	}
//	else
//	{
//		alloc = (unsigned char*)GC_MALLOC(ObjectHeader::SizeOf() + argspace /*sizeof(RTTypeHead) * argcnt*/);
//	}
//	ObjectHeader instance(cls, (void*)alloc);// = new(alloc) ObjectHeader(cls, argcnt, fields);
//	if (fields == nullptr)
//	{
//		fields = (intptr_t *)(alloc + ObjectHeader::SizeOf() + argspace);
//		for (size_t i = cls.FieldCount(); i > 0; i--)
//		{
//			fields[i - 1] = (intptr_t)(NULLOBJ.Entry());
//		}
//	}
//	instance.Fields() = (intptr_t)fields;
//	//Nom::Runtime::RTTypeHead * args = (Nom::Runtime::RTTypeHead *)((*instance)->args);
//	//for (int i = argcnt - 1; i >= 0; i--)
//	//{
//	//	args[i] = argsarr[i];
//	//}
//	return instance;
//}

//llvm::Function * GetCppCreateInstance(llvm::Module *mod)
//{
//	Function *ret = mod->getFunction("CPP_NOM_CreateInstance");
//	if (ret == nullptr)
//	{
//		std::array<Type*, 4> createInstanceArgs = { REFTYPE, Type::getIntNTy(LLVMCONTEXT, bitsin(int)), REFTYPE, Type::getIntNTy(LLVMCONTEXT, bitsin(intptr_t))->getPointerTo() };
//		FunctionType *cppCreateInstanceFunType = FunctionType::get(REFTYPE, createInstanceArgs, false);
//		ret = Function::Create(cppCreateInstanceFunType, Function::ExternalLinkage, "CPP_NOM_CreateInstance", mod);
//		//NomJIT::Instance().RegisterGlobalMapping(ret,(void*) &CPP_NOM_CreateInstance);
//	}
//	return ret;
//}
//llvm::Function * GetCppLoadStringConstant(llvm::Module *mod)
//{
//	Function *ret = mod->getFunction("CPP_NOM_LoadStringConstant");
//	if (ret == nullptr)
//	{
//		//std::array<Type*, 1> oneInt = { Type::getIntNTy(LLVMCONTEXT, bitsin(ConstantID)) };
//		std::array<Type*, 1> oneInt = { LLVMCONSTANTINDEXTYPE };
//		FunctionType *cppLoadStringConstantFunType = FunctionType::get(REFTYPE, oneInt, false);
//		ret = Function::Create(cppLoadStringConstantFunType, Function::ExternalLinkage, "CPP_NOM_LoadStringConstant", mod);
//		//NomJIT::Instance().RegisterGlobalMapping(ret,(void*) &CPP_NOM_LoadStringConstant);
//	}
//	return ret;
//}
llvm::Function* GetPrint(llvm::Module* mod)
{
	Function* ret = mod->getFunction("CPP_NOM_Print");
	if (ret == nullptr)
	{
		std::array<Type*, 1> chararrpluslen = { { Type::getIntNTy(LLVMCONTEXT, bitsin(uint64_t))} };
		FunctionType* printFunType = FunctionType::get(Type::getVoidTy(LLVMCONTEXT), chararrpluslen, false);
		ret = Function::Create(printFunType, Function::ExternalLinkage, "CPP_NOM_Print", mod);
		//NomJIT::Instance().RegisterGlobalMapping(ret,(void*) &CPP_NOM_LoadStringConstant);
	}
	return ret;
}

llvm::Function* GetAlloc(llvm::Module* mod)
{
	Function* ret = mod->getFunction("CPP_NOM_GCALLOC");
	if (ret == nullptr)
	{
		FunctionType* allocFunType = FunctionType::get(POINTERTYPE, { INTTYPE }, false);
		ret = Function::Create(allocFunType, Function::ExternalLinkage, "CPP_NOM_GCALLOC", mod);
		//NomJIT::Instance().RegisterGlobalMapping(ret,(void*) &CPP_NOM_LoadStringConstant);
	}
	return ret;
}

llvm::Function* GetNewAlloc(llvm::Module* mod)
{
	Function* ret = mod->getFunction("CPP_NOM_NEWALLOC");
	if (ret == nullptr)
	{
		FunctionType* allocFunType = FunctionType::get(REFTYPE, { INTTYPE, INTTYPE }, false);
		ret = Function::Create(allocFunType, Function::ExternalLinkage, "CPP_NOM_NEWALLOC", mod);
		//NomJIT::Instance().RegisterGlobalMapping(ret,(void*) &CPP_NOM_LoadStringConstant);
	}
	return ret;
}

llvm::Function* GetClosureAlloc(llvm::Module* mod)
{
	Function* ret = mod->getFunction("CPP_NOM_CLOSUREALLOC");
	if (ret == nullptr)
	{
		FunctionType* allocFunType = FunctionType::get(LambdaHeader::GetLLVMType()->getPointerTo(), { INTTYPE, INTTYPE, INTTYPE }, false);
		ret = Function::Create(allocFunType, Function::ExternalLinkage, "CPP_NOM_CLOSUREALLOC", mod);
		//NomJIT::Instance().RegisterGlobalMapping(ret,(void*) &CPP_NOM_LoadStringConstant);
	}
	return ret;
}

llvm::Function* GetStructAlloc(llvm::Module* mod)
{
	Function* ret = mod->getFunction("CPP_NOM_STRUCTALLOC");
	if (ret == nullptr)
	{
		FunctionType* allocFunType = FunctionType::get(StructHeader::GetLLVMType()->getPointerTo(), { INTTYPE, INTTYPE, INTTYPE }, false);
		ret = Function::Create(allocFunType, Function::ExternalLinkage, "CPP_NOM_STRUCTALLOC", mod);
		//NomJIT::Instance().RegisterGlobalMapping(ret,(void*) &CPP_NOM_LoadStringConstant);
	}
	return ret;
}

llvm::Function* GetClassTypeAlloc(llvm::Module* mod)
{
	Function* ret = mod->getFunction("CPP_NOM_STRUCTALLOC");
	if (ret == nullptr)
	{
		FunctionType* allocFunType = FunctionType::get(RTClassType::GetLLVMType()->getPointerTo(), { INTTYPE }, false);
		ret = Function::Create(allocFunType, Function::ExternalLinkage, "CPP_NOM_CLASSTYPEALLOC", mod);
	}
	return ret;
}

//llvm::Function* GetPureLambdaInterfaceVtable(llvm::Module* mod)
//{
//	Function* fun = mod->getFunction("CPP_NOM_GetPureLambdaInterfaceVTable");
//	if (fun == nullptr)
//	{
//		auto ft = FunctionType::get(RTVTable::GetLLVMType()->getPointerTo(), { POINTERTYPE, POINTERTYPE, numtype(bool), TYPETYPE->getPointerTo(), RTSubtyping::TypeArgumentListStackType()->getPointerTo() }, false);
//		fun = Function::Create(ft, GlobalValue::LinkageTypes::ExternalLinkage, "CPP_NOM_GetPureLambdaInterfaceVTable", mod);
//	}
//	return fun;
//}

void GenerateLLVMDebugPrint(IRBuilder<>& builder, llvm::Module* mod, const std::string& str)
{
	static std::forward_list<std::string> strings;
	strings.push_front(str);
	uint64_t strptr = reinterpret_cast<uint64_t>(&(strings.front()));
	fprintf(stdout, "%" PRIu64 "\n", strptr);
	std::cout.flush();
	std::cout << *(reinterpret_cast<std::string*>(strptr));
	llvm::ConstantInt* pointerIntConstant = llvm::ConstantInt::get(Type::getIntNTy(LLVMCONTEXT, bitsin(uint64_t)), strptr, false);
	fprintf(stdout, "%" PRIu64 "\n", pointerIntConstant->getZExtValue());
	std::array<Value*, 1> args = { { pointerIntConstant } };
	builder.CreateCall(GetPrint(mod), args);
}

DLLEXPORT void InitVMInterface()
{
	//llvm::sys::DynamicLibrary::AddSymbol("CPP_NOM_CreateInstance", (void*)&CPP_NOM_CreateInstance);
	//llvm::sys::DynamicLibrary::AddSymbol("CPP_NOM_LoadStringConstant", (void*)&CPP_NOM_LoadStringConstant);
	//llvm::sys::DynamicLibrary::AddSymbol("CPP_NOM_Print", (void*)&CPP_NOM_Print);
	//std::array<Type*, 1> oneInt = { Type::getIntNTy(LLVMCONTEXT, sizeof(ConstantID)) };
	//FunctionType *cppLoadStringConstantFunType = FunctionType::get(Type::getDoubleTy(LLVMCONTEXT), oneInt, false);
	//CppLoadStringConstant = Function::Create(cppLoadStringConstantFunType, Function::ExternalLinkage, "CPP_NOM_LoadStringConstant");

	//std::array<Type*, 4> createInstanceArgs = { StructType::create(LLVMCONTEXT)->getPointerTo(), Type::getIntNTy(LLVMCONTEXT, sizeof(int)), StructType::create(LLVMCONTEXT)->getPointerTo(), Type::getIntNTy(LLVMCONTEXT, sizeof(intptr_t))->getPointerTo() };
	//FunctionType *cppCreateInstanceFunType = FunctionType::get(StructType::create(LLVMCONTEXT)->getPointerTo(), createInstanceArgs, false);
	//CppCreateInstance = Function::Create(cppCreateInstanceFunType, Function::ExternalLinkage, "CPP_NOM_CreateInstance");
}

extern "C" DLLEXPORT void* CPP_NOM_GCALLOC(size_t size)
{
	if (NomCastStats > 0)
	{
		RT_NOM_STATS_IncAllocations(AllocationType::General);
	}
	auto ret = gcalloc(size);
	return ret;
}

extern "C" DLLEXPORT void* CPP_NOM_NEWALLOC(size_t numfields, size_t numtargs)
{
	if (NomCastStats > 0)
	{
		RT_NOM_STATS_IncAllocations(AllocationType::Object);
	}
	auto ret = (void*)(((char**)gcalloc(ObjectHeader::SizeOf() + (sizeof(char*) * (numfields + numtargs)))) + numtargs);
	return ret;
}

extern "C" DLLEXPORT void* CPP_NOM_CLOSUREALLOC(size_t numfields, size_t numtargs, size_t numreservedargs)
{
	if (NomCastStats > 0)
	{
		RT_NOM_STATS_IncAllocations(AllocationType::Lambda);
	}
	auto ret = (void*)(((char**)gcalloc(LambdaHeader::SizeOf() + (sizeof(char*) * (numfields + numtargs + numreservedargs)))) + numreservedargs);
	return ret;
}

extern "C" DLLEXPORT void* CPP_NOM_STRUCTALLOC(size_t numfields, size_t numtargs, size_t numreservedargs)
{
	if (NomCastStats > 0)
	{
		RT_NOM_STATS_IncAllocations(AllocationType::Struct);
	}
	auto ret = (void*)(((char**)gcalloc(StructHeader::SizeOf() + (sizeof(char*) * (numfields + numtargs + numreservedargs)))) + numreservedargs);
	return ret;
}

extern "C" DLLEXPORT void* CPP_NOM_CLASSTYPEALLOC(size_t numtargs)
{
	if (NomCastStats > 0)
	{
		RT_NOM_STATS_IncAllocations(AllocationType::ClassType);
	}
	auto ret = (void*)(((char**)gcalloc(RTClassType::SizeOf() + (sizeof(char*) * (numtargs)))) + numtargs);
	return ret;
}

extern "C" DLLEXPORT void CPP_NOM_Print(uint64_t str)
{
	fprintf(stdout, "%" PRIu64 "\n", str);
	std::cout << "PRINT: ";
	std::cout.flush();
	std::cout << *(reinterpret_cast<std::string*>(str));
}

//DLLEXPORT Nom::Runtime::ObjectHeader * CPP_NOM_LoadStringConstant(ConstantID constantID)
//{
//	return NomConstants::GetString(constantID)->getObject();
//}

//extern "C" DLLEXPORT void* CPP_NOM_GetPureLambdaInterfaceVTable(const Nom::Runtime::NomLambda * lambda, const Nom::Runtime::NomInterface * iface, bool fitsTypeArgs, void* typeArgs, void* substitutions)
//{
//	return iface->GetPureLambdaVtable(lambda, fitsTypeArgs, typeArgs, substitutions);
//}