#pragma once
#include "Defs.h"
#include "TypeOperations.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/IR/Value.h"
#include "NomBuilder.h"

namespace Nom
{
	namespace Runtime
	{
		llvm::CallInst* GenerateFunctionCall(NomBuilder &builder, llvm::Module &mod, llvm::Value* fun, llvm::FunctionType* ftype, llvm::ArrayRef<llvm::Value*>& args, bool fastcall = true);
		llvm::CallInst* GenerateFunctionCall(NomBuilder& builder, llvm::Module& mod, llvm::Function* fun, llvm::ArrayRef<llvm::Value*>& args, bool fastcall);

		llvm::Value* GenerateIsNull(NomBuilder& builder, llvm::Value* val);
		void CreateDummyReturn(NomBuilder& builder, llvm::Function* fun);

		llvm::StoreInst* MakeStore(NomBuilder& builder, llvm::Module& mod, llvm::Value* val, llvm::Value* ptr, llvm::AtomicOrdering ordering = llvm::AtomicOrdering::Unordered);
		llvm::StoreInst* MakeStore(NomBuilder& builder, llvm::Value* val, llvm::Value* ptr, llvm::AtomicOrdering ordering = llvm::AtomicOrdering::Unordered);
		llvm::StoreInst* MakeStore(NomBuilder& builder, llvm::Value* val, llvm::Value* ptr, llvm::ArrayRef<llvm::Value*> indices, llvm::AtomicOrdering ordering = llvm::AtomicOrdering::Unordered);
		llvm::StoreInst* MakeStore(NomBuilder& builder, llvm::Value* val, llvm::Value* ptr, llvm::PointerType* asType, llvm::ArrayRef<llvm::Value*> indices, llvm::AtomicOrdering ordering = llvm::AtomicOrdering::Unordered);
		llvm::StoreInst* MakeStore(NomBuilder& builder, llvm::Value* val, llvm::Value* ptr, llvm::Value* index, llvm::AtomicOrdering ordering = llvm::AtomicOrdering::Unordered);
		llvm::StoreInst* MakeStore(NomBuilder& builder, llvm::Value* val, llvm::Value* ptr, llvm::PointerType* asType, llvm::Value* index, llvm::AtomicOrdering ordering = llvm::AtomicOrdering::Unordered);


		llvm::StoreInst* MakeInvariantStore(NomBuilder& builder, llvm::Module& mod, llvm::Value* val, llvm::Value* ptr, llvm::AtomicOrdering ordering = llvm::AtomicOrdering::Unordered);
		llvm::StoreInst* MakeInvariantStore(NomBuilder& builder, llvm::Value* val, llvm::Value* ptr, llvm::AtomicOrdering ordering = llvm::AtomicOrdering::Unordered);
		llvm::StoreInst* MakeInvariantStore(NomBuilder& builder, llvm::Value* val, llvm::Value* ptr, llvm::ArrayRef<llvm::Value*> indices, llvm::AtomicOrdering ordering = llvm::AtomicOrdering::Unordered);
		llvm::StoreInst* MakeInvariantStore(NomBuilder& builder, llvm::Value* val, llvm::Value* ptr, llvm::PointerType* asType, llvm::ArrayRef<llvm::Value*> indices, llvm::AtomicOrdering ordering = llvm::AtomicOrdering::Unordered);
		llvm::StoreInst* MakeInvariantStore(NomBuilder& builder, llvm::Value* val, llvm::Value* ptr, llvm::Value* index, llvm::AtomicOrdering ordering = llvm::AtomicOrdering::Unordered);
		llvm::StoreInst* MakeInvariantStore(NomBuilder& builder, llvm::Value* val, llvm::Value* ptr, llvm::PointerType* asType, llvm::Value* index, llvm::AtomicOrdering ordering = llvm::AtomicOrdering::Unordered);

		llvm::LoadInst* MakeLoad(NomBuilder& builder, llvm::Module& mod, llvm::Value* ptr, llvm::AtomicOrdering ordering = llvm::AtomicOrdering::Unordered);
		llvm::LoadInst* MakeLoad(NomBuilder& builder, llvm::Value* ptr, llvm::Twine name = "", llvm::AtomicOrdering ordering = llvm::AtomicOrdering::Unordered);
		llvm::LoadInst* MakeLoad(NomBuilder& builder, llvm::Value* ptr, llvm::ArrayRef<llvm::Value*> indices, llvm::Twine name = "", llvm::AtomicOrdering ordering = llvm::AtomicOrdering::Unordered);
		llvm::LoadInst* MakeLoad(NomBuilder& builder, llvm::Value* ptr, llvm::PointerType* asType, llvm::ArrayRef<llvm::Value*> indices, llvm::Twine name = "", llvm::AtomicOrdering ordering = llvm::AtomicOrdering::Unordered);
		llvm::LoadInst* MakeLoad(NomBuilder& builder, llvm::Value* ptr, llvm::Value* index, llvm::Twine name = "", llvm::AtomicOrdering ordering = llvm::AtomicOrdering::Unordered);

		llvm::LoadInst* MakeInvariantLoad(NomBuilder& builder, llvm::Module& mod, llvm::Value* ptr, llvm::AtomicOrdering ordering = llvm::AtomicOrdering::Unordered);
		llvm::LoadInst* MakeInvariantLoad(NomBuilder& builder, llvm::Value* ptr, llvm::Twine name = "", llvm::AtomicOrdering ordering = llvm::AtomicOrdering::Unordered);
		llvm::LoadInst* MakeInvariantLoad(NomBuilder& builder, llvm::Value* ptr, llvm::ArrayRef<llvm::Value*> indices, llvm::Twine name = "", llvm::AtomicOrdering ordering = llvm::AtomicOrdering::Unordered);
		llvm::LoadInst* MakeInvariantLoad(NomBuilder& builder, llvm::Value* ptr, llvm::PointerType* asType, llvm::ArrayRef<llvm::Value*> indices, llvm::Twine name = "", llvm::AtomicOrdering ordering = llvm::AtomicOrdering::Unordered);
		llvm::LoadInst* MakeInvariantLoad(NomBuilder& builder, llvm::Value* ptr, llvm::Value* index, llvm::Twine name = "", llvm::AtomicOrdering ordering = llvm::AtomicOrdering::Unordered);

		llvm::LoadInst* MakeLoad(NomBuilder& builder, llvm::Value* ptr, llvm::PointerType* asType, llvm::Value* index, llvm::Twine name = "", llvm::AtomicOrdering ordering = llvm::AtomicOrdering::Unordered);

		typedef void* (*ReadFieldFunction)(void*, int32_t);
		typedef void (*WriteFieldFunction)(void*, int32_t, void*);
		typedef void** (*FieldAddrFunction)(void*, int32_t);
		typedef void* (*ReadTypeArgFunction)(void*, int32_t);
		typedef void (*WriteTypeArgFunction)(void*, int32_t, void*);
		typedef void (*WriteVTableFunction)(void*, void*);
		ReadFieldFunction GetReadFieldFunction();
		WriteFieldFunction GetWriteFieldFunction();
		FieldAddrFunction GetFieldAddrFunction();
		ReadTypeArgFunction GetReadTypeArgFunction();
		WriteTypeArgFunction GetWriteTypeArgFunction();
		WriteVTableFunction GetWriteVTableFunction();
		void* GetGeneralLLVMFunction(llvm::StringRef name);
		void* GetBooleanTrue();
		void* GetBooleanFalse();
		void* GetVoidObj();

		llvm::Constant* EnsureIntegerSize(llvm::Constant* cnst, int bitwidth);
		llvm::Value* EnsureIntegerSize(NomBuilder& builder, llvm::Value* val, int bitwidth);

		llvm::Value* UnmaskDictionaryValue(NomBuilder& builder, llvm::Value* val);


		template<class T>
		typename ::std::enable_if<(sizeof(T) <= sizeof(uint64_t)) &&
			::std::is_trivially_copyable<T>::value, ::llvm::ConstantInt*>::type
			MakeInt(T val)
		{
			return llvm::ConstantInt::get(llvm::IntegerType::get(LLVMCONTEXT, bitsin(T)), bit_cast<uint64_t, T>(val), std::numeric_limits<T>::is_signed);
		}

		llvm::Constant* MakeLLVMStruct(llvm::ArrayRef<llvm::Constant*> args);

		llvm::ConstantInt* MakeUInt(size_t size, uint64_t val);
		llvm::ConstantInt* MakeSInt(size_t size, int64_t val);
		llvm::ConstantInt* MakeInt(size_t size, int64_t val);
		llvm::ConstantInt* MakeInt32(int32_t val);
		llvm::ConstantInt* MakeInt32(uint32_t val);
		llvm::ConstantInt* MakeInt32(uint64_t val);
		llvm::ConstantInt* MakeInt32(int64_t val);
		template<typename T>
		inline llvm::ConstantInt* MakeInt32(T val)
		{
			static_assert(sizeof(T) == sizeof(unsigned char), "This is definitely not a correct enum");
			return MakeInt32((uint32_t)((unsigned char)val));
		}
		llvm::ConstantInt* MakeInt(size_t size, uint64_t val);
		llvm::ConstantInt* MakeIntLike(llvm::Value* value, uint64_t val);

		llvm::Constant* GetLLVMRef(const void* const ptr);
		llvm::Constant* GetLLVMPointer(const void* const ptr);
		llvm::Constant* GetMask(int length, int leadingZeroes, int trailingZeroes);

		llvm::Value* CreatePointerEq(NomBuilder& builder, llvm::Value* left, llvm::Value* right, const llvm::Twine& name = "");
		void CreateExpect(NomBuilder& builder, llvm::Value* value, llvm::Value* expected);
	}
}