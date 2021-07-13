#pragma once
#include "Defs.h"
#include "llvm/IR/Constant.h"
#include "llvm/IR/Constants.h"
#include "ARTRep.h"
#include "NomBuilder.h"

#define DICTIONARYTYPE POINTERTYPE

namespace Nom
{
	namespace Runtime
	{
		enum class RTDescriptorDictionaryEntryKind : unsigned char { Field = 0, PartialApp = 1, Dispatcher = 2 };
		enum class RTDescriptorDictionaryEntryFields : unsigned char { Kind = 0, Flags = 1, Visibility = 2, Reserved = 3, Index = 4, TypeOrPartialAppOrDispatcher = 5 };


		/// <summary>
		/// Also bit-encoded as follows:
		/// LSB = 0 -> type arguments recorded in instance
		/// </summary>
		enum class RTDescriptorKind : unsigned char { Class = 0, Lambda = 1, OptimizedLambda = 2, Struct = 3, OptimizedStruct = 4, PartialApplication = 5, MultiCast = 7 };
		//llvm::StructType* GetDescriptorDictionaryEntryType();
		//llvm::Constant* CreateDescriptorDictionaryEntryConstant(DescriptorDictionaryEntryKind det, bool readonly, Visibility visibility, llvm::Constant* typeOrPartialAppOrDispatcher, int32_t index);

		class RTDescriptorDictionaryEntry : public ARTRep<RTDescriptorDictionaryEntry, RTDescriptorDictionaryEntryFields>
		{
		public:
			static llvm::StructType* GetLLVMType();
			static llvm::Constant* CreateConstant(RTDescriptorDictionaryEntryKind det, bool readonly, Visibility visibility, llvm::Constant* typeOrPartialAppOrDispatcher, int32_t index, bool rawInt, bool rawFloat);

			static llvm::Value* GenerateReadKind(NomBuilder& builder, llvm::Value* entry);
			static llvm::Value* GenerateReadFlags(NomBuilder& builder, llvm::Value* entry);
			static llvm::Value* GenerateReadVisibility(NomBuilder& builder, llvm::Value* entry);
			static llvm::Value* GenerateReadIndex(NomBuilder& builder, llvm::Value* entry);
			static llvm::Value* GenerateReadTypeOrPartialAppOrDispatcher(NomBuilder& builder, llvm::Value* entry);
		};
	}
}