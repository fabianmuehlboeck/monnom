#pragma once
PUSHDIAGSUPPRESSION
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/IR/Value.h"
POPDIAGSUPPRESSION
#include "Defs.h"
#include "TypeOperations.h"
#include "NomBuilder.h"
#include "PWInt.h"

namespace Nom
{
	namespace Runtime
	{
		class CompileEnv;
		llvm::Type* GetGEPTargetType(llvm::Type* origType, llvm::ArrayRef<llvm::Value*> arr);
		llvm::Type* GetGEPTargetType(llvm::Type* origType, llvm::ArrayRef<llvm::Constant*> arr);

		llvm::CallInst* GenerateFunctionCall(NomBuilder &builder, llvm::Module &mod, llvm::Value* fun, llvm::FunctionType* ftype, llvm::ArrayRef<llvm::Value*>& args, bool fastcall = true);
		llvm::CallInst* GenerateFunctionCall(NomBuilder& builder, llvm::Module& mod, llvm::Function* fun, llvm::ArrayRef<llvm::Value*>& args, bool fastcall);

		llvm::Value* GenerateIsNull(NomBuilder& builder, llvm::Value* val);
		void CreateDummyReturn(NomBuilder& builder, llvm::Function* fun);

		llvm::StoreInst* MakeStore(NomBuilder& builder, llvm::Value* val, llvm::Value* ptr, llvm::AtomicOrdering ordering = llvm::AtomicOrdering::Unordered);
		llvm::StoreInst* MakeStore(NomBuilder& builder, llvm::Value* val, llvm::Type* targetType, llvm::Value* ptr, llvm::ArrayRef<llvm::Value*> indices, llvm::AtomicOrdering ordering = llvm::AtomicOrdering::Unordered);
		llvm::StoreInst* MakeStore(NomBuilder& builder, llvm::Value* val, llvm::Type* targetType, llvm::Value* ptr, PWInt32 index, llvm::AtomicOrdering ordering = llvm::AtomicOrdering::Unordered);
		llvm::StoreInst* MakeStore(NomBuilder& builder, llvm::Value* val, llvm::Type* targetType, llvm::Value* ptr, PWCInt32 index, llvm::AtomicOrdering ordering = llvm::AtomicOrdering::Unordered);

		llvm::StoreInst* MakeInvariantStore(NomBuilder& builder, llvm::Value* val, llvm::Value* ptr, llvm::AtomicOrdering ordering = llvm::AtomicOrdering::Unordered);
		llvm::StoreInst* MakeInvariantStore(NomBuilder& builder, llvm::Value* val, llvm::Type* targetType, llvm::Value* ptr, llvm::ArrayRef<llvm::Value*> indices, llvm::AtomicOrdering ordering = llvm::AtomicOrdering::Unordered);
		llvm::StoreInst* MakeInvariantStore(NomBuilder& builder, llvm::Value* val, llvm::Type* targetType, llvm::Value* ptr, PWInt32 index, llvm::AtomicOrdering ordering = llvm::AtomicOrdering::Unordered);
		llvm::StoreInst* MakeInvariantStore(NomBuilder& builder, llvm::Value* val, llvm::Type* targetType, llvm::Value* ptr, PWCInt32 index, llvm::AtomicOrdering ordering = llvm::AtomicOrdering::Unordered);

		llvm::LoadInst* MakeLoad(NomBuilder& builder, llvm::Type* elementType, llvm::Value* ptr, llvm::Twine name = "", llvm::AtomicOrdering ordering = llvm::AtomicOrdering::Unordered);
		llvm::LoadInst* MakeLoad(NomBuilder& builder, llvm::Type* elementType, llvm::Value* ptr, llvm::ArrayRef<llvm::Value*> indices, llvm::Twine name = "", llvm::AtomicOrdering ordering = llvm::AtomicOrdering::Unordered);
		llvm::LoadInst* MakeLoad(NomBuilder& builder, llvm::Type* elementType, llvm::Value* ptr, PWInt32 index, llvm::Twine name = "", llvm::AtomicOrdering ordering = llvm::AtomicOrdering::Unordered);
		llvm::LoadInst* MakeLoad(NomBuilder& builder, llvm::Type* elementType, llvm::Value* ptr, PWCInt32 index, llvm::Twine name = "", llvm::AtomicOrdering ordering = llvm::AtomicOrdering::Unordered);

		llvm::LoadInst* MakeInvariantLoad(NomBuilder& builder, llvm::Type* elementType, llvm::Value* ptr, llvm::Twine name = "", llvm::AtomicOrdering ordering = llvm::AtomicOrdering::Unordered);
		llvm::LoadInst* MakeInvariantLoad(NomBuilder& builder, llvm::Type* elementType, llvm::Value* ptr, llvm::ArrayRef<llvm::Value*> indices, llvm::Twine name = "", llvm::AtomicOrdering ordering = llvm::AtomicOrdering::Unordered);
		llvm::LoadInst* MakeInvariantLoad(NomBuilder& builder, llvm::Type* elementType, llvm::Value* ptr, PWInt32 index, llvm::Twine name = "", llvm::AtomicOrdering ordering = llvm::AtomicOrdering::Unordered);
		llvm::LoadInst* MakeInvariantLoad(NomBuilder& builder, llvm::Type* elementType, llvm::Value* ptr, PWCInt32 index, llvm::Twine name = "", llvm::AtomicOrdering ordering = llvm::AtomicOrdering::Unordered);

		typedef void* (*ReadFieldFunction)(void*, int32_t);
		typedef void (*WriteFieldFunction)(void*, int32_t, void*);
		typedef void** (*FieldAddrFunction)(void*, int32_t);
		typedef void* (*ReadTypeArgFunction)(void*, int32_t);
		typedef void (*WriteTypeArgFunction)(void*, int32_t, void*);
		typedef void (*WriteVTableFunction)(void*, void*);
		ReadFieldFunction GetReadFieldFunction();
		WriteFieldFunction GetWriteFieldFunction();
		FieldAddrFunction GetFieldAddrFunction();
		ReadFieldFunction GetReadFieldFunction_ForInvoke();
		WriteFieldFunction GetWriteFieldFunction_ForInvoke();
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
		PWCInt32 MakeInt32(int32_t val);
		PWCInt32 MakeInt32(uint32_t val);
		PWCInt32 MakeInt32(uint64_t val);
		PWCInt32 MakeInt32(int64_t val);
		template<typename T>
		PWCInt32 MakeInt32(T val)
		{
			static_assert(sizeof(std::underlying_type_t<T>) == sizeof(unsigned char), "This is definitely not a correct enum");
			return MakeInt32(static_cast<uint32_t>(static_cast<std::underlying_type_t<T>>(val)));
		}
		llvm::ConstantInt* MakeInt(size_t size, uint64_t val);
		llvm::ConstantInt* MakeIntLike(llvm::Value* value, uint64_t val);

		llvm::Constant* GetLLVMRef(const void* const ptr);
		llvm::Constant* GetLLVMPointer(const void* const ptr);
		llvm::Constant* GetMask(int length, int leadingZeroes, int trailingZeroes);

		llvm::Value* CreatePointerEq(NomBuilder& builder, llvm::Value* left, llvm::Value* right, const llvm::Twine& name = "");
		void CreateExpect(NomBuilder& builder, llvm::Value* value, llvm::Value* expected);

		template<typename T>
		PWCInt8 MakeInt8(T v)
		{
			if (std::is_unsigned_v< std::underlying_type_t<T> > ==true)
			{
				PWCInt32 pwuint = MakePWUInt32(static_cast<std::underlying_type_t<T> >(v));
				return pwuint.Resize<8>();
			}
			else
			{
				PWCInt32 pwint = MakePWInt32(static_cast<std::underlying_type_t<T> >(v));
				return pwint.Resize<8>();
			}
		}
	}
}
