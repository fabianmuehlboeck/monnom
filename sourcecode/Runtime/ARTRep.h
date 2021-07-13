#pragma once
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/DataLayout.h"
#include "NomJITLight.h"
#include "Context.h"
#include "Defs.h"

namespace Nom
{
	namespace Runtime
	{
		template<class C, typename Flds>
		class ARTRep
		{
		private:
			const char * entry;
			static const intptr_t offset = 0;
		public:
			//enum class Fields : unsigned char;

			using Ptr = ARTRep<C, Flds>;

			static const llvm::StructLayout *GetLLVMLayout();
			static llvm::PointerType* GetLLVMPointerType();
			static llvm::ArrayType *GetLLVMArrayType();
			static llvm::ArrayType *GetLLVMArrayType(size_t count);
			void * Entry(uint64_t fieldoffset = 0) const;
			void * Entry(const Flds &field) const;
			ARTRep(const char * const entry/*, intptr_t offset = 0*/);
			ARTRep(const void * const entry/*, intptr_t offset = 0*/);
			~ARTRep() {}

			static size_t SizeOf()
			{
				static const size_t size = (size_t)GetLLVMLayout()->getSizeInBytes(); return size;
			}
			//C& operator++()
			//{
			//	offset++;
			//	return *((C*)this);
			//}
			//C operator++(int)
			//{
			//	C tmp(*((C*)this)); // copy
			//	operator++(); // pre-increment
			//	return tmp;   // return old value
			//}
			//C& operator--()
			//{
			//	offset--;
			//	return *((C*)this);
			//}
			//C operator--(int)
			//{
			//	C tmp(*((C*)this)); // copy
			//	operator--(); // pre-increment
			//	return tmp;   // return old value
			//}
			//void MoveEntry(void * entry, intptr_t offset = 0)
			//{
			//	this->entry = entry;
			//	//this->offset = offset;
			//}

			C& operator=(const C &arg)
			{
				this->entry = arg.entry;
				//this->offset = arg.offset;
				return (C&)(*this);
			}
			//C &AsC()
			//{
			//	return (C&)(*this);
			//}
			//const C &AsC() const
			//{
			//	return (const C&)(*this);
			//}
		};

		template<class C, typename Flds>
		const llvm::StructLayout* ARTRep<C, Flds>::GetLLVMLayout()
		{
			static const llvm::StructLayout* layout = GetNomJITDataLayout().getStructLayout(C::GetLLVMType()); return layout;
		}
		template<class C, typename Flds>
		llvm::ArrayType* ARTRep<C, Flds>::GetLLVMArrayType()
		{
			static llvm::ArrayType* type = llvm::ArrayType::get(C::GetLLVMType(), 0); return type;
		}
		template<class C, typename Flds>
		llvm::PointerType* ARTRep<C, Flds>::GetLLVMPointerType()
		{
			return C::GetLLVMType()->getPointerTo();
		}
		template<class C, typename Flds>
		llvm::ArrayType* ARTRep<C, Flds>::GetLLVMArrayType(size_t count)
		{
			return llvm::ArrayType::get(C::GetLLVMType(), count);;
		}
		template<class C, typename Flds>
		void* ARTRep<C, Flds>::Entry(uint64_t fieldoffset) const {
			if (entry == nullptr)
			{
				return nullptr;
			}
			if (offset == 0) { return (void*)(entry + fieldoffset); }
			else { return (void*)((entry + GetNomJITDataLayout().getIndexedOffsetInType(GetLLVMArrayType(), llvm::ArrayRef<llvm::Value*>({ llvm::ConstantInt::get(llvm::IntegerType::get(LLVMCONTEXT, bitsin(intptr_t)), offset, true) }))) + fieldoffset); }
		}
		template<class C, typename Flds>
		void* ARTRep<C, Flds>::Entry(const Flds & field) const
		{
			if (entry == nullptr)
			{
				return nullptr;
			}
			return (void*)(entry + GetLLVMLayout()->getElementOffset((unsigned char)field));
		}
		template<class C, typename Flds>
		ARTRep<C, Flds>::ARTRep(const char* const entry/*, intptr_t offset*/) : entry(entry)/*, offset(offset)*/
		{
		}
		template<class C, typename Flds>
		ARTRep<C, Flds>::ARTRep(const void* const entry/*, intptr_t offset*/) : entry((const char* const)entry)/*, offset(offset)*/
		{
		}
	}
}

