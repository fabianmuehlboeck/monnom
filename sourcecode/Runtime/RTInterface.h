#pragma once
#include "ARTRep.h"
PUSHDIAGSUPPRESSION
#include "llvm/IR/DerivedTypes.h"
POPDIAGSUPPRESSION
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

		enum class RTInterfaceFields : unsigned char { IRPtr = 0, Flags = 1, TypeArgCount = 2, SuperTypesCount = 3, SuperClassCount = 4, SuperInterfaceCount = 5, SuperTypes = 6, SuperInterfaces = 7, ReturnValueCheckFunction = 8, InstantiationDict = 9, Signature = 10, CastFunction = 11 };
		class RTInterface : public ARTRep<RTInterface, RTInterfaceFields>
		{
		public:
			static llvm::StructType* GetLLVMType();

			static llvm::Constant* CreateConstant(const NomInterface* irptr, RTInterfaceFlags flags, llvm::Constant* typeArgCount, llvm::Constant* superClassCount, llvm::Constant* superInterfaceCount, llvm::Constant* superTypeEntries, llvm::Constant* checkReturnValueFunction, llvm::Constant* instantiationDictionary, llvm::Constant* signature);
			static llvm::Value* GenerateReadSuperInstanceCount(NomBuilder& builder, llvm::Value* descriptorPtr);
			static llvm::Value* GenerateReadSuperClassCount(NomBuilder& builder, llvm::Value* descriptorPtr);
			static llvm::Value* GenerateReadSuperInterfaceCount(NomBuilder& builder, llvm::Value* descriptorPtr);
			static llvm::Value* GenerateReadSuperInstances(NomBuilder& builder, llvm::Value* descriptorPtr);
			static llvm::Value* GenerateReadSuperInterfaces(NomBuilder& builder, llvm::Value* descriptorPtr);
			static llvm::Value* GenerateReadTypeArgCount(NomBuilder& builder, llvm::Value* descriptorPtr);
			static llvm::Value* GenerateReadSignature(NomBuilder& builder, llvm::Value* descriptorPtr);
			static llvm::Value* GenerateReadReturnValueCheckFunction(NomBuilder& builder, llvm::Value* descriptorPtr);

			static llvm::Value* GenerateReadFlags(NomBuilder& builder, llvm::Value* descriptorPtr);
			static llvm::Value* GenerateReadIRPtr(NomBuilder& builder, llvm::Value* descriptorPtr);

		};
	}
}
