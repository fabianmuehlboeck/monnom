#include "RTInterface.h"
#include "Defs.h"
#include "RTVTable.h"
#include "NomInterface.h"
#include "CompileHelpers.h"
#include "RTSignature.h"

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
					GetMethodEnsureFunctionType()->getPointerTo(),  //method ensure function, untyped version
					POINTERTYPE,									//pointer to type instantiations dictionary
					RTSignature::GetLLVMType()->getPointerTo()		//signature of lambda method if present, otherwise null
					});
			}
			return rtit;
		}
		llvm::Constant* RTInterface::CreateConstant(const NomInterface* irptr, RTInterfaceFlags flags, llvm::Constant* typeArgCount, llvm::Constant* superTypesCount, llvm::Constant* superTypeEntries, llvm::Constant* interfaceMethodTable, llvm::Constant* checkReturnValueFunction, llvm::Constant* methodEnsureFunction, llvm::Constant* instantiationDictionary, llvm::Constant* signature)
		{
			return CreateConstant(RTDescriptorKind::Class, irptr, flags, typeArgCount, superTypesCount, superTypeEntries, interfaceMethodTable, checkReturnValueFunction, methodEnsureFunction, instantiationDictionary, signature);
		}
			
		llvm::Constant* RTInterface::CreateConstant(RTDescriptorKind kind, const NomInterface* irptr, RTInterfaceFlags flags, llvm::Constant* typeArgCount, llvm::Constant* superTypesCount, llvm::Constant* superTypeEntries, llvm::Constant* interfaceMethodTable, llvm::Constant* checkReturnValueFunction, llvm::Constant* methodEnsureFunction, llvm::Constant* instantiationDictionary, llvm::Constant * signature)
		{
			return ConstantStruct::get(GetLLVMType(), RTVTable::CreateConstant(kind, irptr, interfaceMethodTable, checkReturnValueFunction), MakeInt<RTInterfaceFlags>(flags), EnsureIntegerSize(typeArgCount, 32), EnsureIntegerSize(superTypesCount, bitsin(size_t)), superTypeEntries, methodEnsureFunction, instantiationDictionary, signature);
		}

		llvm::Value* RTInterface::GenerateReadKind(NomBuilder& builder, llvm::Value* descriptorPtr)
		{
			return RTVTable::GenerateReadKind(builder, builder->CreateGEP(descriptorPtr, { MakeInt32(0), MakeInt32(RTInterfaceFields::RTVTable) }));
		}
		llvm::Value* RTInterface::GenerateReadSignature(NomBuilder& builder, llvm::Value* descriptorPtr)
		{
			return MakeInvariantLoad(builder, builder->CreatePointerCast(descriptorPtr, GetLLVMType()->getPointerTo()), MakeInt32(RTInterfaceFields::Signature), "Signature", AtomicOrdering::NotAtomic);
		}

		llvm::Value* RTInterface::GenerateReadNomIRLink(NomBuilder& builder, llvm::Value* descriptorPtr)
		{
			return RTVTable::GenerateReadNomIRLink(builder, builder->CreateGEP(descriptorPtr, { MakeInt32(0), MakeInt32(RTInterfaceFields::RTVTable) }));
		}
		llvm::Value* RTInterface::GenerateReadSuperInstanceCount(NomBuilder& builder, llvm::Value* descriptorPtr)
		{
			return MakeInvariantLoad(builder, builder->CreatePointerCast(descriptorPtr, GetLLVMType()->getPointerTo()), MakeInt32(RTInterfaceFields::SuperTypesCount), "SuperTypesCount", AtomicOrdering::NotAtomic);
		}
		llvm::Value* RTInterface::GenerateReadSuperInstances(NomBuilder& builder, llvm::Value* descriptorPtr)
		{
			return MakeInvariantLoad(builder, builder->CreatePointerCast(descriptorPtr, GetLLVMType()->getPointerTo()), MakeInt32(RTInterfaceFields::SuperTypes),"SuperTypes", AtomicOrdering::NotAtomic);
		}
		llvm::Value* RTInterface::GenerateReadMethodTableEntry(NomBuilder& builder, llvm::Value* vtablePtr, llvm::Value* offset)
		{
			return RTVTable::GenerateReadMethodTableEntry(builder, builder->CreateGEP(builder->CreatePointerCast(vtablePtr, GetLLVMPointerType()), { MakeInt32(0), MakeInt32(RTInterfaceFields::RTVTable) }), offset);
		}
		llvm::Value* RTInterface::GenerateReadTypeArgCount(NomBuilder& builder, llvm::Value* descriptorPtr)
		{
			return MakeInvariantLoad(builder, builder->CreateGEP(builder->CreatePointerCast(descriptorPtr, GetLLVMType()->getPointerTo()), { MakeInt32(0), MakeInt32(RTInterfaceFields::TypeArgCount) }), "TypeArgumentCount", AtomicOrdering::NotAtomic);
		}
		llvm::Value* RTInterface::GenerateReadFlags(NomBuilder& builder, llvm::Value* descriptorPtr)
		{
			return MakeInvariantLoad(builder, builder->CreateGEP(builder->CreatePointerCast(descriptorPtr, GetLLVMType()->getPointerTo()), { MakeInt32(0), MakeInt32(RTInterfaceFields::Flags) }), "InterfaceFlags", AtomicOrdering::NotAtomic);
		}
		llvm::Value* RTInterface::GenerateReadMethodEnsure(NomBuilder& builder, llvm::Value* descriptorPtr)
		{
			return MakeInvariantLoad(builder, builder->CreateGEP(builder->CreatePointerCast(descriptorPtr, GetLLVMType()->getPointerTo()), { MakeInt32(0), MakeInt32(RTInterfaceFields::MethodEnsureFunction) }), "EnsureMethodFunction", AtomicOrdering::NotAtomic);
		}
		void RTInterface::GenerateInitialization(NomBuilder& builder, llvm::Value* ifcptr, llvm::Value* /*vt_ifcoffset*/ vt_imtptr, llvm::Value* vt_kind, llvm::Value* vt_irdesc, llvm::Value* flags, llvm::Value* targcount, llvm::Value* supercount, llvm::Value* superentries)
		{
			RTVTable::GenerateInitialization(builder, ifcptr, /*vt_ifcoffset*/ vt_imtptr, vt_kind, vt_irdesc);
			llvm::Value* selfptr = builder->CreatePointerCast(ifcptr, GetLLVMType()->getPointerTo());
			MakeInvariantStore(builder, flags, selfptr, MakeInt32(RTInterfaceFields::Flags));
			MakeInvariantStore(builder, targcount, selfptr, MakeInt32(RTInterfaceFields::TypeArgCount));
			MakeInvariantStore(builder, supercount, selfptr, MakeInt32(RTInterfaceFields::SuperTypesCount));
			MakeInvariantStore(builder, superentries, selfptr, MakeInt32(RTInterfaceFields::SuperTypes));
		}
	}
}