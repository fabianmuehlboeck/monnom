#pragma once
#include "ARTRep.h"
#include "llvm/IR/DerivedTypes.h"
#include "NomBuilder.h"
#include "RTPartialApp.h"

namespace Nom
{
	namespace Runtime
	{
		class NomStruct;
		enum class RTStructFields : unsigned char { STable = 0, FieldLookup = 1, FieldStore = 2, DispatcherLookup = 3, /* Dictionary = 1,*/ FieldCount = 4/*2*/, TypeArgCount = 5/*3*/, MethodEnsureFunction = 6, PreallocatedSlots = 7/*4*/, SpecializedVTableCastID = 8/*5*/, SpecializedVTable = 9/*6*/ };
		class RTStruct : public ARTRep<RTStruct, RTStructFields>
		{
		public:
			static llvm::StructType* GetLLVMType();
			static llvm::Constant* CreateConstant(const NomStruct* structptr, llvm::Function* fieldRead, llvm::Function* fieldWrite, llvm::Function* lookupDispatcher, llvm::Function* ensureMethodFunction/*, llvm::Constant* dictPointer*/);
			static llvm::Value* GenerateReadMethodEnsure(NomBuilder& builder, llvm::Value* descriptor);
			static llvm::Value* GenerateReadPreallocatedSlots(NomBuilder& builder, llvm::Value* descriptor);
			static llvm::Value* GenerateReadFieldLookup(NomBuilder& builder, llvm::Value* descriptorPtr);
			static llvm::Value* GenerateReadFieldStore(NomBuilder& builder, llvm::Value* descriptorPtr);
			static llvm::Value* GenerateReadDispatcherLookup(NomBuilder& builder, llvm::Value* descriptorPtr);
			//static llvm::Value* GenerateReadDictionary(NomBuilder& builder, llvm::Value* descriptor);
			static llvm::Value* GenerateReadFieldCount(NomBuilder& builder, llvm::Value* descriptor);
			static llvm::Value* GenerateReadTypeArgCount(NomBuilder& builder, llvm::Value* descriptor);
			static llvm::Value* GenerateReadSpecializedVTableCastID(NomBuilder& builder, llvm::Value* descriptor);
			static llvm::Value* GenerateReadSpecializedVTable(NomBuilder& builder, llvm::Value* descriptor);
			static llvm::Value* GenerateReadSignature(NomBuilder& builder, llvm::Value* descriptor);
			static void GenerateWriteSpecializedVTable(NomBuilder& builder, llvm::Value* descriptor, llvm::Value* castID, llvm::Value* vtable);
			//static llvm::Value* GenerateReadDescriptorKind(NomBuilder& builder, llvm::Module& mod, llvm::Value* descriptorPtr);
			//static llvm::Value* GenerateReadDescriptorLink(NomBuilder& builder, llvm::Module& mod, llvm::Value* descriptorPtr);
			//static llvm::Value* GenerateReadDescriptorDictionary(NomBuilder& builder, llvm::Module& mod, llvm::Value* descriptorPtr);
			//static llvm::Value* GenerateReadFieldCount(NomBuilder& builder, llvm::Module& mod, llvm::Value* descriptorPtr);
			//static llvm::Value* GenerateReadSuperInstanceCount(NomBuilder& builder, llvm::Module& mod, llvm::Value* descriptorPtr);
			//static llvm::Value* GenerateReadSuperInstances(NomBuilder& builder, llvm::Module& mod, llvm::Value* descriptorPtr);
			//static llvm::Value* GenerateReadInterfaceTableSize(NomBuilder& builder, llvm::Module& mod, llvm::Value* descriptorPtr);
			//static llvm::Value* GenerateReadInterfaceTable(NomBuilder& builder, llvm::Module& mod, llvm::Value* descriptorPtr);
		};
	}
}