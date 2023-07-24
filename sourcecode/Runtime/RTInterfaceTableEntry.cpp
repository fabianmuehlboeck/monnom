#include "RTInterfaceTableEntry.h"
#include "RTInterface.h"
#include "CompileHelpers.h"

//using namespace llvm;
//using namespace std;
//namespace Nom
//{
//	namespace Runtime
//	{
//		llvm::Constant* RTInterfaceTableEntry::CreateConstant(InterfaceID interfaceId, int32_t offset)
//		{
//			return ConstantStruct::get(GetLLVMType(), MakeInt32(interfaceId), MakeInt32(-offset));
//		}
//		llvm::StructType* RTInterfaceTableEntry::GetLLVMType()
//		{
//			static llvm::StructType* llvmtype = llvm::StructType::create(LLVMCONTEXT, "RT_NOM_InterfaceTableEntry");
//			static bool once = true;
//			if (once)
//			{
//				once = false;
//				llvmtype->setBody(numtype(32), numtype(32));
//			}
//			return llvmtype;
//		}
//		llvm::Value* RTInterfaceTableEntry::GenerateReadKey([[maybe_unused]] NomBuilder& builder, [[maybe_unused]] llvm::Value* entry)
//		{
//			throw new std::exception();
//			//return MakeLoad(builder, builder->CreatePointerCast(entry, GetLLVMType()->getPointerTo()), MakeInt32(RTInterfaceTableEntryFields::InterfaceKey));
//		}
//		llvm::Value* RTInterfaceTableEntry::GenerateReadMethodOffset(NomBuilder& builder, llvm::Value* entry)
//		{
//			return MakeLoad(builder, builder->CreatePointerCast(entry, GetLLVMType()->getPointerTo()), MakeInt32(RTInterfaceTableEntryFields::MethodOffset));
//		}
//	}
//}
