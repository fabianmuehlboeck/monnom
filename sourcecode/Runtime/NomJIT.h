#pragma once

#include <memory>
PUSHDIAGSUPPRESSION
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
#include "llvm/ExecutionEngine/Orc/LLJIT.h"
POPDIAGSUPPRESSION

namespace Nom
{
	namespace Runtime
	{
		void* GetVoidObject();
		class NomJIT
		{
		private:
			llvm::orc::ThreadSafeContext Ctx;

			std::unique_ptr<llvm::orc::LLJIT> lljit;

		public:
			NomJIT(std::unique_ptr<llvm::orc::LLJIT> &&llj);
			static llvm::Expected<std::unique_ptr<NomJIT>> Create();

			const llvm::DataLayout& getDataLayout() const { return lljit->getDataLayout(); }

			llvm::LLVMContext& getContext() { return *Ctx.getContext(); }

			llvm::Error addModule(std::unique_ptr<llvm::Module> M) {
				return lljit->addIRModule(llvm::orc::ThreadSafeModule(std::move(M), Ctx));
			}


			llvm::Expected<llvm::orc::ExecutorAddr> lookup(llvm::StringRef Name) {
				return lljit->lookup(Name);
			}


		public:
			static NomJIT& Instance() { [[clang::no_destroy]] static std::unique_ptr<NomJIT> instance(std::move(*Create())); return *instance; }
	

			uint64_t getSymbolAddress(const std::string Name) {
				return lookup(Name)->getValue();
			}

			~NomJIT();

		private:
			static llvm::Expected<llvm::orc::ThreadSafeModule>
				optimizeModule(llvm::orc::ThreadSafeModule TSM, const llvm::orc::MaterializationResponsibility& R);
		};

	}
}
