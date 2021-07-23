// Runtime.cpp : Defines the exported functions for the static library.
//
#include "gcinclude_config.h"
#include <iostream>
#include <string>
#include <locale>
#include "Runtime.h"
#include "NomConstants.h"
#include "Defs.h"
#include "NomClass.h"
#include "NomInstruction.h"
#include "instructions/LoadStringConstantInstruction.h"
#include "instructions/CallCheckedInstanceMethod.h"
#include "instructions/ReturnInstruction.h"
#include "NomModule.h"
#include "NomVMInterface.h"
#include "RTModule.h"
#include "NomJIT.h"
#include "ObjectClass.h"
#include "NomClassType.h"
#include "Loader.h"
#include "LibraryVersion.h"
#include "StringClass.h"
#include "IntClass.h"
#include "FloatClass.h"
#include "BoolClass.h"
#include "NullClass.h"
#include "VoidClass.h"
#include "FunInterface.h"
#include "TimerClass.h"
#include "llvm/ADT/Triple.h"
#include "llvm/Analysis/CallGraph.h"
#include "llvm/Analysis/CallGraphSCCPass.h"
#include "llvm/Analysis/LoopPass.h"
#include "llvm/Analysis/RegionPass.h"
#include "llvm/Analysis/TargetLibraryInfo.h"
#include "llvm/Analysis/TargetTransformInfo.h"
#include "llvm/Bitcode/BitcodeWriterPass.h"
//#include "llvm/CodeGen/CommandFlags.inc"
#include "llvm/CodeGen/TargetPassConfig.h"
#include "llvm/Config/llvm-config.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/DebugInfo.h"
#include "llvm/IR/IRPrintingPasses.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/LegacyPassNameParser.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Verifier.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/InitializePasses.h"
#include "llvm/LinkAllIR.h"
#include "llvm/LinkAllPasses.h"
#include "llvm/MC/SubtargetFeature.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/InitLLVM.h"
#include "llvm/Support/PluginLoader.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/SystemUtils.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/ToolOutputFile.h"
#include "llvm/Support/YAMLTraits.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Transforms/Coroutines.h"
#include "llvm/Transforms/IPO/AlwaysInliner.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/Transforms/Instrumentation/InstrProfiling.h"
#include "CastStats.h"
#include "RTConfig.h"
#include "IComparableInterface.h"
#include "IPairInterface.h"
#include "FunctionTimings.h"
#include "IEnumerableInterface.h"
#include "IEnumeratorInterface.h"
#include "ArrayListClass.h"
#include "MathClass.h"
#include "IMT.h"


// This is an example of an exported variable
int nRuntime = 0;
static int _isInWarmup = 0;

using namespace Nom::Runtime;
using namespace std::literals;
using namespace std::string_literals;
using namespace std::literals::string_literals;

// This is an example of an exported function.
int fnRuntime(void)
{
	return 0;
}

// This is the constructor of a class that has been exported.
// see Runtime.h for the class definition
CRuntime::CRuntime()
{
	return;
}

int test()
{
	InitVMInterface();
	NomProgram * prog = new NomProgram();
	NomModule mod(prog);
	NomConstantContext context;
	context.AddString(1, NomString("Hello World!"));
	context.AddString(2, NomString("String"));
	context.AddString(12, NomString("std"));
	context.AddClass(3, 12, 2);
	context.AddString(4, NomString("Main"));
	context.AddString(5, NomString("Object"));
	context.AddClass(6, 12, 5);
	context.AddClassType(7, 6, 0);
	context.AddString(8, NomString("Print"));
	context.AddMethod(9, 3, 8, 0, 0);
	context.AddString(10, NomString("main"));
	context.AddClassType(11, 5, 0);
	NomClassLoaded* mainclass = prog->AddClass(context[4], 0, context[7], 0, nullptr);
	mod.Classes.emplace_front(mainclass);
	std::unique_ptr<NomInstruction> i1 = std::unique_ptr<NomInstruction>(new LoadStringConstantInstruction(2, context[1]));
	std::unique_ptr<NomInstruction> i2 = std::unique_ptr<NomInstruction>(new CallCheckedInstanceMethod(3, context[9], 0, 2));
	std::unique_ptr<NomInstruction> i3 = std::unique_ptr<NomInstruction>(new ReturnInstruction(2));
	NomStaticMethodLoaded* meth =mainclass->AddStaticMethod(NomConstants::GetString(context[10])->GetText()->ToStdString(), mainclass->GetName()->ToStdString() + "." + NomConstants::GetString(context[10])->GetText()->ToStdString(), 0, context[7], 0, 4);
	meth->AddInstruction(i1.get());
	meth->AddInstruction(i2.get());
	meth->AddInstruction(i3.get());
	std::cout << "\nCompiling...\n";
	const RTModule rtm(&mod);
	std::cout << "\nCompilation complete\n";
	auto MainSymbol = NomJIT::Instance().lookup(*meth->GetSymbolName());
	assert(MainSymbol && "Function not found");
	std::cout << "\nMain method symbol found\n";
	void* (*mainmeth)() = (void* (*)())(intptr_t)MainSymbol->getAddress();
	std::cout << "\nGot main method pointer\n";
	mainmeth();
	std::cout << "\nMain method executed\n";
	int x;
	std::cin >> x;
	return 0;
}

int run(const std::vector<std::string> args)
{
	const Loader* loader = Loader::GetInstance();
	const LibraryVersion* const lib = loader->GetLibrary(&NomMainClassName);
	const NomClass* mainCls = lib->GetClass(lib->GetMainClassName());
	const NomStaticMethod* mainMethod = mainCls->GetStaticMethod(lib->GetMainMethodName(), TypeList(), TypeList()).Elem;
	auto MainSymbol = NomJIT::Instance().lookup(*mainMethod->GetSymbolName());
	void* (*mainmeth)() = (void* (*)())(intptr_t)MainSymbol->getAddress();

	_isInWarmup = 1;
	for (int i = 0; i < NomWarmupRuns; i++)
	{
		GC_gcollect();
		mainmeth();
		RTModule::ClearCaches();
	}
	_isInWarmup = 0;
	GC_gcollect();
	mainmeth();
	if (NomTimings)
	{
		Nom::Runtime::PrintCastStats();
	}
	if (NomFunctionTimingLevel>2)
	{
		Nom::Runtime::PrintFunctionTimings();
	}
	if (NomRuntimeStopAtEnd)
	{
		std::cin.ignore();
	}
	return 0;
}

void initLibrary()
{
	//FunInterface::GetFun(0);
	//Nom::Runtime::NomObjectClass::GetInstance();
	//Nom::Runtime::NomVoidClass::GetInstance();
	//Nom::Runtime::NomNullClass::GetInstance();
	//Nom::Runtime::NomStringClass::GetInstance();
	//Nom::Runtime::NomIntClass::GetInstance();
	//Nom::Runtime::NomFloatClass::GetInstance();
	//Nom::Runtime::NomBoolClass::GetInstance();
	//Nom::Runtime::NomTimerClass::GetInstance();
	//Nom::Runtime::IComparableInterface::GetInstance();
	//Nom::Runtime::IEnumeratorInterface::GetInstance();
	//Nom::Runtime::IEnumerableInterface::GetInstance();
	//Nom::Runtime::IPairInterface::GetInstance();
	//Nom::Runtime::ArrayListClass::GetInstance();
	//Nom::Runtime::ArrayListEnumeratorClass::GetInstance();
	//Nom::Runtime::NomMathClass::GetInstance();
}


int main(int argc, char** args)
{
	parseArguments(argc, args);
	GC_INIT();

	llvm::InitializeNativeTarget();
	llvm::InitializeNativeTargetAsmPrinter();
	llvm::InitializeNativeTargetAsmParser();

	PassRegistry& Registry = *PassRegistry::getPassRegistry();
	initializeCore(Registry);
	initializeCoroutines(Registry);
	initializeScalarOpts(Registry);
	initializeObjCARCOpts(Registry);
	initializeVectorization(Registry);
	initializeIPO(Registry);
	initializeAnalysis(Registry);
	initializeTransformUtils(Registry);
	initializeInstCombine(Registry);
	initializeAggressiveInstCombine(Registry);
	initializeInstrumentation(Registry);
	initializeTarget(Registry);
	// For codegen passes, only passes that do IR to IR transformation are
	// supported.
	initializeExpandMemCmpPassPass(Registry);
	initializeScalarizeMaskedMemIntrinLegacyPassPass(Registry);
	initializeCodeGenPreparePass(Registry);
	initializeAtomicExpandPass(Registry);
	initializeRewriteSymbolsLegacyPassPass(Registry);
	initializeWinEHPreparePass(Registry);
	//initializeDwarfEHPreparePass(Registry);
	initializeSafeStackLegacyPassPass(Registry);
	initializeSjLjEHPreparePass(Registry);
	initializePreISelIntrinsicLoweringLegacyPassPass(Registry);
	initializeGlobalMergePass(Registry);
	initializeIndirectBrExpandPassPass(Registry);
	initializeInterleavedAccessPass(Registry);
	initializeEntryExitInstrumenterPass(Registry);
	initializePostInlineEntryExitInstrumenterPass(Registry);
	initializeTailCallElimPass(Registry);
	initializeUnreachableBlockElimLegacyPassPass(Registry);
	initializeExpandReductionsPass(Registry);
	initializeWasmEHPreparePass(Registry);
	initializeWriteBitcodePassPass(Registry);

	initLibrary();
	if (argc == 1)
	{
		return test();
	}
	else
	{
		//std::vector<std::string> argvec;
		//for (int i = 1; i < argc; i++)
		//{
		//	argvec.push_back(args[i]);
		//}
		return run(NomApplicationArgs);
	}
}

int isInWarmup()
{
	return _isInWarmup;
}

