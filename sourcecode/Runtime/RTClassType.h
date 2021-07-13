#pragma once
#include "RTTypeHead.h"
#include "RTClass.h"
#include "RTConcreteType.h"
#include "llvm/IR/DerivedTypes.h"
#include "AvailableExternally.h"

namespace Nom
{
	namespace Runtime
	{
		class RTClass;
		class NomClassType;
		class RTClassType;
		enum class RTClassTypeFields : unsigned char { TypeArgs=0, Head = 1, Class = 2 };
		class RTClassType : public ARTRep<RTClassType, RTClassTypeFields>, public AvailableExternally<llvm::Function>
		{
		public:
			static RTClassType& Instance() { static RTClassType rtct((const char*)nullptr); return rtct; }
			static llvm::StructType *GetLLVMType();
			static llvm::StructType * GetLLVMType(size_t size);
			static llvm::Constant *GetConstant(llvm::Module &mod, const NomClassType *cls);
			static uint64_t HeadOffset();
			static uint64_t ClassOffset();
			static uint64_t ArgumentsOffset();
			RTTypeHead GetHead() const { return RTTypeHead(Entry(HeadOffset())); }
			RTClass GetClass() const { return RTClass(Entry(ClassOffset())); }
			RTTypeHead Arguments() const  { return RTTypeHead(Entry(ArgumentsOffset())); }
			static llvm::Value *FromHead(NomBuilder &builder, llvm::Value *head);
			RTClassType(const char * entry) : ARTRep<RTClassType, RTClassTypeFields>(entry)
			{
			}
			RTClassType(const void * entry) : ARTRep<RTClassType, RTClassTypeFields>(entry)
			{
			}

			static llvm::Value* GenerateReadClassDescriptorLink(NomBuilder& builder, llvm::Value* type);
			static llvm::Value* GetTypeArgumentsPtr(NomBuilder& builder, llvm::Value* type);

			static llvm::FunctionType* GetInstantiateClassTypeFunctionType();

			// Inherited via AvailableExternally
			virtual llvm::Function* createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const override;
			virtual llvm::Function* findLLVMElement(llvm::Module& mod) const override;
		};
	}
}
