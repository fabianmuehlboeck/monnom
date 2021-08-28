#include "RTInstanceType.h"
#include "CompileHelpers.h"
#include "RTInterface.h"

namespace Nom
{
	namespace Runtime
	{
		llvm::StructType* RTInstanceType::GetLLVMType()
		{
			static llvm::StructType* type = llvm::StructType::create("NOM_RTT_InstanceType", RTTypeHead::GetLLVMType(), RTInterface::GetLLVMType()->getPointerTo(), TYPETYPE->getPointerTo());
			return type;
		}
		void RTInstanceType::CreateInitialization(NomBuilder& builder, llvm::Module& mod, llvm::Value* ptr, llvm::Value* hash, llvm::Value* nomtypeptr, llvm::Value *rtclassdesc, llvm::Value *ptrToTypeArgs)
		{
			RTTypeHead::CreateInitialization(builder, mod, builder->CreateGEP(ptr, { MakeInt32(0), MakeInt32((unsigned char)RTInstanceTypeFields::Head) }), TypeKind::TKInstance, hash, nomtypeptr);
			MakeInvariantStore(builder, mod, rtclassdesc, builder->CreateGEP(ptr, { MakeInt32(0), MakeInt32((unsigned char)RTInstanceTypeFields::Class) }));
			MakeInvariantStore(builder, mod, ptrToTypeArgs, builder->CreateGEP(ptr, { MakeInt32(0), MakeInt32((unsigned char)RTInstanceTypeFields::TypeArgs) }));
		}
		llvm::Value* RTInstanceType::GenerateReadClassDescriptorLink(NomBuilder& builder, llvm::Value* type)
		{
			return MakeInvariantLoad(builder, type, GetLLVMPointerType(), MakeInt32(RTInstanceTypeFields::Class), "classDescriptor");
		}
		llvm::Value* RTInstanceType::GetTypeArgumentsPtr(NomBuilder& builder, llvm::Value* type)
		{
			return MakeInvariantLoad(builder, type, GetLLVMPointerType(), MakeInt32(RTInstanceTypeFields::TypeArgs), "typeArguments");
		}
	}
}