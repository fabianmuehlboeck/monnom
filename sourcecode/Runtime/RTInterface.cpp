#include "RTInterface.h"
#include "Defs.h"
#include "RTVTable.h"
#include "NomInterface.h"
#include "CompileHelpers.h"
#include "RTSignature.h"
#include "IMT.h"
#include "RTTypeHead.h"
#include "PWInterface.h"
#include "PWSuperInstance.h"
#include "PWSignature.h"
#include "PWCastFunction.h"
#include "PWCheckReturnValueFunction.h"

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
					NLLVMPointer(SuperInstanceEntryType()),				//superclasses
					NLLVMPointer(SuperInstanceEntryType()),				//superinterfaces
					NLLVMPointer(GetCheckReturnValueFunctionType()),	//method ensure function, untyped version
					POINTERTYPE,										//pointer to type instantiations dictionary
					NLLVMPointer(RTSignature::GetLLVMType())			//signature of lambda method if present, otherwise null
					});
			}
			return rtit;
		}

		llvm::Constant* RTInterface::CreateConstant(const NomInterface* irptr, RTInterfaceFlags flags, llvm::Constant* typeArgCount, llvm::Constant* superClassCount, llvm::Constant* superInterfaceCount, llvm::Constant* superTypeEntries, llvm::Constant* checkReturnValueFunction, llvm::Constant* instantiationDictionary, llvm::Constant* signature)
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
				(signature == nullptr ? ConstantPointerNull::get(RTSignature::GetLLVMType()->getPointerTo()) : signature));
		}

		llvm::Value* RTInterface::GenerateReadSignature(NomBuilder& builder, llvm::Value* descriptorPtr)
		{
			return PWInterface(descriptorPtr).ReadSignature(builder);
		}
		llvm::Value* RTInterface::GenerateReadReturnValueCheckFunction(NomBuilder& builder, llvm::Value* descriptorPtr)
		{
			return PWInterface(descriptorPtr).ReadReturnValueCheckFunction(builder);
		}
		llvm::Value* RTInterface::GenerateReadSuperInstanceCount(NomBuilder& builder, llvm::Value* descriptorPtr)
		{
			return PWInterface(descriptorPtr).ReadSuperInstanceCount(builder);
		}
		llvm::Value* RTInterface::GenerateReadSuperClassCount(NomBuilder& builder, llvm::Value* descriptorPtr)
		{
			return PWInterface(descriptorPtr).ReadSuperClassCount(builder);
		}
		llvm::Value* RTInterface::GenerateReadSuperInterfaceCount(NomBuilder& builder, llvm::Value* descriptorPtr)
		{
			return PWInterface(descriptorPtr).ReadSuperInterfaceCount(builder);
		}
		llvm::Value* RTInterface::GenerateReadSuperInstances(NomBuilder& builder, llvm::Value* descriptorPtr)
		{
			return PWInterface(descriptorPtr).ReadSuperInstances(builder);
		}
		llvm::Value* RTInterface::GenerateReadSuperInterfaces(NomBuilder& builder, llvm::Value* descriptorPtr)
		{
			return PWInterface(descriptorPtr).ReadSuperInterfaces(builder);
		}
		llvm::Value* RTInterface::GenerateReadTypeArgCount(NomBuilder& builder, llvm::Value* descriptorPtr)
		{
			return PWInterface(descriptorPtr).ReadTypeArgCount(builder);
		}
		llvm::Value* RTInterface::GenerateReadFlags(NomBuilder& builder, llvm::Value* descriptorPtr)
		{
			return PWInterface(descriptorPtr).ReadFlags(builder);
		}
		llvm::Value* RTInterface::GenerateReadIRPtr(NomBuilder& builder, llvm::Value* descriptorPtr)
		{
			return PWInterface(descriptorPtr).ReadIRPtr(builder);
		}
	}
}
