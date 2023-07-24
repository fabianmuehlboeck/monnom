#include "RTInstanceType.h"
#include "CompileHelpers.h"
#include "RTInterface.h"
#include "PWInstanceType.h"
#include "PWTypeArr.h"
#include "PWInterface.h"
#include "PWNomType.h"

using namespace llvm;
namespace Nom
{
	namespace Runtime
	{
		llvm::StructType* RTInstanceType::GetLLVMType()
		{
			static llvm::StructType* type = llvm::StructType::create("NOM_RTT_InstanceType", RTTypeHead::GetLLVMType(), RTInterface::GetLLVMType()->getPointerTo(), TYPETYPE->getPointerTo());
			return type;
		}
		void RTInstanceType::CreateInitialization(NomBuilder& builder, [[maybe_unused]] llvm::Module& mod, llvm::Value* ptr, llvm::Value* hash, llvm::Value* nomtypeptr, llvm::Value* rtclassdesc, llvm::Value* ptrToTypeArgs)
		{
			PWInstanceType(ptr).InitializeInstanceType(builder, hash, nomtypeptr, rtclassdesc, ptrToTypeArgs);
		}
		llvm::Value* RTInstanceType::GenerateReadClassDescriptorLink(NomBuilder& builder, llvm::Value* type)
		{
			return PWInstanceType(type).ReadClassDescriptorLink(builder);
		}
		llvm::Value* RTInstanceType::GetTypeArgumentsPtr(NomBuilder& builder, llvm::Value* type)
		{
			return PWInstanceType(type).TypeArgumentsPtr(builder);
		}
	}
}
