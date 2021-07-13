#pragma once
#include "ARTRep.h"
#include "Context.h"
#include "NomBuilder.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Value.h"
#include "RTDescriptor.h"

namespace Nom
{
	namespace Runtime
	{
		enum class RTPartialAppFields : unsigned char { NumEntries = 0, Entries = 1 };
		enum class RTPartialAppEntryFields : unsigned char { Key = 0, Dispatcher = 1 };
		class RTPartialApp : public ARTRep<RTPartialApp, RTPartialAppFields>
		{
		public:
			static llvm::StructType* GetLLVMType();
			static llvm::Constant* CreateConstant(llvm::ArrayRef<std::pair<std::pair<uint32_t, uint32_t>, llvm::Constant*>> entries);
			static llvm::Value* GenerateFindDispatcher(NomBuilder& builder, llvm::Value* partialAppDesc, uint32_t typeargcount, uint32_t argcount);
			//static llvm::Constant* CreateConstant(const void* nomlink, const void* dictionary, RTDescriptorKind kind = RTDescriptorKind::PartialApplication);
			//static llvm::Value* GenerateReadDescriptorKind(NomBuilder& builder, llvm::Module& mod, llvm::Value* descriptorPtr);
			//static llvm::Value* GenerateReadDescriptorLink(NomBuilder& builder, llvm::Module& mod, llvm::Value* descriptorPtr);
			//static llvm::Value* GenerateReadDescriptorDictionary(NomBuilder& builder, llvm::Module& mod, llvm::Value* descriptorPtr);
			//static llvm::Value* GenerateCheckIsExpando(NomBuilder& builder, llvm::Module& mod, llvm::Value* descriptorPtr);
			//static llvm::Value* GenerateFindDispatcher(NomBuilder& builder, llvm::Module& mod, llvm::Value* descriptorPtr, llvm::ConstantInt* key);
			//static llvm::Value* CreateDescriptorDictionaryLookup(NomBuilder& builder, llvm::Module& mod, NomValue receiver, llvm::Value* key, const llvm::Twine keyname = "");
			//static llvm::Value* CreateDictionaryLoad(NomBuilder& builder, llvm::Module& mod, llvm::Value* thisObj, llvm::Value* cdesc, llvm::ConstantInt* key, const llvm::Twine keyname = "");
			//static llvm::Value* CreateDictionaryStore(NomBuilder& builder, llvm::Module& mod, llvm::Value* thisObj, llvm::Value* cdesc, llvm::ConstantInt* key, llvm::Value* value);
		};
	}
}