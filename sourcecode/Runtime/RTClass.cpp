#include "RTClass.h"
#include "ObjectHeader.h"
#include "RTInterfaceTableEntry.h"
#include "NomJIT.h"
#include "NomClass.h"
#include "RTDictionary.h"
#include "CompileHelpers.h"
#include "RTVTable.h"
#include "RTInterface.h"
#include "IMT.h"
#include "RTCompileConfig.h"
#include "PWClass.h"
#include "PWSuperInstance.h"

using namespace llvm;
using namespace std;
namespace Nom
{
	namespace Runtime
	{
		llvm::StructType* RTClass::GetLLVMType()
		{
			static StructType* rtct = StructType::create(LLVMCONTEXT, "RT_NOM_ClassDescriptorType");
			static bool once = true;
			if (once)
			{
				once = false;
				rtct->setBody({ RTVTable::GetLLVMType(),		//vtable
								RTInterface::GetLLVMType(),		//interface stuff: supertypes, etc.
								numtype(size_t)					//field count
					});
			}
			return rtct;
		}

		/// <summary>
		/// 
		/// </summary>
		/// <param name="offset">offset relative to 256 bit alignment (with min 64 bit alignment) in bytes (i.e. 0..32:8)</param>
		/// <param name="methodTableType"></param>
		/// <param name="ddtype">type of dispatcher dictionary entry struct</param>
		/// <param name="stetype">type of super type entry table</param>
		/// <returns></returns>
		llvm::StructType* RTClass::GetConstantType(size_t offset, llvm::Type* methodTableType, llvm::StructType* ddtype, llvm::StructType *stetype)
		{
			auto baseOffset = NomJIT::Instance().getDataLayout().getTypeAllocSize(methodTableType) % 64;
			if (baseOffset == offset)
			{
				return StructType::get(LLVMCONTEXT, { methodTableType, GetLLVMType(), ddtype, stetype }, true);
			}
			else
			{
				if (baseOffset > offset)
				{
					offset += 64;
				}
				offset -= baseOffset;
				return StructType::get(LLVMCONTEXT, { arrtype(inttype(64), offset / 8) , methodTableType, GetLLVMType(), ddtype, stetype }, true);
			}
		}

		llvm::Constant* RTClass::CreateConstant(llvm::GlobalVariable* gvar, llvm::StructType* gvartype, const NomInterface* irptr, llvm::Function* fieldRead, llvm::Function* fieldWrite, llvm::Constant* dynamicDispatcherTable, llvm::ConstantInt* fieldCount, llvm::ConstantInt* typeArgCount, llvm::ConstantInt* superClassCount, llvm::ConstantInt* superInterfaceCount, llvm::Constant* superTypeEntries, llvm::Constant* methodTable, llvm::Constant* checkReturnValueFunction, llvm::Constant* interfaceMethodTable, llvm::Constant* signature, llvm::Constant* castFunction)
		{
			gvar->setAlignment(Align(256));
			if (gvartype->getNumElements() == 4)
			{
				auto ddarr = makealloca(Constant*, IMTsize);
				for (decltype(IMTsize) i = 0; i < IMTsize; i++)
				{
					ddarr[i] = ConstantExpr::getPointerCast(ConstantExpr::getGetElementPtr(gvartype, gvar, ArrayRef<llvm::Constant*>({ MakeInt32(0), MakeInt32(gvartype->getNumElements() - 2), MakeInt32(i) })), GetDynamicDispatchListEntryType()->getPointerTo());
				}
				auto dynamicDispatcherTableReferences = ConstantArray::get(arrtype(GetDynamicDispatchListEntryType()->getPointerTo(), IMTsize), ArrayRef<Constant*>(ddarr, IMTsize));
				auto ptrToClassInstantiations = ConstantExpr::getGetElementPtr(gvartype, gvar, ArrayRef<Constant*>({MakeInt32(0), MakeInt32(gvartype->getNumElements() - 1), MakeInt32(0), MakeInt32(0) }));
				gvar->setInitializer(llvm::ConstantStruct::get(gvartype, methodTable, llvm::ConstantStruct::get(GetLLVMType(), RTVTable::CreateConstant(RTDescriptorKind::Class, interfaceMethodTable, dynamicDispatcherTableReferences, fieldRead, fieldWrite, MakeInt32(irptr->GetHasRawInvoke()?1:0)), RTInterface::CreateConstant(irptr, RTInterfaceFlags::None, typeArgCount, superClassCount, superInterfaceCount, ptrToClassInstantiations, checkReturnValueFunction, GetLLVMPointer(&irptr->runtimeInstantiations), signature, castFunction), fieldCount), dynamicDispatcherTable, superTypeEntries));
			}
			else
			{
				auto arrsize = gvartype->getElementType(0)->getArrayNumElements();
				auto fillarr = makealloca(Constant*, arrsize);
				for (decltype(arrsize) i = 0; i < arrsize; i++)
				{
					fillarr[i] = MakeUInt(64, 0);
				}
				auto ddarr = makealloca(Constant*, IMTsize);
				for (decltype(IMTsize) i = 0; i < IMTsize; i++)
				{
					ddarr[i] = ConstantExpr::getPointerCast(ConstantExpr::getGetElementPtr(gvartype, gvar, ArrayRef<llvm::Constant*>({ MakeInt32(0), MakeInt32(gvartype->getNumElements() - 2), MakeInt32(i) })), GetDynamicDispatchListEntryType()->getPointerTo());
				}
				auto dynamicDispatcherTableReferences = ConstantArray::get(arrtype(GetDynamicDispatchListEntryType()->getPointerTo(), IMTsize), ArrayRef<Constant*>(ddarr, IMTsize));

				auto ptrToClassInstantiations = ConstantExpr::getGetElementPtr(gvartype, gvar, ArrayRef<Constant*>({ MakeInt32(0), MakeInt32(gvartype->getNumElements() - 1), MakeInt32(0), MakeInt32(0) }));
				gvar->setInitializer(llvm::ConstantStruct::get(gvartype, ConstantArray::get(arrtype(inttype(64), arrsize), ArrayRef<Constant*>(fillarr, arrsize)), methodTable, llvm::ConstantStruct::get(GetLLVMType(), RTVTable::CreateConstant(RTDescriptorKind::Class, interfaceMethodTable, dynamicDispatcherTableReferences, fieldRead, fieldWrite, MakeInt32(irptr->GetHasRawInvoke()?1:0)), RTInterface::CreateConstant(irptr, RTInterfaceFlags::None, typeArgCount, superClassCount, superInterfaceCount, ptrToClassInstantiations, checkReturnValueFunction, GetLLVMPointer(&irptr->runtimeInstantiations), signature, castFunction), fieldCount), dynamicDispatcherTable, superTypeEntries));
			}
			return ConstantExpr::getGetElementPtr(gvartype, gvar, ArrayRef<llvm::Constant*>({ MakeInt32(0), MakeInt32(gvartype->getNumElements() - 3) }));
		}
		llvm::Constant* RTClass::FindConstant(llvm::Module& mod, const StringRef name)
		{
			auto cnst = mod.getGlobalVariable(name);
			if (cnst == nullptr)
			{
				return nullptr;
			}
			return ConstantExpr::getGetElementPtr(cnst->getValueType(), cnst, ArrayRef<llvm::Constant*>({ MakeInt32(0), MakeInt32(cnst->getValueType()->getStructNumElements() - 3) }));
		}

		llvm::Value* RTClass::GenerateReadFieldCount(NomBuilder& builder, llvm::Value* descriptorPtr)
		{
			return PWClass(descriptorPtr).ReadFieldCount(builder);
		}
		llvm::Value* RTClass::GenerateReadSuperInstanceCount(NomBuilder& builder, llvm::Value* descriptorPtr)
		{
			return PWClass(descriptorPtr).GetInterface(builder).ReadSuperInstanceCount(builder);
		}
		llvm::Value* RTClass::GenerateReadSuperInstances(NomBuilder& builder, llvm::Value* descriptorPtr)
		{
			return PWClass(descriptorPtr).GetInterface(builder).ReadSuperInstances(builder);
		}
		llvm::Constant* RTClass::GetInterfaceReference(llvm::Constant* clsVtablePtr)
		{
			return ConstantExpr::getGetElementPtr(GetLLVMType(), ConstantExpr::getPointerCast(clsVtablePtr, GetLLVMPointerType()), llvm::ArrayRef<Constant*>({ MakeInt32(0), MakeInt32(RTClassFields::RTInterface) }));
		}
		llvm::Value* RTClass::GetInterfaceReference(NomBuilder &builder, llvm::Value* clsVtablePtr)
		{
			return PWClass(clsVtablePtr).GetInterface(builder);
		}
		llvm::Value* RTClass::GenerateReadTypeArgCount(NomBuilder& builder, llvm::Value* descriptorPtr)
		{
			return PWClass(descriptorPtr).GetInterface(builder).ReadTypeArgCount(builder);
		}
	}
}
