#include "RTModule.h"
#include "ClassTypeList.h"
#include "Context.h"
#include "NomJIT.h"
#include "NomAlloc.h"
#include "NomClass.h"
#include "NomClassType.h"
#include "ObjectHeader.h"
#include <iostream>
PUSHDIAGSUPPRESSION
#include "llvm/Support/raw_os_ostream.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Verifier.h"
POPDIAGSUPPRESSION
#include "GlobalNameAddressLookupList.h"
#include "RTConfig.h"
#include "boehmgcinterface.h"
#include "RTRecord.h"
#include "RTLambda.h"
#include "BoolClass.h"
#include "NomMaybeType.h"
#include "RTClassType.h"
#include <fstream>
#include <iostream>

namespace Nom
{
	namespace Runtime
	{
		static size_t structClearAreaSize;
		static size_t lambdaClearAreaSize;
		std::forward_list<void*>& RTModule::structRecords()
		{
			[[clang::no_destroy]] static std::forward_list<void*> lst; return lst;
		}
		std::forward_list<void*>& RTModule::lambdaRecords()
		{
			[[clang::no_destroy]] static std::forward_list<void*> lst; return lst;
		}
		std::forward_list<RuntimeInstantiationDictionary*>& RTModule::instantiationDictionaries()
		{
			[[clang::no_destroy]] static std::forward_list<RuntimeInstantiationDictionary*> lst; return lst;
		}

		void RTModule::ClearCaches()
		{
			for (auto ptr : structRecords())
			{
				memset(ptr, 0, structClearAreaSize);
			}
			for (auto ptr : lambdaRecords())
			{
				memset(ptr, 0, lambdaClearAreaSize);
			}
			for (auto dict : instantiationDictionaries())
			{
				dict->clear();
			}
		}
		RTModule::RTModule(NomModule* mod) : theModule(std::make_unique<llvm::Module>("module", LLVMCONTEXT))
		{
			theModule->setDataLayout(NomJIT::Instance().getDataLayout());
			llvm::Function::Create(llvm::FunctionType::get(llvm::Type::getVoidTy(LLVMCONTEXT), { {POINTERTYPE, POINTERTYPE} }, false), llvm::GlobalValue::LinkageTypes::ExternalLinkage, "RT_NOM_PRINT_STORE", theModule.get());
			llvm::Function::Create(llvm::FunctionType::get(llvm::Type::getVoidTy(LLVMCONTEXT), { {POINTERTYPE, POINTERTYPE} }, false), llvm::GlobalValue::LinkageTypes::ExternalLinkage, "RT_NOM_PRINT_LOAD", theModule.get());
			NomClassType::GetInitializerFunction(*(theModule.get()));
			NomMaybeType::GetInitializerFunction(*(theModule.get()));
			RTClassType::Instance().GetLLVMElement(*(theModule.get()));
			auto& ifaces = mod->GetInterfaces();
			auto& clses = mod->GetClasses();
			for (auto& iface : ifaces)
			{
				iface->PreprocessInheritance();
				instantiationDictionaries().push_front(const_cast<RuntimeInstantiationDictionary*>(&iface->runtimeInstantiations));
			}
			for (auto& cls : clses)
			{
				cls->PreprocessInheritance();
				instantiationDictionaries().push_front(const_cast<RuntimeInstantiationDictionary*>(&cls->runtimeInstantiations));
			}
			NomLambda::ProcessPreprocessQueue();
			for (auto& iface : ifaces)
			{
				iface->GetLLVMElement(*theModule.get());
			}
			NomBoolObjects::GetInstance()->GetLLVMElement(*theModule.get());
			for (auto& cls : clses)
			{
				cls->CompileLLVM(theModule.get());
			}
			ObjectHeader::EnsureExternalReadWriteFieldFunctions(theModule.get());
			std::vector<std::string> compiledFunctionNames;
			std::basic_ofstream<char> pobof;
			if (NomVerbose)
			{
				pobof=std::basic_ofstream<char>(NomPath + "//addresses.txt");
				int funnamecount = 0;
				std::cout << "\nModule compilation complete, adding to JIT...\n";
				llvm::raw_os_ostream out(std::cout);
				theModule->print(out, nullptr);
				out.flush();
				for (auto& f : *theModule.get())
				{
					if (f.begin()!=f.end())
					{
						if (f.getName().empty())
						{
							std::string funname = "AnonymousFunction" + std::to_string(funnamecount++);
							f.setName(funname);
						}
						compiledFunctionNames.push_back(f.getName().str());
					}
				}
			}

			std::vector<std::string> structDescriptorNames;
			std::vector<std::string> lambdaDescriptorNames;
			[[clang::no_destroy]] static const std::string structdescprefix = "RT_NOM_STRUCTDESC_";
			[[clang::no_destroy]] static const std::string lambdadescprefix = "RT_NOM_LAMBDADESC_";
			for (auto& gv : theModule.get()->globals())
			{
				if (gv.getLinkage() == GlobalValue::LinkageTypes::ExternalLinkage)
				{
					if (gv.getName().substr(0, structdescprefix.length()) == structdescprefix)
					{
						structDescriptorNames.push_back(gv.getName().str());
					}
					else if (gv.getName().substr(0, lambdadescprefix.length()) == lambdadescprefix)
					{
						lambdaDescriptorNames.push_back(gv.getName().str());
					}
				}
			}
			auto &jit = NomJIT::Instance();
			std::vector<std::string> globalnames;
			for (auto& glbl : theModule.get()->globals())
			{
				if (glbl.getParent() != theModule.get() || glbl.getLinkage() != GlobalValue::LinkageTypes::ExternalLinkage)
				{
					std::cout << glbl.getName().str();
					std::cout.flush();
					throw new std::exception();
				}
				if (!glbl.getName().empty())
				{
					globalnames.push_back(glbl.getName().str());
				}
			}
			if (NomVerbose)
			{
				llvm::raw_os_ostream out(std::cout);
				if (verifyModule(*theModule.get(), &out))
				{
					out.flush();
					std::cout << "Could not verify module!";
					out.flush();
					std::cout.flush();
					throw new std::exception();
				}
			}
			if (jit.addModule(std::move(theModule)))
			{
				throw new std::exception();
			}
			for (auto &glbl : globalnames)
			{
				if (NomVerbose)
				{
					std::cout << "\n" << glbl;
				}
				auto evalSymbol = jit.lookup(glbl);
				if (!evalSymbol)
				{
					throw evalSymbol.takeError();
				}
				if (NomVerbose)
				{
					std::cout << "\n" << glbl << " : " << std::hex << std::move(evalSymbol->getValue()) << std::dec;
					pobof << "\n" << glbl << " : " << std::hex << std::move(evalSymbol->getValue()) << std::dec;
				}
			}
			if (NomVerbose)
			{
				std::cout << "\nadded to JIT";
				for (std::string& gname : GetGlobalsForAddressLookup())
				{
					std::cout << "\n" << gname << ": ";
					auto symAddress = NomJIT::Instance().lookup(gname)->getValue();
					std::cout << std::hex << std::move(symAddress);
				}
				for (auto fname : compiledFunctionNames)
				{
					std::cout << "\nFUN " << fname << ": ";
					auto symAddress = NomJIT::Instance().lookup(fname)->getValue();
					std::cout << std::hex << std::move(symAddress);
				}
				std::cout << "\n";
				pobof.flush();
				pobof.close();
			}

			GetGlobalsForAddressLookup().clear();

			GetVoidObject(); //make sure to call this once to initialize it
		}

		RTModule::~RTModule()
		{

		}
	}
}
