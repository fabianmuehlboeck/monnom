#pragma once
#include "RTTypeHead.h"
#include "RTClass.h"
#include "RTConcreteType.h"
PUSHDIAGSUPPRESSION
#include "llvm/IR/DerivedTypes.h"
POPDIAGSUPPRESSION
#include "AvailableExternally.h"

namespace Nom
{
	namespace Runtime
	{
		class RTClass;
		class NomClassType;
		class XRTClassType;
		enum class RTClassTypeFields : unsigned char { TypeArgs=0, Head = 1, Class = 2 };
		class XRTClassType : public AvailableExternally<llvm::Function>
		{
		private:
			XRTClassType() {}
		public:
			static XRTClassType& Instance() { [[clang::no_destroy]] static XRTClassType rtct; return rtct; }
			static llvm::StructType *GetLLVMType();
			static llvm::StructType * GetLLVMType(size_t size);
			static llvm::Constant *GetConstant(llvm::Module &mod, const NomClassType *cls, llvm::Constant* subt);

			static llvm::Value* GenerateReadClassDescriptorLink(NomBuilder& builder, llvm::Value* type);
			static llvm::Value* GetTypeArgumentsPtr(NomBuilder& builder, llvm::Value* type);

			static llvm::FunctionType* GetInstantiateClassTypeFunctionType();

			// Inherited via AvailableExternally
			virtual llvm::Function* createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const override;
			virtual llvm::Function* findLLVMElement(llvm::Module& mod) const override;
		};
	}
}
