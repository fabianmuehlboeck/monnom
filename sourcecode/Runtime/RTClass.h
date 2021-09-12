#pragma once

#include <map>
#include <unordered_map>
#include "Defs.h"
#include "NomString.h"
#include "llvm/ADT/TinyPtrVector.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/IR/Module.h"
#include "llvm/ADT/StringRef.h"
#include "ARTRep.h"
#include "NomBuilder.h"

namespace Nom
{
	namespace Runtime
	{
		class RTClassTypeScheme;
		class RTInterfaceTableEntry;
		class ObjectHeader;
		class RTClass;

		enum class RTClassFields : unsigned char { RTVTable = 0, RTInterface = 1, FieldCount = 2 };
		class RTClass : public ARTRep<RTClass, RTClassFields>
		{
		public:

			static llvm::StructType* GetLLVMType();
			static llvm::StructType* GetConstantType(size_t offset, llvm::Type* methodTableType, llvm::StructType* ddtype, llvm::StructType* stetype);
			static llvm::Constant* CreateConstant(llvm::GlobalVariable* gvar, llvm::StructType* gvartype, const NomInterface* irptr, llvm::Function* fieldRead, llvm::Function* fieldWrite, llvm::Constant* dynamicDispatcherTable, llvm::ConstantInt* fieldCount, llvm::ConstantInt* typeArgCount, llvm::ConstantInt* superClassCount, llvm::ConstantInt* superInterfaceCount, llvm::Constant* superTypeEntries, llvm::Constant* methodTable, llvm::Constant* checkReturnValueFunction, llvm::Constant* interfaceMethodTable, llvm::Constant* signature, llvm::Constant* castFunction);
			static llvm::Constant* FindConstant(llvm::Module& mod, const llvm::StringRef name);

			static llvm::Value* GenerateReadFieldCount(NomBuilder& builder, llvm::Value* descriptorPtr);
			static llvm::Value* GenerateReadSuperInstanceCount(NomBuilder& builder, llvm::Value* descriptorPtr);
			static llvm::Value* GenerateReadSuperInstances(NomBuilder& builder, llvm::Value* descriptorPtr);

			static llvm::Constant* GetInterfaceReference(llvm::Constant* clsVtablePtr);

			static llvm::Value* GetInterfaceReference(NomBuilder& builder, llvm::Value* clsVtablePtr);

			static llvm::Value* GenerateReadTypeArgCount(NomBuilder& builder, llvm::Value* descriptorPtr);

			~RTClass()
			{

			}
		};

	}
}
