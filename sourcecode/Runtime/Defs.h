#pragma once
PUSHDIAGSUPPRESSION
#include "llvm/IR/Type.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Constants.h"
#include "llvm/ADT/Twine.h"
#include "llvm/IR/IRBuilder.h"
POPDIAGSUPPRESSION
#include <string>
#include "BoehmAtomicAllocator.h"
#include "Context.h"
#include "CharStream.h"
#include "NomValue.h"
#include <limits>
#include <type_traits>
#include "DLLExport.h"

#define BITSINABYTE 8
#define bitsin(t) (sizeof(t) * BITSINABYTE)
#define inttype(s) (::llvm::IntegerType::get(LLVMCONTEXT, (s)))
#define numtype(t) (inttype(bitsin(t)))
#define arrtype(t,n) (::llvm::ArrayType::get(t,n))
#define llvmsizeof(t) (llvm::ConstantExpr::getPtrToInt(::llvm::ConstantExpr::getGetElementPtr((t),llvm::ConstantPointerNull::get((t)->getPointerTo()), llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(LLVMCONTEXT), 1)), llvm::IntegerType::get(LLVMCONTEXT, bitsin(size_t))))
#define INTTYPE (::Nom::Runtime::BytecodeTypes::GetIntType())
#define FLOATTYPE (::Nom::Runtime::BytecodeTypes::GetFloatType())
#define BOOLTYPE (::Nom::Runtime::BytecodeTypes::GetBoolType())
#define REFTYPE (::Nom::Runtime::BytecodeTypes::GetRefType())
#define POINTERTYPE (::Nom::Runtime::BytecodeTypes::GetPointerType())
#define LLVMCONSTANTINDEXTYPE (::Nom::Runtime::BytecodeTypes::GetConstantIndexType())
#define TYPETYPE (::Nom::Runtime::BytecodeTypes::GetTypeType())

#define NOM_CONSTANT_DEPENCENCY_CONTAINER llvm::SmallVector<Nom::Runtime::ConstantID, 16>

#ifdef _WIN32
#define makealloca(t,n) (static_cast<t *>((n)>0?(_malloca(sizeof(t)*static_cast<size_t>(n))):nullptr))
#else
#define makealloca(t,n) (static_cast<t *>((n)>0?(alloca(sizeof(t)*static_cast<size_t>(n))):nullptr))
#endif
#define nullarray(t) (::llvm::ArrayRef<t>(static_cast<t *>(nullptr),static_cast<size_t>(0)))
#define notnullarray(t, arr, n) (::llvm::ArrayRef<t>((arr==nullptr?reinterpret_cast<t *>(this) : (arr)), (arr==nullptr?0:(n))))


namespace Nom
{
	namespace Runtime
	{
		class NLLVMPointer
		{
		private:
			llvm::Type* const target;
		public:
			NLLVMPointer(llvm::Type* tgt) : target(tgt)
			{
			}
			inline bool operator==(const NLLVMPointer& other) const
			{
				return target==other.target;
			}
			inline llvm::PointerType* operator->() const
			{
				return llvm::PointerType::get(target->getContext(), 0);
			}
			inline operator llvm::PointerType*() const
			{
				return llvm::PointerType::get(target->getContext(), 0);
			}
			inline llvm::Type* GetTargetType() const
			{
				return target;
			}
			llvm::PointerType* AsLLVMType() const
			{
				return llvm::PointerType::get(target->getContext(), 0);
			}
		};
		class NLLVMPointerArr
		{
		private:
			llvm::Type* const target;
			const uint64_t size;
		public:
			NLLVMPointerArr(llvm::Type* tgt, uint64_t sz) : target(tgt), size(sz)
			{

			}
			inline bool operator==(const NLLVMPointerArr& other) const
			{
				return target == other.target && size == other.size;
			}
			inline llvm::ArrayType* operator->() const
			{
				return llvm::ArrayType::get(llvm::PointerType::get(target->getContext(), 0), size);
			}
			inline operator llvm::ArrayType* () const
			{
				return llvm::ArrayType::get(llvm::PointerType::get(target->getContext(), 0), size);
			}
			inline llvm::Type* GetTargetType() const
			{
				return target;
			}
			inline uint64_t GetSize() const
			{
				return size;
			}
			llvm::ArrayType* AsLLVMType() const
			{
				return llvm::ArrayType::get(llvm::PointerType::get(target->getContext(), 0), size);
			}
		};
		template <typename T>
		class NLLVMTypeWrap
		{
		private:
			llvm::Type* const annotation;
			T* const wrapped;
		public:
			NLLVMTypeWrap(T* _wrapped) : annotation(0), wrapped(_wrapped)
			{

			}
			NLLVMTypeWrap(T * _wrapped, llvm::Type* _annotation) : annotation(_annotation), wrapped(_wrapped)
			{

			}
			inline bool operator==(const NLLVMTypeWrap<T>& other) const
			{
				return wrapped == other.wrapped;
			}
			inline T* operator->() const
			{
				return wrapped;
			}
			inline operator T* () const
			{
				return wrapped;
			}
			inline llvm::Type* GetAnnotation() const
			{
				return annotation;
			}
		};
		using NLLVMConstant = NLLVMTypeWrap<llvm::Constant>;
		using NLLVMValue = NLLVMTypeWrap<llvm::Value>;
		using NLLVMGlobalVariable = NLLVMTypeWrap<llvm::GlobalVariable>;
		using NLLVMType = NLLVMTypeWrap<llvm::Type>;

		using RegIndex = size_t;
		using BinRegIndex = uint32_t;
		using ConstantID = uint64_t;
		using LocalConstantID = uint64_t;
		enum class NomConstantType : unsigned char
		{
			CTString = 103, 
			CTInteger = 104,
			CTFloat = 105,
			CTRecord = 110,
			CTClass = 111,
			CTInterface = 112,
			CTTypeParameter = 113,
			CTLambda = 114,
			CTIntersection = 115,
			CTUnion = 116,
			CTBottom = 117,
			CTDynamic=118,
			CTMaybe=119,
			CTClassType = 121,
			CTTypeVar = 122,
			CTMethod = 131,
			CTStaticMethod = 132,
			CTConstructor = 133,
			CTSuperClass = 141,
			CTSuperInterfaces = 142,
			CTTypeList = 191,
			CTSuperInterfaceList = 192,
			CTClassTypeList = 193,
			CTTypeParameters = 194
		};

		enum class Visibility : unsigned char
		{
			Private = 1,
			Protected = 3,
			Internal = 7,
			ProtectedInternal = 15,
			Public = 31
		};

		enum class TypeKind :unsigned char { TKClass, TKTop, TKVariable, TKBottom, TKInstance, TKRecord, TKLambda, TKPartialApp, TKDynamic, TKMaybe };
		using TypeID = int64_t;
		class NomInterface;
		using InterfaceID = uint32_t;

		using NomChar = char16_t;
		using NomStringType = std::basic_string<NomChar, std::char_traits<NomChar>, BoehmAtomicAllocator<NomChar>>;
		class NomString;

		class BytecodeTypes
		{
		public:
			static llvm::Type * GetIntType() {
				static llvm::Type * intType = (llvm::Type::getInt64Ty(LLVMCONTEXT));
				return intType;
			}

			static llvm::Type * GetFloatType() {
				static llvm::Type * floatType = (llvm::Type::getDoubleTy(LLVMCONTEXT));
				return floatType;
			}

			static llvm::Type* GetBoolType() {
				static llvm::Type* boolType = (llvm::Type::getInt1Ty(LLVMCONTEXT));
				return boolType;
			}

			static NLLVMPointer &GetRefType();

			static llvm::Type * GetConstantIndexType() {
				static llvm::Type * ciType = llvm::Type::getIntNTy(LLVMCONTEXT, bitsin(::Nom::Runtime::ConstantID));
				return ciType;
			}

			static NLLVMPointer &GetPointerType() {
				static NLLVMPointer ptrType = llvm::Type::getIntNTy(LLVMCONTEXT, bitsin(char));
				return ptrType;
			}

			static NLLVMPointer & GetTypeType();

		};

		template <class To, class From>
		typename ::std::enable_if<(sizeof(To) >= sizeof(From)) &&
			::std::is_trivially_copyable<From>::value &&
			::std::is_trivial<To>::value, To>::type
		bit_cast(const From &src) noexcept
		{
			To dst;
			size_t copysize = sizeof(To);
			if (sizeof(To) > sizeof(From))
			{
				copysize = sizeof(From);
			}
			std::memcpy(&dst, &src, copysize);
			return dst;
		}

		using NomStringRef = const NomString * const;

		template <typename E>
		constexpr auto to_underlying(E e) noexcept
		{
			return static_cast<std::underlying_type_t<E>>(e);
		}
	}
}

extern "C" DLLEXPORT void RT_NOM_PRINT_STORE(void* val, void* addr);

extern "C" DLLEXPORT void RT_NOM_PRINT_LOAD(void* val, void* addr);
