#pragma once
#include <unordered_map>
#include "BoehmAllocator.h"
#include "CompileEnv.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Value.h"
#include "Context.h"
#include "ARTRep.h"
#include "NomInstantiationRef.h"
#include "NomString.h"

namespace Nom
{
	namespace Runtime
	{
		using namespace llvm;
		class RTClass;
		class NomMethod;


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
			static llvm::StructType *GetLLVMType();
			static llvm::StructType* GetLLVMType(int fieldcount, int typeargcount, bool rawInvoke);
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

			static llvm::Value* GenerateSetClassDescriptor(NomBuilder &builder, llvm::Value * ohref, size_t fieldCount, llvm::Value * desc);

			static llvm::Value* CreateDictionaryLoad(NomBuilder& builder, CompileEnv* env, NomValue& receiver, llvm::ConstantInt *key,const llvm::Twine keyName="");

			static void EnsureExternalReadWriteFieldFunctions(llvm::Module* mod);


			static size_t SizeOf()
			{
				static const size_t size = (size_t)GetLLVMLayout()->getSizeInBytes(); return size;
			}


			static llvm::Value *GetDispatchMethodPointer(NomBuilder &builder, CompileEnv* env, RegIndex reg, int lineno, NomInstantiationRef<const NomMethod> method);
			static llvm::Value* GetInterfaceMethodTableFunction(NomBuilder& builder, CompileEnv* env, RegIndex reg, llvm::Constant* index, int lineno);
		};
	}
}