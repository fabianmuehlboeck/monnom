#pragma once
#include "NomValue.h"
#include "NomTypeDecls.h"
#include "NomBuilder.h"
#include "AvailableExternally.h"
#include "RTSubstStack.h"
#include "RTValuePtr.h"

namespace Nom
{
	namespace Runtime
	{
		class CompileEnv;


		class RTCast
		{
		private:
		public:
			static llvm::FunctionType* GetAdjustFunctionType();
			static llvm::Value* GenerateMonotonicCast(NomBuilder& builder, CompileEnv* env, RTValuePtr value, NomClassTypeRef type);
			static llvm::Value* GenerateMonotonicCast(NomBuilder& builder, CompileEnv* env, RTValuePtr value, llvm::Value* type);
			static uint64_t nextCastSiteID();
			static llvm::Value* GenerateCast(NomBuilder& builder, CompileEnv* env, RTValuePtr value, NomTypeRef type);
			//static llvm::Value* GenerateCast(NomBuilder& builder, CompileEnv* env, llvm::Value* value, NomTypeRef type);
		};

		class FailingAdjustFun : public AvailableExternally<llvm::Function>
		{
		private:
			FailingAdjustFun() {}
		public:
			static FailingAdjustFun* GetInstance() { [[clang::no_destroy]] static FailingAdjustFun instance; return &instance; }
			~FailingAdjustFun() override {}
			// Inherited via AvailableExternally
			virtual llvm::Function* createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const override;
			virtual llvm::Function* findLLVMElement(llvm::Module& mod) const override;
		};
	}
}
