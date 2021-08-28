#pragma once

#include <map>
#include <unordered_map>
#include "Defs.h"
#include "NomString.h"
#include "SmallMap.h"
#include "llvm/ADT/TinyPtrVector.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/IR/Module.h"
#include "llvm/ADT/StringRef.h"
#include "ARTRep.h"
#include "RTStruct.h"
#include "NomBuilder.h"

namespace Nom
{
	namespace Runtime
	{
		//using DispatchDict = std::unordered_map < NomStringRef, std::unordered_map<std::tuple<int, int>, void*>, NomStringHash, NomStringEquality >;
		class RTClassTypeScheme;
		class RTInterfaceTableEntry;
		class ObjectHeader;
		class RTClass;

		//enum class ClassDictionaryKind : unsigned char { PartialApplication = 0, Lambda = 1, StructuralObject = 2, NominalObject = 3 };
		enum class RTClassFields : unsigned char { RTInterface=0, FieldLookup=1, FieldStore=2, DispatcherLookup=3, /* Dictionary = 1,*/ FieldCount = 4/*2*/ /*, TypeArgCount = 3, SuperTypesCount = 4, SuperTypes = 5, MethodTable=6*/ };
		//enum class RTClassFields : unsigned char { /*Name = 0, */ MethodTable = 0, ArgCount = 1, FieldCount = 2, ExpandoReader = 3, SuperTypesCount = 4, SuperTypes = 5, InterfaceTableSize = 6, InterfaceTable = 7, Dictionary = 8, NomNamedLink = 9 };
		class RTClass : public ARTRep<RTClass, RTClassFields>
		{
		private:
			//const char* entry;
			//uint64_t offset;
			//const char* Entry() const;
		public:

			static llvm::StructType* GetLLVMType();
			static llvm::StructType* GetConstantType(llvm::Type* interfaceTableType, llvm::Type* methodTableType);
			//static llvm::StructType* GetLLVMType(int methodCount);
			static llvm::Constant* CreateConstant(/*llvm::Module &mod, GlobalValue::LinkageTypes linkage, const Twine &name*/ llvm::GlobalVariable* gvar, llvm::StructType* gvartype, const NomInterface* irptr, llvm::Function* fieldRead, llvm::Function* fieldWrite, llvm::Function* lookupDispatcher, /*llvm::Constant* dictPointer,*/ llvm::ConstantInt* fieldCount, llvm::ConstantInt* typeArgCount, llvm::ConstantInt* superTypesCount, llvm::Constant* superTypeEntries, llvm::Constant* methodTable, llvm::Constant* checkReturnValueFunction, llvm::Constant* methodEnsureFunction, llvm::Constant* interfaceTable, llvm::Constant* signature);
			static llvm::Constant* FindConstant(llvm::Module& mod, const llvm::StringRef name);
			//static const llvm::StructLayout *GetLLVMLayout();
			//static uint64_t NameOffset();
			//static uint64_t MethodTableOffset();
			//static uint64_t ArgCountOffset();
			//static uint64_t FieldCountOffset();
			//static uint64_t SuperTypesCountOffset();
			//static uint64_t SuperTypesOffset();
			//static uint64_t InterfaceTableSizeOffset();
			//static uint64_t InterfaceTableOffset();
			//static uint64_t DispatchDictOffset();

			//ObjectHeader Name() const;
			//void*** MethodTable() const { return (void***)(Entry(MethodTableOffset())); }
			//const int& ArgCount() const { return *((int*)(Entry(ArgCountOffset()))); }
			//const int& FieldCount() const { return *((int*)(Entry(FieldCountOffset()))); }
			//size_t& SuperTypesCount() const { return *((size_t*)(Entry(SuperTypesCountOffset()))); }
			//RTClassTypeScheme*& SuperTypes() const { return *((RTClassTypeScheme * *)(Entry(SuperTypesOffset()))); }
			//size_t& InterfaceTableSize() const { return *((size_t*)(Entry(InterfaceTableSizeOffset()))); }
			//RTInterfaceTableEntry*& InterfaceTable() const { return *((RTInterfaceTableEntry * *)(Entry(InterfaceTableOffset()))); }
			//DispatchDict& DynamicDispatchers() const { return *((DispatchDict*)(Entry(DispatchDictOffset()))); }

			//static llvm::StructType* GetClassDictionaryEntryType();
			//static llvm::Constant* CreateClassDictionaryEntryConstant(ClassDictionaryEntryKind det, bool readonly, Visibility visibility, llvm::Constant* typeOrPartialAppOrDispatcher, int32_t index = 0);

			static llvm::Value* GenerateReadKind(NomBuilder& builder, llvm::Value* descriptorPtr);
			static llvm::Value* GenerateReadNomIRLink(NomBuilder& builder, llvm::Value* descriptorPtr);
			static llvm::Value* GenerateReadFieldLookup(NomBuilder& builder, llvm::Value* descriptorPtr);
			static llvm::Value* GenerateReadFieldStore(NomBuilder& builder, llvm::Value* descriptorPtr);
			static llvm::Value* GenerateReadDispatcherLookup(NomBuilder& builder, llvm::Value* descriptorPtr);
			//static llvm::Value* GenerateReadDescriptorDictionary(NomBuilder& builder, llvm::Value* descriptorPtr);
			static llvm::Value* GenerateReadFieldCount(NomBuilder& builder, llvm::Value* descriptorPtr);
			static llvm::Value* GenerateReadSuperInstanceCount(NomBuilder& builder, llvm::Value* descriptorPtr);
			static llvm::Value* GenerateReadSuperInstances(NomBuilder& builder, llvm::Value* descriptorPtr);
			static llvm::Value* GenerateReadMethodTableEntry(NomBuilder& builder, llvm::Value* vtablePtr, llvm::Value* offset);
			//static llvm::Value* GenerateReadNumInterfaceTableEntries(NomBuilder& builder, llvm::Value* descriptorPtr);
			//static llvm::Value* GenerateReadFirstInterfaceTableEntryPointer(NomBuilder& builder, llvm::Value* descriptorPtr);
			//static llvm::Value* GenerateReadMethodTable(NomBuilder& builder, llvm::Value* descriptorPtr, llvm::Value* offset=nullptr);
			static llvm::Value* GenerateReadTypeArgCount(NomBuilder& builder, llvm::Value* descriptorPtr);

			static void GenerateInitialization(NomBuilder& builder, llvm::Value* clsptr, llvm::Value* /*vt_ifcoffset*/ vt_imtptr, llvm::Value* vt_kind, llvm::Value* vt_irdesc, llvm::Value* ifc_flags, llvm::Value* ifc_targcount, llvm::Value* ifc_supercount, llvm::Value* ifc_superentries, llvm::Value* fieldlookup, llvm::Value* fieldstore, llvm::Value* displookup, llvm::Value* fieldcount);

			//static llvm::Value* CreateDictionaryLoad(NomBuilder& builder, llvm::Module& mod, llvm::Value* thisObj, llvm::Value* cdesc, llvm::ConstantInt* key);
			//static llvm::Value* CreateDictionaryStore(NomBuilder& builder, llvm::Module& mod, llvm::Value* thisObj, llvm::Value* cdesc, llvm::ConstantInt* key, llvm::Value* value);
			static llvm::Value* CreateCheckIsExpando(NomBuilder& builder, llvm::Module& mod, llvm::Value* cdesc);

			RTClass(const char* entry/*, intptr_t offset = 0*/) :ARTRep<RTClass, RTClassFields>(entry/*, offset*/) //: entry(entry), offset(offset)
			{

			}
			RTClass(const void* entry/*, intptr_t offset = 0*/) :ARTRep<RTClass, RTClassFields>(entry/*, offset*/) //: entry((char*)entry), offset(offset)
			{

			}

			//size_t getSize() const;

			~RTClass()
			{

			}
		};

	}
}
