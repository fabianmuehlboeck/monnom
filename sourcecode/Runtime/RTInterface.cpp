#include "RTInterface.h"
#include "Defs.h"
#include "RTVTable.h"
#include "NomInterface.h"
#include "CompileHelpers.h"
#include "RTSignature.h"
#include "IMT.h"
#include "RTTypeHead.h"

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
				rtit->setBody({ POINTERTYPE,							//Pointer to IR representation
					numtype(RTInterfaceFlags),							//flags
					inttype(32),										//type arg count
					numtype(size_t),									//super instances count
					numtype(size_t),									//superclass count
					numtype(size_t),									//superinterface count
					SuperInstanceEntryType()->getPointerTo(),			//superclasses
					SuperInstanceEntryType()->getPointerTo(),			//superinterfaces
					GetCheckReturnValueFunctionType()->getPointerTo(),  //method ensure function, untyped version
					POINTERTYPE,										//pointer to type instantiations dictionary
					RTSignature::GetLLVMType()->getPointerTo(),			//signature of lambda method if present, otherwise null
					GetCastFunctionType()->getPointerTo()				//cast function for run-time class-type instantiations
					});
			}
			return rtit;
		}

		llvm::Constant* RTInterface::CreateConstant(const NomInterface* irptr, RTInterfaceFlags flags, llvm::Constant* typeArgCount, llvm::Constant* superClassCount, llvm::Constant* superInterfaceCount, llvm::Constant* superTypeEntries, llvm::Constant* checkReturnValueFunction, llvm::Constant* instantiationDictionary, llvm::Constant* signature, llvm::Constant* castFunction)
		{
			auto STEs = ConstantExpr::getPointerCast(superTypeEntries, SuperInstanceEntryType()->getPointerTo());
			return ConstantStruct::get(GetLLVMType(), 
				GetLLVMPointer(irptr), 
				MakeInt<RTInterfaceFlags>(flags), 
				EnsureIntegerSize(typeArgCount, 32), 
				EnsureIntegerSize(ConstantExpr::getAdd(superClassCount, superInterfaceCount), bitsin(size_t)), 
				EnsureIntegerSize(superClassCount, bitsin(size_t)), 
				EnsureIntegerSize(superInterfaceCount, bitsin(size_t)), 
				STEs, 
				ConstantExpr::getGetElementPtr(SuperInstanceEntryType(), STEs, ArrayRef<Constant*>({ superClassCount })), 
				checkReturnValueFunction, 
				instantiationDictionary, 
				(signature == nullptr ? ConstantPointerNull::get(RTSignature::GetLLVMType()->getPointerTo()) : signature), 
				castFunction);
		}

		llvm::Value* RTInterface::GenerateReadSignature(NomBuilder& builder, llvm::Value* descriptorPtr)
		{
			return MakeInvariantLoad(builder, builder->CreatePointerCast(descriptorPtr, GetLLVMType()->getPointerTo()), MakeInt32(RTInterfaceFields::Signature), "Signature", AtomicOrdering::NotAtomic);
		}
		llvm::Value* RTInterface::GenerateReadReturnValueCheckFunction(NomBuilder& builder, llvm::Value* descriptorPtr)
		{
			return MakeInvariantLoad(builder, builder->CreatePointerCast(descriptorPtr, GetLLVMType()->getPointerTo()), MakeInt32(RTInterfaceFields::ReturnValueCheckFunction), "ReturnValueCheckFunction", AtomicOrdering::NotAtomic);
		}
		llvm::Value* RTInterface::GenerateReadSuperInstanceCount(NomBuilder& builder, llvm::Value* descriptorPtr)
		{
			return MakeInvariantLoad(builder, builder->CreatePointerCast(descriptorPtr, GetLLVMType()->getPointerTo()), MakeInt32(RTInterfaceFields::SuperTypesCount), "SuperTypesCount", AtomicOrdering::NotAtomic);
		}
		llvm::Value* RTInterface::GenerateReadSuperClassCount(NomBuilder& builder, llvm::Value* descriptorPtr)
		{
			return MakeInvariantLoad(builder, builder->CreatePointerCast(descriptorPtr, GetLLVMType()->getPointerTo()), MakeInt32(RTInterfaceFields::SuperClassCount), "SuperClassCount", AtomicOrdering::NotAtomic);
		}
		llvm::Value* RTInterface::GenerateReadSuperInterfaceCount(NomBuilder& builder, llvm::Value* descriptorPtr)
		{
			return MakeInvariantLoad(builder, builder->CreatePointerCast(descriptorPtr, GetLLVMType()->getPointerTo()), MakeInt32(RTInterfaceFields::SuperInterfaceCount), "SuperInterfaceCount", AtomicOrdering::NotAtomic);
		}
		llvm::Value* RTInterface::GenerateReadSuperInstances(NomBuilder& builder, llvm::Value* descriptorPtr)
		{
			return MakeInvariantLoad(builder, builder->CreatePointerCast(descriptorPtr, GetLLVMType()->getPointerTo()), MakeInt32(RTInterfaceFields::SuperTypes), "SuperTypes", AtomicOrdering::NotAtomic);
		}
		llvm::Value* RTInterface::GenerateReadSuperInterfaces(NomBuilder& builder, llvm::Value* descriptorPtr)
		{
			return MakeInvariantLoad(builder, builder->CreatePointerCast(descriptorPtr, GetLLVMType()->getPointerTo()), MakeInt32(RTInterfaceFields::SuperInterfaces), "SuperInterfaces", AtomicOrdering::NotAtomic);
		}
		llvm::Value* RTInterface::GenerateReadTypeArgCount(NomBuilder& builder, llvm::Value* descriptorPtr)
		{
			return MakeInvariantLoad(builder, builder->CreateGEP(builder->CreatePointerCast(descriptorPtr, GetLLVMType()->getPointerTo()), { MakeInt32(0), MakeInt32(RTInterfaceFields::TypeArgCount) }), "TypeArgumentCount", AtomicOrdering::NotAtomic);
		}
		llvm::Value* RTInterface::GenerateReadFlags(NomBuilder& builder, llvm::Value* descriptorPtr)
		{
			return MakeInvariantLoad(builder, builder->CreateGEP(builder->CreatePointerCast(descriptorPtr, GetLLVMType()->getPointerTo()), { MakeInt32(0), MakeInt32(RTInterfaceFields::Flags) }), "InterfaceFlags", AtomicOrdering::NotAtomic);
		}
		llvm::Value* RTInterface::GenerateReadIRPtr(NomBuilder& builder, llvm::Value* descriptorPtr)
		{
			return MakeInvariantLoad(builder, builder->CreateGEP(builder->CreatePointerCast(descriptorPtr, GetLLVMType()->getPointerTo()), { MakeInt32(0), MakeInt32(RTInterfaceFields::IRPtr) }), "InterfaceFlags", AtomicOrdering::NotAtomic);
		}
		llvm::Value* RTInterface::GenerateReadCastFunction(NomBuilder& builder, llvm::Value* descriptorPtr)
		{
			return MakeInvariantLoad(builder, builder->CreatePointerCast(descriptorPtr, GetLLVMType()->getPointerTo()), MakeInt32(RTInterfaceFields::CastFunction), "CastFunction", AtomicOrdering::NotAtomic);
		}
	}
}