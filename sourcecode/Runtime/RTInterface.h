#pragma once
#include "ARTRep.h"
#include "llvm/IR/DerivedTypes.h"
#include "RTDescriptor.h"
#include "NomBuilder.h"
#include "BoehmAllocator.h"
#include <unordered_map>

namespace Nom
{
	namespace Runtime
	{
		enum class RTInterfaceFlags : unsigned char { None = 0x00, IsInterface = 0x01, IsFunctional = 0x02 };

		inline RTInterfaceFlags operator | (RTInterfaceFlags lhs, RTInterfaceFlags rhs)
		{
			using T = std::underlying_type_t <RTInterfaceFlags>;
			return static_cast<RTInterfaceFlags>(static_cast<T>(lhs) | static_cast<T>(rhs));
		}
		inline RTInterfaceFlags operator & (RTInterfaceFlags lhs, RTInterfaceFlags rhs)
		{
			using T = std::underlying_type_t <RTInterfaceFlags>;
			return static_cast<RTInterfaceFlags>(static_cast<T>(lhs) & static_cast<T>(rhs));
		}

		enum class RTInterfaceFields : unsigned char { RTVTable = 0, Flags = 1, TypeArgCount = 2, SuperTypesCount = 3, SuperTypes = 4, MethodEnsureFunction = 5, InstantiationDict = 6, Signature = 7 };
		class RTInterface : public ARTRep<RTInterface, RTInterfaceFields>
		{
		public:
			static llvm::StructType* GetLLVMType();

			static llvm::Constant* CreateConstant(const NomInterface* irptr, RTInterfaceFlags flags, llvm::Constant* typeArgCount, llvm::Constant* superTypesCount, llvm::Constant* superTypeEntries, llvm::Constant* interfaceMethodTable, llvm::Constant* checkReturnValueFunction, llvm::Constant* methodEnsureFunction, llvm::Constant* instantiationDictionary, llvm::Constant* signature);
			static llvm::Constant* CreateConstant(RTDescriptorKind kind, const NomInterface* irptr, RTInterfaceFlags flags, llvm::Constant* typeArgCount, llvm::Constant* superTypesCount, llvm::Constant* superTypeEntries, llvm::Constant* interfaceMethodTable, llvm::Constant* checkReturnValueFunction, llvm::Constant* methodEnsureFunction, llvm::Constant* instantiationDictionary, llvm::Constant* signature);
			static llvm::Value* GenerateReadKind(NomBuilder& builder, llvm::Value* descriptorPtr);
			static llvm::Value* GenerateReadNomIRLink(NomBuilder& builder, llvm::Value* descriptorPtr);
			static llvm::Value* GenerateReadSuperInstanceCount(NomBuilder& builder, llvm::Value* descriptorPtr);
			static llvm::Value* GenerateReadSuperInstances(NomBuilder& builder, llvm::Value* descriptorPtr);
			static llvm::Value* GenerateReadMethodTableEntry(NomBuilder& builder, llvm::Value* vtablePtr, llvm::Value* offset);
			static llvm::Value* GenerateReadTypeArgCount(NomBuilder& builder, llvm::Value* descriptorPtr);
			static llvm::Value* GenerateReadSignature(NomBuilder& builder, llvm::Value* descriptorPtr);

			static llvm::Value* GenerateReadFlags(NomBuilder& builder, llvm::Value* descriptorPtr);

			static llvm::Value* GenerateReadMethodEnsure(NomBuilder& builder, llvm::Value* descriptorPtr);

			static void GenerateInitialization(NomBuilder& builder, llvm::Value* ifcptr, llvm::Value* /*vt_ifcoffset*/ vt_imtptr, llvm::Value* vt_kind, llvm::Value* vt_irdesc, llvm::Value* flags, llvm::Value* targcount, llvm::Value* supercount, llvm::Value* superentries);
		};
	}
}