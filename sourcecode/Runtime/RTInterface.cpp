#include "RTInterface.h"
#include "Defs.h"
#include "RTVTable.h"
#include "NomInterface.h"
#include "CompileHelpers.h"

using namespace llvm;
using namespace std;
namespace Nom
{
	namespace Runtime
	{
		llvm::StructType* RTInterface::GetLLVMType()
		{
			static llvm::StructType* rtit = llvm::StructType::create(LLVMCONTEXT, "NOM_RT_Interface");
			static bool once = true;
			if (once)
			{
				once = false;
				rtit->setBody({ RTVTable::GetLLVMType(),			//interface table
					numtype(RTInterfaceFlags),						//flags
					inttype(32),									//type arg count
					numtype(size_t),								//super instances count
					SuperInstanceEntryType()->getPointerTo(),		//super instances
					POINTERTYPE										//pointer to type instantiations dictionary
					});
			}
			return rtit;
		}
		llvm::Constant* RTInterface::CreateConstant(const NomInterface* irptr, RTInterfaceFlags flags, llvm::Constant* typeArgCount, llvm::Constant* superTypesCount, llvm::Constant* superTypeEntries, llvm::Constant* interfaceMethodTable, llvm::Constant* instantiationDictionary)
		{
			return CreateConstant(RTDescriptorKind::Class, irptr, flags, typeArgCount, superTypesCount, superTypeEntries, interfaceMethodTable, instantiationDictionary);
		}
			
		llvm::Constant* RTInterface::CreateConstant(RTDescriptorKind kind, const NomInterface* irptr, RTInterfaceFlags flags, llvm::Constant* typeArgCount, llvm::Constant* superTypesCount, llvm::Constant* superTypeEntries, llvm::Constant* interfaceMethodTable, llvm::Constant* instantiationDictionary)
		{
			return ConstantStruct::get(GetLLVMType(), RTVTable::CreateConstant(kind, irptr, interfaceMethodTable), MakeInt<RTInterfaceFlags>(flags), EnsureIntegerSize(typeArgCount, 32), EnsureIntegerSize(superTypesCount, bitsin(size_t)), superTypeEntries, instantiationDictionary);
		}

		llvm::Value* RTInterface::GenerateReadKind(NomBuilder& builder, llvm::Value* descriptorPtr)
		{
			return RTVTable::GenerateReadKind(builder, builder->CreateGEP(descriptorPtr, { MakeInt32(0), MakeInt32(RTInterfaceFields::RTVTable) }));
		}

		llvm::Value* RTInterface::GenerateReadNomIRLink(NomBuilder& builder, llvm::Value* descriptorPtr)
		{
			return RTVTable::GenerateReadNomIRLink(builder, builder->CreateGEP(descriptorPtr, { MakeInt32(0), MakeInt32(RTInterfaceFields::RTVTable) }));
		}
		llvm::Value* RTInterface::GenerateReadSuperInstanceCount(NomBuilder& builder, llvm::Value* descriptorPtr)
		{
			return MakeLoad(builder, builder->CreatePointerCast(descriptorPtr, GetLLVMType()->getPointerTo()), MakeInt32(RTInterfaceFields::SuperTypesCount));
		}
		llvm::Value* RTInterface::GenerateReadSuperInstances(NomBuilder& builder, llvm::Value* descriptorPtr)
		{
			return MakeLoad(builder, builder->CreatePointerCast(descriptorPtr, GetLLVMType()->getPointerTo()), MakeInt32(RTInterfaceFields::SuperTypes));
		}
		llvm::Value* RTInterface::GenerateReadMethodTableEntry(NomBuilder& builder, llvm::Value* vtablePtr, llvm::Value* offset)
		{
			return RTVTable::GenerateReadMethodTableEntry(builder, builder->CreateGEP(builder->CreatePointerCast(vtablePtr, GetLLVMPointerType()), { MakeInt32(0), MakeInt32(RTInterfaceFields::RTVTable) }), offset);
		}
		llvm::Value* RTInterface::GenerateReadTypeArgCount(NomBuilder& builder, llvm::Value* descriptorPtr)
		{
			return MakeLoad(builder, builder->CreateGEP(builder->CreatePointerCast(descriptorPtr, GetLLVMType()->getPointerTo()), { MakeInt32(0), MakeInt32(RTInterfaceFields::TypeArgCount) }));;
		}
		llvm::Value* RTInterface::GenerateReadFlags(NomBuilder& builder, llvm::Value* descriptorPtr)
		{
			return MakeLoad(builder, builder->CreateGEP(builder->CreatePointerCast(descriptorPtr, GetLLVMType()->getPointerTo()), { MakeInt32(0), MakeInt32(RTInterfaceFields::Flags) }));;
		}
		void RTInterface::GenerateInitialization(NomBuilder& builder, llvm::Value* ifcptr, llvm::Value* /*vt_ifcoffset*/ vt_imtptr, llvm::Value* vt_kind, llvm::Value* vt_irdesc, llvm::Value* flags, llvm::Value* targcount, llvm::Value* supercount, llvm::Value* superentries)
		{
			RTVTable::GenerateInitialization(builder, ifcptr, /*vt_ifcoffset*/ vt_imtptr, vt_kind, vt_irdesc);
			llvm::Value* selfptr = builder->CreatePointerCast(ifcptr, GetLLVMType()->getPointerTo());
			MakeStore(builder, flags, selfptr, MakeInt32(RTInterfaceFields::Flags));
			MakeStore(builder, targcount, selfptr, MakeInt32(RTInterfaceFields::TypeArgCount));
			MakeStore(builder, supercount, selfptr, MakeInt32(RTInterfaceFields::SuperTypesCount));
			MakeStore(builder, superentries, selfptr, MakeInt32(RTInterfaceFields::SuperTypes));
		}
	}
}