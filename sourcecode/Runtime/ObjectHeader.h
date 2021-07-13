#pragma once
#include "RTTypes.h"
//#include "RTClassTypeTag.h"
#include <unordered_map>
#include "BoehmAllocator.h"
#include "CompileEnv.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Value.h"
#include "Context.h"
#include "ARTRep.h"
#include "NomInstantiationRef.h"

namespace Nom
{
	namespace Runtime
	{
		using namespace llvm;
		class RTClass;
		class NomMethod;

		//enum class DictionaryEntryType : unsigned char
		//{
		//	Method,
		//	Field
		//};

		struct DictionaryEntry
		{

		};

		using PointerDict = std::unordered_map<NomString *, intptr_t, NomStringHash, NomStringEquality, BoehmAllocator<std::pair<NomString, intptr_t>>>;

		enum class ObjectHeaderFields : unsigned char { TypeArgs = 0, RefValueHeader = 1,  Fields = 2 };

		class ObjectHeader : public ARTRep<ObjectHeader, ObjectHeaderFields>
		{
		private:
			ObjectHeader();
		public:
			//Create instructions that initialize a type descriptor for the given value; uses alloca, so cannot be returned
			//That type is either an RTInstanceType (for objects), an RTClassType (for primitives), or a plain RTTypeHead with kind RTStruct (for structural objects) or RTPartialApp (for partial applications)
			//static llvm::Value* CreateExtractType(NomBuilder& builder, llvm::Module& mod, llvm::Value* value);
			static llvm::StructType *GetLLVMType();
			static llvm::StructType* GetLLVMType(int fieldcount, int typeargcount, bool rawInvoke);
			//static llvm::StructType *GetLLVMType(llvm::Type *clstype, llvm::Type * fieldstype=nullptr, llvm::Type * typeargstype=nullptr);
			static llvm::Constant* GetConstant(llvm::Constant* clsdesc, llvm::Constant* fields=nullptr, llvm::Constant* typeargs=nullptr, llvm::Constant* invokeptr=nullptr);
			static llvm::Constant * GetGlobal(llvm::Module &mod, llvm::GlobalValue::LinkageTypes linkage, llvm::Twine name, llvm::Constant * clsdesc, llvm::Constant * fields=nullptr, llvm::Constant * typeargs=nullptr, bool isConstant = true);

			static llvm::Constant* FindGlobal(llvm::Module& mod, const std::string & name);

			static llvm::Value *GenerateReadTypeArgument(NomBuilder &builder, llvm::Value * objPointer, int32_t argindex);
			static void GenerateWriteTypeArgument(NomBuilder &builder, llvm::Value * objPointer, int32_t argindex, llvm::Value * val);

			static llvm::Value* GeneratePointerToTypeArguments(NomBuilder& builder, llvm::Value * objPointer);

			static llvm::Value *ReadField(NomBuilder &builder, llvm::Value * objPointer, int32_t fieldindex, bool targetHasRawInvoke);
			static llvm::Value* ReadField(NomBuilder& builder, llvm::Value* objPointer, llvm::Value* fieldindex, bool targetHasRawInvoke);
			static void WriteField(NomBuilder &builder, llvm::Value * objPointer, int32_t fieldindex, llvm::Value * val, bool targetHasRawInvoke);

			static void WriteField(NomBuilder& builder, llvm::Value* objPointer, llvm::Value* fieldindex, llvm::Value* val, bool targetHasRawInvoke);

			static llvm::Value* GenerateReadVTablePointer(NomBuilder& builder, llvm::Value* objPointer);

			///Returns pointer to class descriptor, i.e. actual object pointer
			static llvm::Value* GenerateSetClassDescriptor(NomBuilder &builder, llvm::Value * ohref, size_t fieldCount, llvm::Value * desc);
			//static void GenerateSetFieldsRef(NomBuilder &builder, CompileEnv* env, int lineno, llvm::Value * ohref, llvm::Value * fieldsref);

			static llvm::Value* CreateDictionaryLoad(NomBuilder& builder, CompileEnv* env, NomValue& receiver, llvm::ConstantInt *key,const llvm::Twine keyName="");

			static void EnsureExternalReadWriteFieldFunctions(llvm::Module* mod);

			//static const llvm::StructLayout *GetLLVMLayout();
			//static uint64_t ClassDescriptorOffset();
			//static uint64_t FieldsPointerOffset();
			//static uint64_t DictionaryPointerOffset();
			//static uint64_t TypeArgsOffset();
			static size_t SizeOf()
			{
				static const size_t size = (size_t)GetLLVMLayout()->getSizeInBytes(); return size;
			}
			//ObjectHeader(const RTClass cls, const void * const entry/*, intptr_t offset = 0*/) : ARTRep<ObjectHeader, ObjectHeaderFields>(entry/*, offset*/)
			//{
			//	ClassDescriptor(cls);
			//}
			//ObjectHeader(const void * const entry/*, intptr_t offset = 0*/) : ARTRep<ObjectHeader, ObjectHeaderFields>(entry/*, offset*/)
			//{

			//}
			//ObjectHeader(const char * const entry/*, intptr_t offset = 0*/) : ARTRep<ObjectHeader, ObjectHeaderFields>(entry/*, offset*/)
			//{

			//}
			//ObjectHeader(const RTClass* cls, size_t argcount, intptr_t * fields = nullptr)// : RTClassType(cls, sizeof(ObjectHeader))
			//{
			//	new(((ObjectHeaderProto*)this)->rtct) RTClassType(cls, sizeof(ObjectHeader));
			//	if (fields == nullptr)
			//	{
			//		((ObjectHeaderProto*)this)->fields = (intptr_t *)(((RTTypeHead *)(this + 1)) + argcount);
			//	}
			//	else
			//	{
			//		((ObjectHeaderProto*)this)->fields = fields;
			//	}
			//	new (((ObjectHeaderProto*)this)->dict) PointerDict();
			//}
			//const RTClassTypeTag* operator->() const
			//{
			//	return ((const RTClassTypeTag *)((const ObjectHeaderProto*)this)->rtct);
			//}
			//intptr_t * const &GetFields() const
			//{
			//	return ((const ObjectHeaderProto*)this)->fields;
			//}
			//intptr_t * &GetFields() 
			//{
			//	return ((ObjectHeaderProto*)this)->fields;
			//}
			//const RTClass ClassDescriptor() { return RTClass(*((void**)Entry(ClassDescriptorOffset()))); }
			//void ClassDescriptor(const RTClass desc) { *((const void **)(Entry(ClassDescriptorOffset()))) = desc.Entry(); }
			//intptr_t &Fields() { return *((intptr_t*)(Entry(FieldsPointerOffset()))); }
			//intptr_t Fields() const { return *((intptr_t*)(Entry(FieldsPointerOffset()))); }
			//PointerDict * &Dictionary() { return *((PointerDict **)(Entry(DictionaryPointerOffset()))); }


			static llvm::Value *GetDispatchMethodPointer(NomBuilder &builder, CompileEnv* env, RegIndex reg, int lineno, NomInstantiationRef<const NomMethod> method);
			//static llvm::Value *GetInterfaceMethodPointer(NomBuilder &builder, CompileEnv* env, RegIndex reg, int lineno, InterfaceID iface, int offset);
			static llvm::Value* GetInterfaceMethodTableFunction(NomBuilder& builder, CompileEnv* env, RegIndex reg, llvm::Constant* index, int lineno);
		};
	}
}