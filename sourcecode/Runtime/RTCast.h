#pragma once
#include "NomValue.h"
#include "NomTypeDecls.h"
#include "NomBuilder.h"
#include "AvailableExternally.h"

namespace Nom
{
	namespace Runtime
	{
		class CompileEnv;
		enum class TypeArgumentListStackFields :unsigned char { Next = 0, Types = 1 };

		llvm::Value* GenerateGetTypeArgumentListStackTypes(NomBuilder& builder, llvm::Value* talst);

		class RTCast
		{
		private:
		public:
			static llvm::FunctionType* GetAdjustFunctionType();
			static llvm::Value* GenerateMonotonicCast(NomBuilder& builder, CompileEnv* env, NomValue &value, NomClassTypeRef type);
			static llvm::Value* GenerateMonotonicCast(NomBuilder& builder, CompileEnv* env, NomValue& value, llvm::Value* type);
			static uint64_t nextCastSiteID();
			static llvm::Value* GenerateCast(NomBuilder& builder, CompileEnv* env, NomValue value, NomTypeRef type);
			static llvm::Value* GenerateCast(NomBuilder& builder, CompileEnv* env, llvm::Value* value, NomTypeRef type);
		};

		class FailingAdjustFun : public AvailableExternally<llvm::Function>
		{
		private:
			FailingAdjustFun() {}
		public:
			static FailingAdjustFun* GetInstance() { static FailingAdjustFun instance; return &instance; }
			~FailingAdjustFun() {};
			// Inherited via AvailableExternally
			virtual llvm::Function* createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const override;
			virtual llvm::Function* findLLVMElement(llvm::Module& mod) const override;
		};
	}
}