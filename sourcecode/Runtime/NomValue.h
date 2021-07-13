#pragma once
#include "llvm/IR/Value.h"
#include "NomTypeDecls.h"

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
		public:
			NomValueContainer() : val(nullptr), type(nullptr) {}
			NomValueContainer(T *val, NomTypeRef type, bool isFunctionCall=false) : val(val), type(type), isFunctionCall(isFunctionCall) {
				#ifdef _DEBUG
				__NomValueContainerDebugCheck(type, val->getType());
				#endif
			}
			NomValueContainer(T* val, bool isFunctionCall = false) : val(val), type(GetDynOrPrimitiveType(val)), isFunctionCall(isFunctionCall) {
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

			operator T*() const { return val; }
			operator NomTypeRef() const { return type; }
		};

		using NomValue = NomValueContainer<llvm::Value>;
	}
}

