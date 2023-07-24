#pragma once
PUSHDIAGSUPPRESSION
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/DataLayout.h"
POPDIAGSUPPRESSION
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
			static const uintptr_t offset = 0;
		public:

			using Ptr = ARTRep<C, Flds>;

			static const llvm::StructLayout *GetLLVMLayout();
			static llvm::PointerType* GetLLVMPointerType();
			static llvm::ArrayType *GetLLVMArrayType();
			static llvm::ArrayType *GetLLVMArrayType(size_t count);
			void * Entry(uint64_t fieldoffset = 0) const;
			void * Entry(const Flds &field) const;
			ARTRep(const char * const entry);
			ARTRep(const void * const entry);

			static size_t SizeOf()
			{
				static const size_t size = static_cast<size_t>(GetLLVMLayout()->getSizeInBytes()); return size;
			}

			C& operator=(const C &arg)
			{
				this->entry = arg.entry;
				return (C&)(*this);
			}
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
			if (offset == 0) { return reinterpret_cast<void*>(const_cast<char*>(entry) + fieldoffset); }
			else { return reinterpret_cast<void*>((const_cast<char*>(entry) + GetNomJITDataLayout().getIndexedOffsetInType(GetLLVMArrayType(), llvm::ArrayRef<llvm::Value*>({ llvm::ConstantInt::get(llvm::IntegerType::get(LLVMCONTEXT, bitsin(intptr_t)), offset, true) }))) + fieldoffset); }
		}
		template<class C, typename Flds>
		void* ARTRep<C, Flds>::Entry(const Flds & field) const
		{
			if (entry == nullptr)
			{
				return nullptr;
			}
			return reinterpret_cast<void*>(const_cast<char*>(entry) + GetLLVMLayout()->getElementOffset(reinterpret_cast<unsigned char>(field)));
		}
		template<class C, typename Flds>
		ARTRep<C, Flds>::ARTRep(const char* const _entry) : entry(_entry)
		{
		}
		template<class C, typename Flds>
		ARTRep<C, Flds>::ARTRep(const void* const _entry) : entry(reinterpret_cast<const char* const>(_entry))
		{
		}
	}
}

