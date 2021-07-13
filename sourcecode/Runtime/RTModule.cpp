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

namespace Nom
{
	namespace Runtime
	{
		RTModule::RTModule(NomModule* mod) : theModule(std::make_unique<llvm::Module>("module", LLVMCONTEXT))
		{
			theModule->setDataLayout(NomJIT::Instance().getDataLayout());
			llvm::Function::Create(llvm::FunctionType::get(llvm::Type::getVoidTy(LLVMCONTEXT), { {POINTERTYPE, POINTERTYPE} }, false), llvm::GlobalValue::LinkageTypes::ExternalLinkage, "RT_NOM_PRINT_STORE", theModule.get());
			llvm::Function::Create(llvm::FunctionType::get(llvm::Type::getVoidTy(LLVMCONTEXT), { {POINTERTYPE, POINTERTYPE} }, false), llvm::GlobalValue::LinkageTypes::ExternalLinkage, "RT_NOM_PRINT_LOAD", theModule.get());
			NomClassType::GetInitializerFunction(*(theModule.get()));
			NomMaybeType::GetInitializerFunction(*(theModule.get()));
			RTClassType::Instance().GetLLVMElement(*(theModule.get()));
			auto& ifaces = mod->GetInterfaces();
			//for (auto& iface : ifaces)
			//{
			//	classes.emplace_front(iface->GetTypeArgCount(), 0);
			//	iface->SetRTClass(&classes.front());
			//}
			auto& clses = mod->GetClasses();
			//for (auto& cls : clses)
			//{
			//	classes.emplace_front(cls->GetTypeArgCount(), cls->GetFieldCount());
			//	cls->SetRTClass(&classes.front());
			//}
			//for (auto& iface : ifaces)
			//{
			//	llvm::TinyPtrVector<const RTClassType *> stypes;
			//	ClassTypeList ctl = iface->GetSuperInterfaces();
			//	for (auto& stype : *ctl)
			//	{
			//		stypes.push_back(stype->GetRTClassType());
			//	}
			//	((RTClass*)(iface->GetRTClass()))->SuperTypes() = stypes;
			//}
			//for (auto& cls : clses)
			//{
			//	llvm::TinyPtrVector<const RTClassType *> stypes;
			//	if (cls->GetSuperClass() != nullptr)
			//	{
			//		stypes.push_back(cls->GetSuperClass()->GetRTClassType());
			//	}
			//	ClassTypeList ctl = cls->GetSuperInterfaces();
			//	for (auto& stype : *ctl)
			//	{
			//		stypes.push_back(stype->GetRTClassType());
			//	}
			//	((RTClass*)(cls->GetRTClass()))->SuperTypes() = stypes;
			//}
			for (auto& iface : ifaces)
			{
				iface->PreprocessInheritance();
			}
			for (auto& cls : clses)
			{
				cls->PreprocessInheritance();
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
			if (NomVerbose)
			{
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
			}

			

			/*for (auto& cls : clses)
			{
				cls->InitializeDictionary();
			}*/
			PreparedDictionary::LoadDictionaryContents();
			GetGlobalsForAddressLookup().clear();

			for (auto& sdn : structDescriptorNames)
			{
				auto symAddress = NomJIT::Instance().lookup(sdn)->getAddress();
				auto addrValue = (std::move(symAddress));
				auto addrPtr = (char*)addrValue;
				addrPtr += RTStruct::GetLLVMLayout()->getElementOffset((unsigned char)RTStructFields::SpecializedVTable);
				bgc_register_root(addrPtr, addrPtr + sizeof(intptr_t));
			}
			for (auto& sdn : lambdaDescriptorNames)
			{
				auto symAddress = NomJIT::Instance().lookup(sdn)->getAddress();
				auto addrValue = (std::move(symAddress));
				auto addrPtr = (char*)addrValue;
				addrPtr += RTLambda::GetLLVMLayout()->getElementOffset((unsigned char)RTLambdaFields::SpecializedVTable);
				bgc_register_root(addrPtr, addrPtr + sizeof(intptr_t));
			}


			//std::cout << "\nadded to JIT, building method tables\n";
			//for (auto& cls : clses)
			//{
			//	std::cout << "\nBuilding method table for class" + (cls->GetName())->ToStdString() +"\n";
			//	void ** methodtable = (void **) nalloc(cls->MethodTable.size() * sizeof(void *));
			//	for (auto& meth : cls->MethodTable)
			//	{
			//		std::cout << "\nFetching method" + (meth->GetName()) + "\n";
			//		methodtable[meth->GetOffset()] = (void*) NomJIT::Instance().getSymbolAddress(meth->GetName());
			//	}
			//	((RTClass*)(cls->GetRTClass()))->methodtable = methodtable;
			//}
			//std::cout << "\nmethod tables complete\n";
			//TODO: interface tables
			GetVoidObject(); //make sure to call this once to initialize it
		}

		RTModule::~RTModule()
		{

		}
	}
}