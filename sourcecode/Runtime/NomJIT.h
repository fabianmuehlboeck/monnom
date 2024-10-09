#pragma once

#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/ExecutionEngine/JITSymbol.h"
#include "llvm/ExecutionEngine/Orc/CompileUtils.h"
#include "llvm/ExecutionEngine/Orc/Core.h"
#include "llvm/ExecutionEngine/Orc/ExecutionUtils.h"
#include "llvm/ExecutionEngine/Orc/IRCompileLayer.h"
#include "llvm/ExecutionEngine/Orc/IRTransformLayer.h"
#include "llvm/ExecutionEngine/Orc/JITTargetMachineBuilder.h"
#include "llvm/ExecutionEngine/Orc/RTDyldObjectLinkingLayer.h"
#include "llvm/ExecutionEngine/Orc/ObjectLinkingLayer.h"
#include "llvm/ExecutionEngine/SectionMemoryManager.h"
#include "llvm/ExecutionEngine/JITLink/JITLink.h"
#include "llvm/ExecutionEngine/JITLink/EHFrameSupport.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/InstCombine/InstCombine.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Scalar/GVN.h"
#include "llvm/ExecutionEngine/JITEventListener.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/Support/SourceMgr.h"
#include "Context.h"
#include <memory>
#include <set>

#include "llvm/ExecutionEngine/Orc/LLJIT.h"

#ifdef _WIN64
#define NATIVEOS "Windows"
#else
#ifdef __unix__
#define NATIVEOS "Unix"
#else
#define NATIVEOS "Unknown"
#endif
#endif

//
//#include "llvm/ExecutionEngine/ExecutionEngine.h"
//#include "llvm/ExecutionEngine/JITSymbol.h"
//#include "llvm/ExecutionEngine/RTDyldMemoryManager.h"
//#include "llvm/ExecutionEngine/SectionMemoryManager.h"
//#include "llvm/ExecutionEngine/Orc/IRTransformLayer.h"
//#include "llvm/ExecutionEngine/Orc/CompileUtils.h"
//#include "llvm/ExecutionEngine/Orc/IRCompileLayer.h"
//#include "llvm/ExecutionEngine/Orc/LambdaResolver.h"
//#include "llvm/ExecutionEngine/Orc/RTDyldObjectLinkingLayer.h"
//#include "llvm/IR/DataLayout.h"
//#include "llvm/IR/Mangler.h"
//#include "llvm/Support/DynamicLibrary.h"
//#include "llvm/Target/TargetMachine.h"
//#include "llvm/ExecutionEngine/Orc/LLJIT.h"
//#include <algorithm>
//#include <memory>
//#include <string>
//#include <vector>
//#include <iostream>

namespace Nom
{
	namespace Runtime
	{
		void* GetVoidObject();
		class NomJIT
		{
		private:
			//llvm::orc::ExecutionSession ES;
			//llvm::orc::RTDyldObjectLinkingLayer ObjectLayer;
			//llvm::orc::IRCompileLayer CompileLayer;
			//llvm::orc::IRTransformLayer OptimizeLayer;

			//llvm::DataLayout DL;
			//llvm::orc::MangleAndInterner Mangle;
			llvm::orc::ThreadSafeContext Ctx;

			//llvm::orc::JITDylib& MainJD;
			std::unique_ptr<llvm::orc::LLJIT> lljit;
			std::set<std::string> loadedFiles;

		public:
			NomJIT(std::unique_ptr<llvm::orc::LLJIT>&& llj /*llvm::orc::JITTargetMachineBuilder JTMB, llvm::DataLayout DL*/);
			static llvm::Expected<std::unique_ptr<NomJIT>> Create();

			const llvm::DataLayout& getDataLayout() const { return lljit->getDataLayout(); }

			llvm::LLVMContext& getContext() { return *Ctx.getContext(); }

			llvm::Error addObjectFile(const std::string& fileName) {
				if (loadedFiles.find(fileName) != loadedFiles.end()) {
					return llvm::Error::success();
				}
				loadedFiles.insert(fileName);
				auto ofile = llvm::MemoryBuffer::getFile(fileName);
				auto r = lljit->addObjectFile(lljit->getMainJITDylib(), std::move(ofile.get()));
				return r;
			}

			llvm::Error addIRFile(const std::string& fileName) {
				if (loadedFiles.find(fileName) != loadedFiles.end()) {
					return llvm::Error::success();
				}
				loadedFiles.insert(fileName);
				auto smd = llvm::SMDiagnostic();
				auto module = parseIRFile(fileName, smd, LLVMCONTEXT);
				return lljit->addIRModule(llvm::orc::ThreadSafeModule(std::move(module), Ctx));
			}

			llvm::Error addModule(std::unique_ptr<llvm::Module> M) {
				return lljit->addIRModule(llvm::orc::ThreadSafeModule(std::move(M), Ctx));
				//return OptimizeLayer.add(MainJD, llvm::orc::ThreadSafeModule(std::move(M), Ctx));
			}


			llvm::Expected<llvm::JITEvaluatedSymbol> lookup(llvm::StringRef Name) {
				return lljit->lookup(Name);
				//return ES.lookup({ &MainJD }, Mangle(Name.str()));
			}


			//llvm::ExitOnError ExitOnError;
			//llvm::orc::ExecutionSession ES;
			//std::shared_ptr<llvm::orc::SymbolResolver> Resolver;
			//std::unique_ptr<llvm::TargetMachine> TM;
			//const llvm::DataLayout DL;
			//llvm::orc::RTDyldObjectLinkingLayer ObjectLayer;
			//llvm::orc::IRCompileLayer<decltype(ObjectLayer), llvm::orc::SimpleCompiler> CompileLayer;

			//using OptimizeFunction =
			//	std::function<std::unique_ptr<llvm::Module>(std::unique_ptr<llvm::Module>)>;

			//llvm::orc::IRTransformLayer<decltype(CompileLayer), OptimizeFunction> OptimizeLayer;
		public:
			static NomJIT& Instance() { static std::unique_ptr<NomJIT> instance(std::move(*Create())); return *instance; }
			//using ModuleHandle = llvm::orc::VModuleKey; //decltype(CompileLayer)::ModuleHandleT;

			//llvm::TargetMachine &getTargetMachine() { return *TM; }

			//const llvm::DataLayout &getDataLayout() const { return DL; }

			//ModuleHandle addModule(std::unique_ptr<llvm::Module> M) {
			//	auto jit = LLJITBuilder()..create();
			//	if (!jit)
			//	{
			//		return jit.takeError();
			//	}
			//	//// Build our symbol resolver:
			//	//// Lambda 1: Look back into the JIT itself to find symbols that are part of
			//	////           the same "logical dylib".
			//	//// Lambda 2: Search for external symbols in the host process.
			//	//auto Resolver = llvm::orc::createLambdaResolver(
			//	//	[&](const std::string &Name) {
			//	//	std::cout << "\nLooking up 1: "+Name+"\n";
			//	//	if (auto Sym = CompileLayer.findSymbol(Name, false))
			//	//		return Sym;
			//	//	return llvm::JITSymbol(nullptr);
			//	//},
			//	//	[](const std::string &Name) {
			//	//	std::cout << "\nLooking up 2: " + Name + "\n";
			//	//	if (auto SymAddr =
			//	//		llvm::RTDyldMemoryManager::getSymbolAddressInProcess(Name))
			//	//		return llvm::JITSymbol(SymAddr, llvm::JITSymbolFlags::Exported);
			//	//	return llvm::JITSymbol(nullptr);
			//	//});

			//	//// Add the set to the JIT with the resolver we created above and a newly
			//	//// created SectionMemoryManager.
			//	//auto mh = llvm::cantFail(CompileLayer.addModule(std::move(M),
			//	//	std::move(Resolver)));
			//	//mh->get()->finalize();
			//	//return mh;

			//	auto K = ES.allocateVModule();
			//	ExitOnError(OptimizeLayer.addModule(K, std::move(M)));
			//	ExitOnError(OptimizeLayer.emitAndFinalize(K));
			//	return K;
			//}

			//llvm::JITSymbol findSymbol(const std::string Name) {
			//	std::string MangledName;
			//	llvm::raw_string_ostream MangledNameStream(MangledName);
			//	llvm::Mangler::getNameWithPrefix(MangledNameStream, Name, DL);
			//	return OptimizeLayer.findSymbol(MangledNameStream.str(), false);
			//}

			llvm::JITTargetAddress getSymbolAddress(const std::string Name) {
				return lookup(Name)->getAddress();
			}

			//void * getAddressIn(ModuleHandle h, const std::string &Name)
			//{
			//	return (void *) llvm::cantFail(OptimizeLayer.findSymbolIn(h, Name, true).getAddress());
			//}

			//void removeModule(ModuleHandle H) {
			//	llvm::cantFail(OptimizeLayer.removeModule(H));
			//}

			//NomJIT();
			~NomJIT();

		private:
			static llvm::Expected<llvm::orc::ThreadSafeModule>
				optimizeModule(llvm::orc::ThreadSafeModule TSM, const llvm::orc::MaterializationResponsibility& R);
			//std::unique_ptr<llvm::Module> optimizeModule(std::unique_ptr<llvm::Module> M);
		};

	}
}