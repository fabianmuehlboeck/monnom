#pragma once
#include "ARTRep.h"
#include "NomBuilder.h"

namespace Nom
{
	namespace Runtime
	{
		enum class RTGeneralInterfaceFields :unsigned char { RTInterface = 0, Template = 1, TemplateSize = 2, TemplateOffset = 3, OptimizedInterface = 4 };
		class RTGeneralInterface : public ARTRep<RTGeneralInterface, RTGeneralInterfaceFields>
		{
		public:
			static llvm::StructType* GetLLVMType();
			static llvm::Value* GenerateReadTemplate(NomBuilder& builder, llvm::Value* descPtr);
			static llvm::Value* GenerateReadOptimizedVTable(NomBuilder& builder, llvm::Value* descPtr);
			static llvm::Value* GenerateReadTemplateSize(NomBuilder& builder, llvm::Value* descPtr);
			static llvm::Value* GenerateReadTemplateOffset(NomBuilder& builder, llvm::Value* descPtr);
			static llvm::Constant* CreateGlobalConstant(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage, const llvm::Twine& name, const NomInterface* irptr, llvm::Constant* typeArgCount, llvm::Constant* superTypesCount, llvm::Constant* superTypeEntries, llvm::Constant* instantiationDictionary);
			static llvm::Constant* FindConstant(llvm::Module& mod, const llvm::StringRef name);
		};
	}
}