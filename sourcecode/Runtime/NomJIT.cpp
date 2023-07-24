#include "NomJIT.h"
#include "NomVMInterface.h"
#include <fstream>
#include <iostream>
#include <atomic>
#include <forward_list>
PUSHDIAGSUPPRESSION
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
#include "llvm/Transforms/IPO.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/OptimizationLevel.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/raw_os_ostream.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/ExecutionEngine/Orc/IRTransformLayer.h"
#define NOM_OMITJITPROFILING
#ifndef NOM_OMITJITPROFILING
#include <jitprofiling.h>
#include "llvm/Object/SymbolSize.h"
#endif
POPDIAGSUPPRESSION
#include "NomJITLight.h"
#include "StringClass.h"
#include "RTConfig.h"
#include "CompileHelpers.h"
#include "RTOutput.h"
#include "CastStats.h"
#include "NomNameRepository.h"
#include "FunctionTimings.h"

using namespace llvm;
using namespace llvm::orc;
using namespace Nom::Runtime;
using namespace std;


namespace Nom
{
	namespace Runtime
	{
#ifndef NOM_OMITJITPROFILING
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
#endif

		const llvm::DataLayout& GetNomJITDataLayout() { return NomJIT::Instance().getDataLayout(); }

		NomJIT::NomJIT(std::unique_ptr<LLJIT>&& llj) : 
			Ctx(std::make_unique<LLVMContext>()), lljit(std::move(llj)) {
			lljit->getMainJITDylib().addGenerator(
				cantFail(DynamicLibrarySearchGenerator::GetForCurrentProcess(
					getDataLayout().getGlobalPrefix())));

			lljit->getIRTransformLayer().setTransform(optimizeModule);
			llvm::sys::DynamicLibrary::AddSymbol("CPP_NOM_GCALLOC", reinterpret_cast<void*>(&CPP_NOM_GCALLOC));
			llvm::sys::DynamicLibrary::AddSymbol("CPP_NOM_Print", reinterpret_cast<void*>(&CPP_NOM_Print));
			llvm::sys::DynamicLibrary::AddSymbol("CPP_NOM_NEWALLOC", reinterpret_cast<void*>(&CPP_NOM_NEWALLOC));
			llvm::sys::DynamicLibrary::AddSymbol("CPP_NOM_RECORDALLOC", reinterpret_cast<void*>(&CPP_NOM_RECORDALLOC));
			llvm::sys::DynamicLibrary::AddSymbol("CPP_NOM_CLOSUREALLOC", reinterpret_cast<void*>(&CPP_NOM_CLOSUREALLOC));
			llvm::sys::DynamicLibrary::AddSymbol("NOM_RT_Fail", reinterpret_cast<void*>(&NOM_RT_Fail));
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
#ifndef NOM_OMITJITPROFILING
						oll->registerJITEventListener(NomJITEventListener::GetListener());
#endif
						return std::move(oll);
			};
			auto JIT = JITbuilder.create();
			if (auto err = JIT.takeError())
			{
				return std::move(err);
			}

			auto njit = std::make_unique<NomJIT>(std::move(JIT.get()));

			return njit;
		}

		NomJIT::~NomJIT()
		{
		}
		Expected<ThreadSafeModule>
			NomJIT::optimizeModule(ThreadSafeModule TSM, [[maybe_unused]] const MaterializationResponsibility& R) {
			TSM.withModuleDo([](Module& M) {

				llvm::raw_os_ostream out(std::cout);
				for (auto& F : M)
				{
					if (verifyFunction(F, &out))
					{
						F.print(out, nullptr);
						out.flush();
						std::cout.flush();
					}
				}

				if (NomOptLevel > 0)
				{
					if(NomVerbose)
					{
						basic_ofstream<char> pobof(NomPath + "//llvmirPO.lli");
						llvm::raw_os_ostream pofout(pobof);
						M.print(pofout, nullptr);
						pofout.flush();
						pobof.flush();
						pobof.close();
					}
					LoopAnalysisManager LAM;
					FunctionAnalysisManager FAM;
					CGSCCAnalysisManager CGAM;
					ModuleAnalysisManager MAM;
					PassBuilder PB;

					PB.registerModuleAnalyses(MAM);
					PB.registerCGSCCAnalyses(CGAM);
					PB.registerFunctionAnalyses(FAM);
					PB.registerLoopAnalyses(LAM);
					PB.crossRegisterProxies(LAM, FAM, CGAM, MAM);

					OptimizationLevel optLevel;
					switch (NomOptLevel)
					{
					case 0:
						optLevel = OptimizationLevel::O0;
						break;
					case 1:
						optLevel = OptimizationLevel::O1;
						break;
					case 2:
						optLevel = OptimizationLevel::O2;
						break;
					default:
					case 3:
						optLevel = OptimizationLevel::O3;
						break;
					}

					legacy::FunctionPassManager FPM=legacy::FunctionPassManager(&M);
					ModulePassManager MPM;
					if (optLevel.getSpeedupLevel() != 0)
					{
						MPM = PB.buildPerModuleDefaultPipeline(optLevel);
					}
					else
					{
						MPM = ModulePassManager();
					}

					FPM.add(createVerifierPass(false));

					//if (TM)
					//{
					//	TM->adjustPassManager(pmb);
					//}

					FPM.add(createAtomicExpandPass());

					// Add some optimizations.
					FPM.add(createTailCallEliminationPass());

					FPM.add(createLoopUnrollPass(2, false, false, -1, -1, -1, -1, -1, 2));
					FPM.add(createLICMPass());
					FPM.add(createLoopUnrollPass(2, false, false, -1, -1, -1, -1, -1, 2));
					FPM.add(createLICMPass());
					FPM.add(createInductiveRangeCheckEliminationPass());
					FPM.add(createTailCallEliminationPass());

					if (RTConfig_AdditionalOptPasses)
					{
						//llvm::PassManagerBuilder pmb2;
						//pmb2.OptLevel = 2;
						//pmb2.populateFunctionPassManager(*FPM);
						//pmb2.populateModulePassManager(*MPM);
						//pmb2.populateLTOPassManager(*MPM);
						//FPM->add(createTailCallEliminationPass());
					}
					FPM.doInitialization();

					// Run the optimizations over all functions in the module being added to
					// the JIT.
					for (auto& F : M)
					{
						if (verifyFunction(F, &out))
						{
							F.print(out, nullptr);
							out.flush();
							std::cout.flush();
						}
						FPM.run(F);
					}

					FPM.doFinalization();
					MPM.addPass(VerifierPass(false));
					MPM.run(M, MAM);

					for (auto& F : M)
					{
						FPM.run(F);
					}
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
								builder.SetInsertPoint(&b, b.getFirstInsertionPt());
								builder.CreateCall(GetEnterFunctionFunction(M), llvm::ArrayRef<llvm::Value*>{MakeInt<size_t>(funnameid)});
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

						llvm::BasicBlock* firstBlock = &F.getEntryBlock();
						builder.SetInsertPoint(firstBlock, firstBlock->getFirstInsertionPt());
						builder.CreateCall(dbgfun, { MakeInt<size_t>(funnameid), MakeInt<size_t>(0), MakeInt<NomDebugPrintValueType>(NomDebugPrintValueType::Nothing), MakeInt<int64_t>(0), MakeInt<decltype(NomDebugPrintLevel)>(2) });

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
					M.print(out, nullptr);
					out.flush();
					std::cout.flush();
				}
				if (NomVerbose)
				{
					basic_ofstream<char> bof(NomPath + "//llvmir.lli");
					llvm::raw_os_ostream fout(bof);
					M.print(fout, nullptr);
					fout.flush();
					bof.flush();
					bof.close();
				}
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
			static void* voidObj = reinterpret_cast<void*>(NomJIT::Instance().getSymbolAddress("RT_NOM_VOIDOBJ"));
			return voidObj;
		}
}
}
