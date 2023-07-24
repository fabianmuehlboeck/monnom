#pragma once
#include <vector>
PUSHDIAGSUPPRESSION
#include "llvm/ADT/TinyPtrVector.h"
#include "llvm/IR/DerivedTypes.h"
POPDIAGSUPPRESSION
#include "Context.h"
#include "Defs.h"
#include <list>
#include <tuple>
#include "NomJIT.h"
#include "NomBuilder.h"


//namespace Nom
//{
//	namespace Runtime
//	{
//		enum class RTInterfaceTableEntryFields : unsigned char { InterfaceKey = 0, MethodOffset = 1 };
//		class RTInterfaceTableEntry
//		{
//		private:
//			RTInterfaceTableEntry();
//		public:
//			static llvm::Constant* CreateConstant(InterfaceID interfaceId, int32_t offset);
//			static llvm::StructType* GetLLVMType();
//			static llvm::Value* GenerateReadKey(NomBuilder& builder, llvm::Value* entry);
//			static llvm::Value* GenerateReadMethodOffset(NomBuilder& builder, llvm::Value* entry);
//		};
//
//	}
//}
