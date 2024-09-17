#pragma once
#include "llvm/IR/Value.h"
#include "../Intermediate_Representation/Types/NomTypeDecls.h"

namespace Nom
{
	namespace Runtime
	{

		class NomType;

		NomTypeRef GetIntClassType();
		NomTypeRef GetFloatClassType();
		NomTypeRef GetBoolClassType();
		NomTypeRef GetDynamicType();
		NomTypeRef GetDynOrPrimitiveType(llvm::Value* val);
		void __NomValueContainerDebugCheck(NomTypeRef type, llvm::Type * valType);
		template<typename T>
		class NomValueContainer
		{
		private:
			T * val;
			NomTypeRef type;
			bool isFunctionCall;
			// Currently I (Alex) think that if the llvm type is something like Double or Int then
			// the val will actually contain an unwrapped type and we are done.
			// If tag is null then its a wrapped type unless val has an llvm type that says its not
			// For now, once an unwrapped value becomes wrapped then we just always use the wrapped
			// value from then on, unwrapping it if necessary
			// We only need to do wrapping up if there is a tag. If there is no tag then whatever
			//
			// Alternatively, always have a tag, and if the type if obvious then the tag will be obvious
			// So llvm can optimise any switches
			T * tag;
			llvm::BasicBlock * packValBlock;
		public:
			NomValueContainer() : val(nullptr), type(nullptr)
				, tag(nullptr), packValBlock(nullptr) 
				{}
			NomValueContainer(T *val, NomTypeRef type, bool isFunctionCall=false) : val(val), type(type), isFunctionCall(isFunctionCall)
				, tag(nullptr), packValBlock(nullptr) 
			{
				#ifdef _DEBUG
				__NomValueContainerDebugCheck(type, val->getType());
				#endif
			}
			NomValueContainer(T* val, bool isFunctionCall = false) : val(val), type(GetDynOrPrimitiveType(val)), isFunctionCall(isFunctionCall)
			, tag(nullptr), packValBlock(nullptr) 
			{
				#ifdef _DEBUG
				__NomValueContainerDebugCheck(type, val->getType());
				#endif
			}
			NomValueContainer(T* val, T* tag, llvm::BasicBlock* packValBlock, NomTypeRef type, bool isFunctionCall = false) : val(val), type(type), isFunctionCall(isFunctionCall)
			, tag(tag), packValBlock(packValBlock) 
			{
				#ifdef _DEBUG
				__NomValueContainerDebugCheck(type, val->getType());
				#endif
			}
			~NomValueContainer() {}

			T * operator*() const
			{
				return val;
			}
			T * operator->() const
			{
				return val;
			}

			NomTypeRef GetNomType() const
			{
				return type;
			}
			bool IsFunctionCall() const
			{
				return isFunctionCall;
			}

			T * GetTag() const
			{
				return tag;
			}
			llvm::BasicBlock * GetPackBlock() {
				return packValBlock;
			}

			void MakePacked(T* newVal) {
				val = newVal;
				tag = nullptr;
			}

			operator T*() const { return val; }
			operator NomTypeRef() const { return type; }
		};

		using NomValue = NomValueContainer<llvm::Value>;
	}
}

