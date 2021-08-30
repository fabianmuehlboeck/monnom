#include "RTClass.h"
#include "ObjectHeader.h"
#include "RTInterfaceTableEntry.h"
#include "NomJIT.h"
#include "NomClass.h"
#include "RTDictionary.h"
#include "CompileHelpers.h"
#include "RTVTable.h"
#include "RTInterface.h"

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
				rtct->setBody({ RTInterface::GetLLVMType(),		//interface table, flags, nom IR link, type argument count, supertypes
					POINTERTYPE,								//field lookup
					POINTERTYPE,								//field store
					POINTERTYPE,								//dispatcher lookup
					numtype(size_t)								//field count
					});
			}
			return rtct;
		}

		/// <summary>
		/// 
		/// </summary>
		/// <param name="offset">offset relative to 128 bit alignment (with min 64 bit alignment) in bytes (i.e. 0 or 8)</param>
		/// <param name="methodTableType"></param>
		/// <returns></returns>
		llvm::StructType* RTClass::GetConstantType(size_t offset, llvm::Type * methodTableType)
		{
			auto baseOffset = NomJIT::Instance().getDataLayout().getTypeAllocSize(methodTableType)%16;
			if (baseOffset == offset)
			{
				return StructType::get(LLVMCONTEXT, { methodTableType, GetLLVMType() }, true);
			}
			else
			{
				return StructType::get(LLVMCONTEXT, { inttype(64), methodTableType, GetLLVMType() }, true);
			}
		}

		llvm::Constant* RTClass::CreateConstant(llvm::GlobalVariable *gvar, llvm::StructType *gvartype, const NomInterface *irptr, llvm::Function* fieldRead, llvm::Function* fieldWrite, llvm::Function* lookupDispatcher, llvm::ConstantInt* fieldCount, llvm::ConstantInt* typeArgCount, llvm::ConstantInt* superTypesCount, llvm::Constant* superTypeEntries, llvm::Constant* methodTable, llvm::Constant* checkReturnValueFunction, llvm::Constant* methodEnsureFunction, llvm::Constant* interfaceTable, llvm::Constant* signature)
		{
			gvar->setAlignment(Align(128));
			if (gvartype->getNumElements() == 2)
			{
				gvar->setInitializer(llvm::ConstantStruct::get(gvartype, methodTable, llvm::ConstantStruct::get(GetLLVMType(), RTInterface::CreateConstant(irptr, RTInterfaceFlags::None, typeArgCount, superTypesCount, ConstantExpr::getGetElementPtr(((PointerType*)superTypeEntries->getType())->getElementType(), superTypeEntries, ArrayRef<Constant*>({ MakeInt32(0), MakeInt32(0) })), interfaceTable, checkReturnValueFunction, methodEnsureFunction, GetLLVMPointer(&irptr->runtimeInstantiations), signature), ConstantExpr::getPointerCast(fieldRead, POINTERTYPE), ConstantExpr::getPointerCast(fieldWrite, POINTERTYPE), ConstantExpr::getPointerCast(lookupDispatcher, POINTERTYPE), fieldCount)));
			}
			else
			{
				gvar->setInitializer(llvm::ConstantStruct::get(gvartype, MakeUInt(64,0), methodTable, llvm::ConstantStruct::get(GetLLVMType(), RTInterface::CreateConstant(irptr, RTInterfaceFlags::None, typeArgCount, superTypesCount, ConstantExpr::getGetElementPtr(((PointerType*)superTypeEntries->getType())->getElementType(), superTypeEntries, ArrayRef<Constant*>({ MakeInt32(0), MakeInt32(0) })), interfaceTable, checkReturnValueFunction, methodEnsureFunction, GetLLVMPointer(&irptr->runtimeInstantiations), signature), ConstantExpr::getPointerCast(fieldRead, POINTERTYPE), ConstantExpr::getPointerCast(fieldWrite, POINTERTYPE), ConstantExpr::getPointerCast(lookupDispatcher, POINTERTYPE), fieldCount)));
			}
			return ConstantExpr::getGetElementPtr(gvartype, gvar, ArrayRef<llvm::Constant*>({ MakeInt32(0), MakeInt32(gvartype->getNumElements()-1) }));
		}
		llvm::Constant* RTClass::FindConstant(llvm::Module& mod, const StringRef name)
		{
			auto cnst = mod.getGlobalVariable(name);
			if (cnst == nullptr)
			{
				return nullptr;
			}
			return ConstantExpr::getGetElementPtr(cnst->getValueType(), cnst, ArrayRef<llvm::Constant*>({ MakeInt32(0), MakeInt32(cnst->getValueType()->getStructNumElements()-1) }));
		}

		llvm::Value* RTClass::GenerateReadKind(NomBuilder& builder, llvm::Value* descriptorPtr)
		{
			return RTInterface::GenerateReadKind(builder, builder->CreateGEP(descriptorPtr, { MakeInt32(0), MakeInt32(RTClassFields::RTInterface) }));
		}

		llvm::Value* RTClass::GenerateReadNomIRLink(NomBuilder& builder, llvm::Value* descriptorPtr)
		{
			return RTInterface::GenerateReadNomIRLink(builder, builder->CreateGEP(descriptorPtr, { MakeInt32(0), MakeInt32(RTClassFields::RTInterface) }));
		}

		llvm::Value* RTClass::GenerateReadFieldLookup(NomBuilder& builder, llvm::Value* descriptorPtr)
		{
			return builder->CreatePointerCast(MakeLoad(builder, builder->CreatePointerCast(descriptorPtr, GetLLVMPointerType()), MakeInt32(RTClassFields::FieldLookup)), NomClass::GetDynamicFieldLookupType()->getPointerTo());
		}

		llvm::Value* RTClass::GenerateReadFieldStore(NomBuilder& builder, llvm::Value* descriptorPtr)
		{
			return builder->CreatePointerCast(MakeLoad(builder, builder->CreatePointerCast(descriptorPtr, GetLLVMPointerType()), MakeInt32(RTClassFields::FieldStore)), NomClass::GetDynamicFieldStoreType()->getPointerTo());
		}

		llvm::Value* RTClass::GenerateReadDispatcherLookup(NomBuilder& builder, llvm::Value* descriptorPtr)
		{
			return builder->CreatePointerCast(MakeLoad(builder, builder->CreatePointerCast(descriptorPtr, GetLLVMPointerType()), MakeInt32(RTClassFields::DispatcherLookup)), NomClass::GetDynamicDispatcherLookupType()->getPointerTo());
		}



		llvm::Value* RTClass::GenerateReadFieldCount(NomBuilder& builder, llvm::Value* descriptorPtr)
		{
			return MakeInvariantLoad(builder, builder->CreatePointerCast(descriptorPtr, GetLLVMType()->getPointerTo()), MakeInt32(RTClassFields::FieldCount),"FieldCount", AtomicOrdering::NotAtomic);
		}
		llvm::Value* RTClass::GenerateReadSuperInstanceCount(NomBuilder& builder, llvm::Value* descriptorPtr)
		{
			return RTInterface::GenerateReadSuperInstanceCount(builder, builder->CreateGEP(descriptorPtr, { MakeInt32(0), MakeInt32(RTClassFields::RTInterface) }));
			//MakeLoad(builder, builder->CreatePointerCast(descriptorPtr, GetLLVMType()->getPointerTo()), MakeInt32(RTClassFields::SuperTypesCount));
		}
		llvm::Value* RTClass::GenerateReadSuperInstances(NomBuilder& builder, llvm::Value* descriptorPtr)
		{
			return RTInterface::GenerateReadSuperInstances(builder, builder->CreateGEP(descriptorPtr, { MakeInt32(0), MakeInt32(RTClassFields::RTInterface) }));
			//return MakeLoad(builder, builder->CreatePointerCast(descriptorPtr, GetLLVMType()->getPointerTo()), MakeInt32(RTClassFields::SuperTypes));
		}
		llvm::Value* RTClass::GenerateReadMethodTableEntry(NomBuilder& builder, llvm::Value* vtablePtr, llvm::Value* offset)
		{
			return RTInterface::GenerateReadMethodTableEntry(builder, builder->CreateGEP(builder->CreatePointerCast(vtablePtr, GetLLVMPointerType()), { MakeInt32(0), MakeInt32(RTClassFields::RTInterface) }), offset);
		}
		llvm::Value* RTClass::GenerateReadTypeArgCount(NomBuilder& builder, llvm::Value* descriptorPtr)
		{
			return RTInterface::GenerateReadTypeArgCount(builder, builder->CreateGEP(descriptorPtr, { MakeInt32(0), MakeInt32(RTClassFields::RTInterface) }));
			//return MakeLoad(builder, builder->CreateGEP(builder->CreatePointerCast(descriptorPtr, GetLLVMType()->getPointerTo()), { MakeInt32(0), MakeInt32(RTClassFields::TypeArgCount) }));;
		}
		void RTClass::GenerateInitialization(NomBuilder& builder, llvm::Value* clsptr, llvm::Value* /*vt_ifcoffset*/ vt_imtptr, llvm::Value* vt_kind, llvm::Value* vt_irdesc, llvm::Value* ifc_flags, llvm::Value* ifc_targcount, llvm::Value* ifc_supercount, llvm::Value* ifc_superentries, llvm::Value* fieldlookup, llvm::Value* fieldstore, llvm::Value* displookup, llvm::Value* fieldcount)
		{
			RTInterface::GenerateInitialization(builder, clsptr, /*vt_ifcoffset*/ vt_imtptr, vt_kind, vt_irdesc, ifc_flags, ifc_targcount, ifc_supercount, ifc_superentries);
			llvm::Value* selfptr = builder->CreatePointerCast(clsptr, GetLLVMType()->getPointerTo());
			MakeInvariantStore(builder, fieldlookup, selfptr, MakeInt32(RTClassFields::FieldLookup));
			MakeInvariantStore(builder, fieldstore, selfptr, MakeInt32(RTClassFields::FieldStore));
			MakeInvariantStore(builder, displookup, selfptr, MakeInt32(RTClassFields::DispatcherLookup));
			MakeInvariantStore(builder, fieldcount, selfptr, MakeInt32(RTClassFields::FieldCount));
		}
		//llvm::Value* RTClass::GenerateReadNumInterfaceTableEntries(NomBuilder& builder, llvm::Value* descriptorPtr)
		//{
		//	return RTVTable::GenerateReadNumInterfaceTableEntries(builder, builder->CreateGEP(descriptorPtr, { MakeInt32(0), MakeInt32(RTClassFields::RTVTable) }));
		//}
		//llvm::Value* RTClass::GenerateReadFirstInterfaceTableEntryPointer(NomBuilder& builder, llvm::Value* descriptorPtr)
		//{
		//	return RTVTable::GenerateReadFirstInterfaceTableEntryPointer(builder, builder->CreateGEP(descriptorPtr, { MakeInt32(0), MakeInt32(RTClassFields::RTVTable) }));
		//}

		//llvm::Value* RTClass::GenerateReadMethodTable(NomBuilder& builder, llvm::Value* descriptorPtr, llvm::Value* offset)
		//{
		//	if (offset == nullptr)
		//	{
		//		offset = ConstantInt::get(numtype(int32_t), 0);
		//	}
		//	return MakeLoad(builder, builder->CreateGEP(builder->CreatePointerCast(descriptorPtr, GetLLVMType()->getPointerTo()), { MakeInt32(0), MakeInt32(RTClassFields::MethodTable), offset }));;
		//}

		

		//static const llvm::StructLayout *GetLLVMLayout();

		/*uint64_t RTClass::NameOffset()
		{
			static const uint64_t offset = GetLLVMLayout()->getElementOffset((unsigned char)RTClassFields::NomNamedLink); return offset;
		}
		uint64_t RTClass::MethodTableOffset()
		{
			static const uint64_t offset = GetLLVMLayout()->getElementOffset((unsigned char)RTClassFields::MethodTable); return offset;
		}
		uint64_t RTClass::ArgCountOffset()
		{
			static const uint64_t offset = GetLLVMLayout()->getElementOffset((unsigned char)RTClassFields::ArgCount); return offset;
		}
		uint64_t RTClass::FieldCountOffset()
		{
			static const uint64_t offset = GetLLVMLayout()->getElementOffset((unsigned char)RTClassFields::FieldCount); return offset;
		}
		uint64_t RTClass::SuperTypesCountOffset()
		{
			static const uint64_t offset = GetLLVMLayout()->getElementOffset((unsigned char)RTClassFields::SuperTypesCount); return offset;
		}
		uint64_t RTClass::SuperTypesOffset()
		{
			static const uint64_t offset = GetLLVMLayout()->getElementOffset((unsigned char)RTClassFields::SuperTypes); return offset;
		}
		uint64_t RTClass::InterfaceTableSizeOffset()
		{
			static const uint64_t offset = GetLLVMLayout()->getElementOffset((unsigned char)RTClassFields::InterfaceTableSize); return offset;
		}
		uint64_t RTClass::InterfaceTableOffset()
		{
			static const uint64_t offset = GetLLVMLayout()->getElementOffset((unsigned char)RTClassFields::InterfaceTable); return offset;
		}
		uint64_t RTClass::DispatchDictOffset()
		{
			static const uint64_t offset = GetLLVMLayout()->getElementOffset((unsigned char)RTClassFields::Dictionary); return offset;
		}*/
		//const char * RTClass::Entry() const
		//{
		//	static llvm::ArrayType * arrtype = llvm::ArrayType::get(GetLLVMType(), 0);
		//	return entry + NomJIT::Instance().getDataLayout().getIndexedOffsetInType(arrtype, llvm::ArrayRef<llvm::Value *>(llvm::ConstantInt::get(INTTYPE, offset, false)));
		//}
		//inline const llvm::StructLayout * RTClass::GetLLVMLayout()
		//{
		//	static const llvm::StructLayout *layout = NomJIT::Instance().getDataLayout().getStructLayout(GetLLVMType()); return layout;
		//}
		//ObjectHeader RTClass::Name() const
		//{
		//	return (ObjectHeader(Entry(NameOffset())));
		//}



		llvm::Value* RTClass::CreateCheckIsExpando(NomBuilder& builder, llvm::Module& mod, llvm::Value* cdesc)
		{
			return ConstantInt::get(BOOLTYPE, 0);
		}
		//size_t RTClass::getSize() const
		//{
		//	return RTClass::SizeOf() + NomJIT::Instance().getDataLayout().getIndexedOffsetInType(REFTYPE, { llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(LLVMCONTEXT), FieldCount()) });
		//}
		//RTClass::RTClass(const char * entry, intptr_t offset, ObjectHeader namestr, void ** methodTable) : entry(entry), offset(offset)
		//{
		//	
		//}
	}
}
