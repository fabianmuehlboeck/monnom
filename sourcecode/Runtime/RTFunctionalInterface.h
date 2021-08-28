#pragma once
#include "ARTRep.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Module.h"
#include "llvm/ADT/Twine.h"
#include "llvm/ADT/StringRef.h"
#include "NomBuilder.h"

namespace Nom
{
	namespace Runtime
	{
		enum class RTFunctionalInterfaceFields : unsigned char { RTInterface = 0, CheckedWrapper = 1, ProtoMismatchWrapper = 2, CastingWrapper = 3, CheckedVTableOpt = 4, CheckedVTable = 5, ProtoMismatchVTableOpt = 6, ProtoMismatchVTable = 7, CastingVTableOpt = 8, CastingVTable = 9 };
		class RTFunctionalInterface : public ARTRep<RTFunctionalInterface, RTFunctionalInterfaceFields>
		{
		public:
			static llvm::StructType* GetLLVMType();
			static llvm::Constant* CreateGlobalConstant(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage, const llvm::Twine& name, const NomInterface* irptr, llvm::Constant* typeArgCount, llvm::Constant* superTypesCount, llvm::Constant* superTypeEntries, llvm::Constant* instantiationDictionary);

			static llvm::Constant* FindConstant(llvm::Module& mod, const llvm::StringRef name);

			static llvm::Value* GenerateReadCheckedRawInvokeWrapper(NomBuilder& builder, llvm::Value* descriptorPtr);
			static llvm::Value* GenerateReadProtoMismatchRawInvokeWrapper(NomBuilder& builder, llvm::Value* descriptorPtr);
			static llvm::Value* GenerateReadCastingRawInvokeWrapper(NomBuilder& builder, llvm::Value* descriptorPtr);
			static llvm::Value* GenerateReadRTSignature(NomBuilder& builder, llvm::Value* descriptorPtr);
			static llvm::Value* GenerateReadProtoMatchVTableOpt(NomBuilder& builder, llvm::Value* descriptorPtr);
			static llvm::Value* GenerateReadProtoMatchVTable(NomBuilder& builder, llvm::Value* descriptorPtr);
			static llvm::Value* GenerateReadProtoMismatchVTableOpt(NomBuilder& builder, llvm::Value* descriptorPtr);
			static llvm::Value* GenerateReadProtoMismatchVTable(NomBuilder& builder, llvm::Value* descriptorPtr);
			static llvm::Value* GenerateReadCastingVTableOpt(NomBuilder& builder, llvm::Value* descriptorPtr);
			static llvm::Value* GenerateReadCastingVTable(NomBuilder& builder, llvm::Value* descriptorPtr);
		};
	}
}