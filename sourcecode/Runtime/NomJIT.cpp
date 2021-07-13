#include "NomJIT.h"
#include "NomVMInterface.h"
#include "llvm/Pass.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/InstCombine/InstCombine.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Scalar/GVN.h"
#include "llvm/Transforms/AggressiveInstCombine/AggressiveInstCombine.h"
#include "llvm/Analysis/TargetTransformInfo.h"
#include "llvm/Analysis/TargetTransformInfoImpl.h"
#include "llvm/Analysis/AliasAnalysis.h"
#include "llvm/Analysis/AliasAnalysisEvaluator.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/Transforms/IPO.h"
#include "llvm/IR/Verifier.h"
#include "NomJITLight.h"
#include <iostream>
#include <atomic>
#include "llvm/Support/raw_os_ostream.h"
#include "llvm/Support/raw_ostream.h"
#include "StringClass.h"
#include <forward_list>
#include "RTConfig.h"
#include "CompileHelpers.h"
#include "RTOutput.h"
#include <fstream>
#include "CastStats.h"
#include "NomNameRepository.h"
#include "FunctionTimings.h"
#include "llvm/ExecutionEngine/Orc/IRTransformLayer.h"
#include <jitprofiling.h>
#include "llvm/Object/SymbolSize.h"

using namespace llvm;
using namespace llvm::orc;
using namespace Nom::Runtime;
using namespace std;
//
//NomJIT::NomJIT() :  TM(EngineBuilder().selectTarget()), DL(TM->createDataLayout()),
//ObjectLayer([]() { return std::make_shared<SectionMemoryManager>(); }),
//CompileLayer(ObjectLayer, SimpleCompiler(*TM)) {
//	llvm::sys::DynamicLibrary::LoadLibraryPermanently(nullptr);
//}

namespace Nom
{
	namespace Runtime
	{
		class NomJITEventListener : public llvm::JITEventListener
		{
		public:
			static NomJITEventListener& GetListener() {
				static NomJITEventListener listener;
				return listener;
			}

			virtual void notifyObjectLoaded(ObjectKey K, const object::ObjectFile& Obj,
				const RuntimeDyld::LoadedObjectInfo& L) override
			{
				//if (iJIT_IsProfilingActive() == iJIT_SAMPLING_ON) {
					for (auto &pair : computeSymbolSizes(Obj)) {
						auto Sym = pair.first;
						Expected<llvm::object::SymbolRef::Type> SymTypeOrErr = Sym.getType();
						if (!SymTypeOrErr) {
							// TODO: Actually report errors helpfully.
							consumeError(SymTypeOrErr.takeError());
							continue;
						}
						llvm::object::SymbolRef::Type SymType = *SymTypeOrErr;
						if (SymType != llvm::object::SymbolRef::ST_Function)
							continue;

						Expected<llvm::StringRef> Name = Sym.getName();
						if (!Name) {
							// TODO: Actually report errors helpfully.
							consumeError(Name.takeError());
							continue;
						}

						Expected<uint64_t> AddrOrErr = Sym.getAddress();
						if (!AddrOrErr) {
							// TODO: Actually report errors helpfully.
							consumeError(AddrOrErr.takeError());
							continue;
						}
						uint64_t Addr = AddrOrErr.get();

						auto section = Sym.getSection();

						if (section)
						{
							auto sect = section.get();
							if (sect != Obj.section_end())
							{
								Addr += L.getSectionLoadAddress(*sect);
							}
						}
						uint64_t Size = pair.second;

						iJIT_Method_Load jmethod = { 0 };
						jmethod.method_id = iJIT_GetNewMethodID();
						jmethod.method_name = const_cast<char*>(Name->data());
						jmethod.method_load_address = (void*)Addr;
						jmethod.method_size = Size;

						iJIT_NotifyEvent(iJVM_EVENT_TYPE_METHOD_LOAD_FINISHED,
							(void*)&jmethod);
						//iJIT_NotifyEvent(iJVM_EVENT_TYPE_SHUTDOWN, NULL);
					}
				//}
			}
		};


		const llvm::DataLayout& GetNomJITDataLayout() { return NomJIT::Instance().getDataLayout(); }


		/*NomJIT::NomJIT(JITTargetMachineBuilder JTMB, DataLayout DL) : ObjectLayer(ES,
			[]() { return std::make_unique<SectionMemoryManager>(); }),
			CompileLayer(ES, ObjectLayer,
				std::make_unique<ConcurrentIRCompiler>(std::move(JTMB))),
			OptimizeLayer(ES, CompileLayer, optimizeModule), DL(std::move(DL)),
			Mangle(ES, this->DL), Ctx(std::make_unique<LLVMContext>()),
			MainJD(ES.createJITDylib("<main>")) {
			MainJD.addGenerator(
				cantFail(DynamicLibrarySearchGenerator::GetForCurrentProcess(
					DL.getGlobalPrefix())));*/
		NomJIT::NomJIT(std::unique_ptr<LLJIT>&& llj) : //DL(lljit->getDataLayout()),
			Ctx(std::make_unique<LLVMContext>()), lljit(std::move(llj)) {
			lljit->getMainJITDylib().addGenerator(
				cantFail(DynamicLibrarySearchGenerator::GetForCurrentProcess(
					getDataLayout().getGlobalPrefix())));
			/*}

				Resolver(createLegacyLookupResolver(
				ES,
				[this](const std::string& Name) -> JITSymbol {
					if (auto Sym = OptimizeLayer.findSymbol(Name, false))
						return Sym;
					else if (auto SymAddr =
						RTDyldMemoryManager::getSymbolAddressInProcess(Name))
						return JITSymbol(SymAddr, JITSymbolFlags::Exported);
					else if (auto Err = Sym.takeError())
						return std::move(Err);
					return nullptr; getDataLayout();
				},
				[](Error Err) { cantFail(std::move(Err), "lookupFlags failed"); })),
				TM(EngineBuilder().selectTarget()), DL(TM->createDataLayout()),
					ObjectLayer(ES,
						[this](VModuleKey) {
							return RTDyldObjectLinkingLayer::Resources{
								std::make_shared<SectionMemoryManager>(), Resolver };
						}),
					CompileLayer(ObjectLayer, SimpleCompiler(*TM)),
							OptimizeLayer(CompileLayer, [this](std::unique_ptr<Module> M) {
							return optimizeModule(std::move(M));
								}) {*/
			lljit->getIRTransformLayer().setTransform(optimizeModule);
			//llvm::sys::DynamicLibrary::LoadLibraryPermanently(nullptr);
			llvm::sys::DynamicLibrary::AddSymbol("CPP_NOM_GCALLOC", (void*)&CPP_NOM_GCALLOC);
			llvm::sys::DynamicLibrary::AddSymbol("CPP_NOM_Print", (void*)&CPP_NOM_Print);
			llvm::sys::DynamicLibrary::AddSymbol("CPP_NOM_NEWALLOC", (void*)&CPP_NOM_NEWALLOC);
			llvm::sys::DynamicLibrary::AddSymbol("CPP_NOM_STRUCTALLOC", (void*)&CPP_NOM_STRUCTALLOC);
			llvm::sys::DynamicLibrary::AddSymbol("CPP_NOM_CLOSUREALLOC", (void*)&CPP_NOM_CLOSUREALLOC);
			llvm::sys::DynamicLibrary::AddSymbol("NOM_RT_Fail", (void*)&NOM_RT_Fail);
			////TM->setOptLevel(llvm::CodeGenOpt::Default);
		}

		llvm::Expected<std::unique_ptr<NomJIT>> NomJIT::Create() {
			auto JITbuilder = llvm::orc::LLJITBuilder();
			JITbuilder.CreateObjectLinkingLayer =
				[](llvm::orc::ExecutionSession& ES,
					const llvm::Triple& TT) -> std::unique_ptr<llvm::orc::ObjectLayer> {
						auto GetMemMgr = []() { return std::make_unique<llvm::SectionMemoryManager>(); };
						auto oll =
							std::make_unique<llvm::orc::RTDyldObjectLinkingLayer>(ES, std::move(GetMemMgr));
						if (TT.isOSBinFormatCOFF())
						{
							oll->setOverrideObjectFlagsWithResponsibilityFlags(true);
							oll->setAutoClaimResponsibilityForObjectSymbols(true);
						}
						//oll->registerJITEventListener(*(llvm::JITEventListener::createIntelJITEventListener()));
						oll->registerJITEventListener(NomJITEventListener::GetListener());
						return std::move(oll);
			};
			auto JIT = JITbuilder.create();
			if (auto err = JIT.takeError())
			{
				return std::move(err);
			}

			auto njit = std::make_unique<NomJIT>(std::move(JIT.get()));
			//llvm::orc::RTDyldObjectLinkingLayer* oll = (llvm::orc::RTDyldObjectLinkingLayer*) & njit->lljit->getObjLinkingLayer();

			return njit;
			//auto JTMB = llvm::orc::JITTargetMachineBuilder::detectHost();

			//if (!JTMB)
			//	return JTMB.takeError();

			//auto DL = JTMB->getDefaultDataLayoutForTarget();
			//if (!DL)
			//	return DL.takeError();

			//return std::make_unique<NomJIT>(std::move(*JTMB), std::move(*DL));
		}

		NomJIT::~NomJIT()
		{
		}
		Expected<ThreadSafeModule>
			NomJIT::optimizeModule(ThreadSafeModule TSM, const MaterializationResponsibility& R) {
			//std::unique_ptr<Module> Nom::Runtime::NomJIT::optimizeModule(std::unique_ptr<Module> M) {
				// Create a function pass manager.
			TSM.withModuleDo([](Module& M) {

				for (auto& F : M)
				{
					llvm::raw_os_ostream out(std::cout);
					if (verifyFunction(F, &out))
					{
						F.print(out, nullptr);
						out.flush();
						std::cout.flush();
					}
				}

				if (NomOptLevel > 0)
				{
					{
						basic_ofstream<char> pobof(NomPath + "//llvmirPO.lli");
						llvm::raw_os_ostream pofout(pobof);
						M.print(pofout, nullptr);
						pofout.flush();
						pobof.flush();
						pobof.close();
					}
					llvm::PassManagerBuilder pmb;
					pmb.OptLevel = NomOptLevel;
					auto FPM = std::make_unique<legacy::FunctionPassManager>(&M);
					auto MPM = std::make_unique<legacy::PassManager>();

					FPM->add(createVerifierPass(false));

					pmb.Inliner = createFunctionInliningPass();

					//if (TM)
					//{
					//	TM->adjustPassManager(pmb);
					//}

					FPM->add(createAtomicExpandPass());
					pmb.populateFunctionPassManager(*FPM);
					pmb.populateModulePassManager(*MPM);

					// Add some optimizations.
					//FPM->add(createInstructionCombiningPass());
					//FPM->add(createReassociatePass());
					//FPM->add(createGVNPass());
					//FPM->add(createCFGSimplificationPass());
					/*FPM->add(createLoopIdiomPass());
					FPM->add(createLoopUnrollPass());
					FPM->add(createInstructionCombiningPass());
					FPM->add(createReassociatePass());
					FPM->add(createGVNPass());
					FPM->add(createCFGSimplificationPass());
					FPM->add(createCorrelatedValuePropagationPass());
					FPM->add(createConstantPropagationPass());
					FPM->add(createPartiallyInlineLibCallsPass());*/
					FPM->add(createTailCallEliminationPass());
					pmb.populateLTOPassManager(*MPM);

					FPM->doInitialization();

					// Run the optimizations over all functions in the module being added to
					// the JIT.
					for (auto& F : M)
						FPM->run(F);

					FPM->doFinalization();
					MPM->add(createVerifierPass(false));
					MPM->run(M);
				}

				if (NomFunctionTimingLevel > 2)
				{
					IRBuilder<> builder(LLVMCONTEXT);
					for (auto& F : M)
					{
						std::string* fname = new std::string(F.getName().str());
						if (fname->empty())
						{
							raw_string_ostream rso(*fname);
							F.printAsOperand(rso, false);
							rso.flush();
						}
						auto funnameid = NomNameRepository::ProfilingInstance().GetNameID(*fname);

						auto& entryBlock = F.getEntryBlock();


						for (auto& b : F)
						{
							if (&b == &entryBlock)
							{
								for (auto& i : b)
								{
									builder.SetInsertPoint(&i);
									builder.CreateCall(GetEnterFunctionFunction(M), llvm::ArrayRef<llvm::Value*>{MakeInt<size_t>(funnameid)});
									break;
								}
							}

							auto term = b.getTerminator();
							if (term->getOpcode() == llvm::Instruction::Ret)
							{
								builder.SetInsertPoint(term);
								builder.CreateCall(GetLeaveFunctionFunction(M));
							}
						}
					}
				}

				if (NomDebugPrintLevel >= 0 || NomStatsLevel > 1)
				{
					auto dbgfun = GetDebugFunction(M);
					//auto dbgfun = GetDebugPrint(&*M);
					//auto dbgfunhex = GetDebugPrintHex(&*M);
					//std::forward_list<std::string>* strlist = new std::forward_list<std::string>();

					auto inttype = numtype(int64_t);
					IRBuilder<> builder(LLVMCONTEXT);
					for (auto& F : M)
					{
						std::string* fname = new std::string(F.getName().str());
						if (fname->empty())
						{
							raw_string_ostream rso(*fname);
							F.printAsOperand(rso, false);
							rso.flush();
						}
						auto funnameid = NomNameRepository::ProfilingInstance().GetNameID(*fname);
						//strlist->push_front(F.getName().str());

						bool isDebugFun = false;
						for (auto dbfname : NomDebugFunctions)
						{
							if (dbfname == F.getName().str())
							{
								isDebugFun = true;
								break;
							}
						}
						for (auto& b : F)
						{
							for (auto& i : b)
							{
								builder.SetInsertPoint(&i);
								//builder.CreateCall(dbgfun, { GetLLVMPointer(&strlist->front()), ConstantInt::get(inttype, 0), MakeInt<decltype(NomDebugPrintLevel)>(2) });
								builder.CreateCall(dbgfun, { MakeInt<size_t>(funnameid), MakeInt<size_t>(0), MakeInt<NomDebugPrintValueType>(NomDebugPrintValueType::Nothing), MakeInt<int64_t>(0), MakeInt<decltype(NomDebugPrintLevel)>(2) });
								break;
							}
							break;
						}

						size_t pos = 1;
						for (auto& b : F)
						{
							llvm::Instruction* lastInstruction = nullptr;
							for (auto& i : b)
							{
								if (i.getOpcode() == llvm::Instruction::PHI)
								{
									continue;
								}
								builder.SetInsertPoint(&i);
								NomDebugPrintValueType valueType = NomDebugPrintValueType::Nothing;
								llvm::Value* debugValue = MakeInt<uint64_t>(0);
								if (lastInstruction != nullptr)
								{
									auto type = lastInstruction->getType();
									if (type->isPointerTy())
									{
										debugValue = builder.CreatePtrToInt(lastInstruction, inttype);
										valueType = NomDebugPrintValueType::Pointer;
										//static std::string label = "Pointer ";
										//builder.CreateCall(dbgfunhex, { GetLLVMPointer(&label), builder.CreatePtrToInt(lastInstruction, inttype), MakeInt<decltype(NomDebugPrintLevel)>(isDebugFun ? 1 : 3) });
									}
									else if (type->isIntegerTy(1))
									{
										debugValue = builder.CreateSExtOrTrunc(lastInstruction, inttype);
										valueType = NomDebugPrintValueType::Bool;
										//static std::string label = "Bool ";
										//builder.CreateCall(dbgfun, { GetLLVMPointer(&label), builder.CreateSExtOrTrunc(lastInstruction, inttype), MakeInt<decltype(NomDebugPrintLevel)>(isDebugFun ? 1 : 3) });
									}
									else if (type->isIntegerTy())
									{
										debugValue = builder.CreateSExtOrTrunc(lastInstruction, inttype);
										valueType = NomDebugPrintValueType::Int;
										//static std::string label = "Int ";
										//builder.CreateCall(dbgfun, { GetLLVMPointer(&label), builder.CreateSExtOrTrunc(lastInstruction, inttype), MakeInt<decltype(NomDebugPrintLevel)>(isDebugFun ? 1 : 3) });
									}
									else if (type->isFloatingPointTy())
									{
										debugValue = builder.CreateFPToSI(lastInstruction, inttype);
										valueType = NomDebugPrintValueType::Float;
										//static std::string label = "Float ";
										//builder.CreateCall(dbgfun, { GetLLVMPointer(&label), builder.CreateFPToSI(lastInstruction, inttype), MakeInt<decltype(NomDebugPrintLevel)>(isDebugFun ? 1 : 3) });
									}
									//builder.CreateCall(dbgfun, { GetLLVMPointer(&strlist->front()), ConstantInt::get(inttype, pos++), MakeInt<decltype(NomDebugPrintLevel)>(isDebugFun ? 1 : 3) });
								}
								builder.CreateCall(dbgfun, { MakeInt<size_t>(funnameid), MakeInt<size_t>(pos++), MakeInt<NomDebugPrintValueType>(valueType), debugValue, MakeInt<decltype(NomDebugPrintLevel)>(isDebugFun ? 1 : 3) });
								lastInstruction = &i;
							}

						}
					}
				}
				if (NomVerbose)
				{
					llvm::raw_os_ostream out(std::cout);
					M.print(out, nullptr);
					out.flush();
					std::cout.flush();
				}
				basic_ofstream<char> bof(NomPath + "//llvmir.lli");
				llvm::raw_os_ostream fout(bof);
				M.print(fout, nullptr);
				fout.flush();
				bof.flush();
				bof.close();
				InitializeProfileCounter();
				if (NomFunctionTimingLevel > 2)
				{
					InitFunctionTimings();
				}
				//return M;
				}
			);

			return std::move(TSM);
		}
		void* GetVoidObject()
		{
			static void* voidObj = (void*)((intptr_t)NomJIT::Instance().getSymbolAddress("RT_NOM_VOIDOBJ"));
			return voidObj;
		}
}
}
