#pragma once
#include "AvailableExternally.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Module.h"
#include "llvm/ADT/Twine.h"
#include "NomBuilder.h"

namespace Nom
{
	namespace Runtime
	{
		class NomCallable;
		enum class RTSignatureFields : unsigned char { TypeParameters = 0, LLVMFunctionType = 1, ReturnType = 2, TypeParamCount = 3, ParamCount = 4, ParameterTypes = 5 };
		class RTSignature : public AvailableExternally<llvm::Function>
		{
		private:
			RTSignature();
		public:
			static RTSignature &Instance();
			static llvm::StructType* GetLLVMType();
			static llvm::StructType* GetLLVMType(size_t argCount);
			static llvm::StructType* GetLLVMType(size_t typeArgCount, size_t argCount);

			static llvm::Value* GenerateReadReturnType(NomBuilder& builder, llvm::Value* signatureRef);
			static llvm::Value* GenerateReadTypeParamCount(NomBuilder& builder, llvm::Value* signatureRef);
			static llvm::Value* GenerateReadParamCount(NomBuilder& builder, llvm::Value* signatureRef);
			static llvm::Value* GenerateReadLLVMFunctionType(NomBuilder& builder, llvm::Value* signatureRef);
			static llvm::Value* GenerateReadTypeParameter(NomBuilder& builder, llvm::Value* signatureRef, llvm::Value* index);
			static llvm::Value* GenerateReadParameter(NomBuilder& builder, llvm::Value* signatureRef, llvm::Value* index);

			static llvm::Constant* CreateGlobalConstant(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage, llvm::Twine name, const NomCallable* method);

			static llvm::FunctionType* GetLLVMFunctionType();
			// Inherited via AvailableExternally
			virtual llvm::Function* createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const override;

			virtual llvm::Function* findLLVMElement(llvm::Module& mod) const override;

		};
	}
}