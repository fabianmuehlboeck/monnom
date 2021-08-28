#include "RTModule.h"
#include "ClassTypeList.h"
#include "Context.h"
#include "NomJIT.h"
#include "NomAlloc.h"
#include "NomClass.h"
#include "NomClassType.h"
#include "ObjectHeader.h"
#include <iostream>
#include "llvm/Support/raw_os_ostream.h"
#include "llvm/IR/Module.h"
#include "GlobalNameAddressLookupList.h"
#include "RTConfig.h"
#include "boehmgcinterface.h"
#include "RTLambda.h"
#include "RTStruct.h"
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
			static std::forward_list<void*> lst; return lst;
		}
		std::forward_list<void*>& RTModule::lambdaRecords()
		{
			static std::forward_list<void*> lst; return lst;
		}
		std::forward_list<RuntimeInstantiationDictionary*>& RTModule::instantiationDictionaries()
		{
			static std::forward_list<RuntimeInstantiationDictionary*> lst; return lst;
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
					if (f.getBasicBlockList().size() > 0)
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
			static const std::string structdescprefix = "RT_NOM_STRUCTDESC_";
			static const std::string lambdadescprefix = "RT_NOM_LAMBDADESC_";
			for (auto& gv : theModule.get()->getGlobalList())
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
				if (!glbl.getName().empty())
				{
					globalnames.push_back(glbl.getName().str());
				}
			}
			if (jit.addModule(std::move(theModule)))
			{
				throw new std::exception();
			}
			for (auto &glbl : globalnames)
			{
				auto evalSymbol = jit.lookup(glbl);
				if (!evalSymbol)
				{
					throw evalSymbol.takeError();
				}
				if (NomVerbose)
				{
					std::cout << "\n" << glbl << " : " << std::hex << std::move(evalSymbol->getAddress()) << std::dec;
					pobof << "\n" << glbl << " : " << std::hex << std::move(evalSymbol->getAddress()) << std::dec;
				}
			}
			if (NomVerbose)
			{
				std::cout << "\nadded to JIT";
				for (std::string& gname : GetGlobalsForAddressLookup())
				{
					std::cout << "\n" << gname << ": ";
					auto symAddress = NomJIT::Instance().lookup(gname)->getAddress();
					std::cout << std::hex << std::move(symAddress);
				}
				for (auto fname : compiledFunctionNames)
				{
					std::cout << "\nFUN " << fname << ": ";
					auto symAddress = NomJIT::Instance().lookup(fname)->getAddress();
					std::cout << std::hex << std::move(symAddress);
				}
				std::cout << "\n";
				pobof.flush();
				pobof.close();
			}

			PreparedDictionary::LoadDictionaryContents();
			GetGlobalsForAddressLookup().clear();

			auto structSpecializedVTableOffset = RTStruct::GetLLVMLayout()->getElementOffset((unsigned char)RTStructFields::SpecializedVTable);
			auto structCastSiteIDOffset = RTStruct::GetLLVMLayout()->getElementOffset((unsigned char)RTStructFields::SpecializedVTableCastID);
			auto structPreallocatedSlotsOFfset = RTStruct::GetLLVMLayout()->getElementOffset((unsigned char)RTStructFields::PreallocatedSlots);
			auto structMinOffset = std::min({ structSpecializedVTableOffset, structCastSiteIDOffset, structPreallocatedSlotsOFfset });
			auto structMaxOffset = std::max({ structSpecializedVTableOffset, structCastSiteIDOffset, structPreallocatedSlotsOFfset });
			structClearAreaSize = structMaxOffset - structMinOffset + sizeof(intptr_t);

			for (auto& sdn : structDescriptorNames)
			{
				auto symAddress = NomJIT::Instance().lookup(sdn)->getAddress();
				auto addrValue = (std::move(symAddress));
				auto addrPtr = (char*)addrValue;
				structRecords().push_front(addrPtr + structMinOffset);
				addrPtr += structSpecializedVTableOffset;
				bgc_register_root(addrPtr, addrPtr + sizeof(intptr_t));
			}

			auto lambdaSpecializedVTableOffset = RTLambda::GetLLVMLayout()->getElementOffset((unsigned char)RTLambdaFields::SpecializedVTable);
			auto lambdaCastSiteIDOffset = RTLambda::GetLLVMLayout()->getElementOffset((unsigned char)RTLambdaFields::SpecializedVTableCastID);
			auto lambdaPreallocatedSlotsOFfset = RTLambda::GetLLVMLayout()->getElementOffset((unsigned char)RTLambdaFields::PreallocatedSlots);
			auto lambdaMinOffset = std::min({ lambdaSpecializedVTableOffset, lambdaCastSiteIDOffset, lambdaPreallocatedSlotsOFfset });
			auto lambdaMaxOffset = std::max({ lambdaSpecializedVTableOffset, lambdaCastSiteIDOffset, lambdaPreallocatedSlotsOFfset });
			lambdaClearAreaSize = lambdaMaxOffset - lambdaMinOffset + sizeof(intptr_t);
			for (auto& sdn : lambdaDescriptorNames)
			{
				auto symAddress = NomJIT::Instance().lookup(sdn)->getAddress();
				auto addrValue = (std::move(symAddress));
				auto addrPtr = (char*)addrValue;
				lambdaRecords().push_front(addrPtr + lambdaMinOffset);
				addrPtr += lambdaSpecializedVTableOffset;
				bgc_register_root(addrPtr, addrPtr + sizeof(intptr_t));
			}

			GetVoidObject(); //make sure to call this once to initialize it
		}

		RTModule::~RTModule()
		{

		}
	}
}