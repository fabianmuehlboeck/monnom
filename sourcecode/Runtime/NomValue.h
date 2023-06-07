#pragma once
#include "llvm/IR/Value.h"
#include "NomTypeDecls.h"

namespace Nom
{
	namespace Runtime
	{

		class NomType;
		class PWRefValue;
		class NomBuilder;

		NomTypeRef GetIntClassType();
		NomTypeRef GetFloatClassType();
		NomTypeRef GetBoolClassType();
		NomTypeRef GetDynamicType();
		NomTypeRef GetDynOrPrimitiveType(llvm::Value* val);
		void __NomValueContainerDebugCheck(NomTypeRef type, llvm::Type * valType);
		template<typename T>
		class NomValueContainer
		{
		protected:
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

		class NomValue : public NomValueContainer<llvm::Value>
		{
		public:
			NomValue() {}
			NomValue(llvm::Value* val, NomTypeRef type, bool isFunctionCall = false) : NomValueContainer(val, type, isFunctionCall)
			{

			}
			NomValue(llvm::Value* val, bool isFunctionCall = false) : NomValueContainer(val, isFunctionCall)
			{

			}
			~NomValue() {}
			PWRefValue operator->() const;
			PWRefValue AsPWRef() const;
			int GenerateRefOrPrimitiveValueSwitch(NomBuilder& builder, llvm::BasicBlock** refValueBlock, llvm::BasicBlock** intBlock, llvm::BasicBlock** floatBlock, bool unpackPrimitives = false, llvm::BasicBlock** primitiveIntBlock = nullptr, llvm::Value** primitiveIntVar = nullptr, llvm::BasicBlock** primitiveFloatBlock = nullptr, llvm::Value** primitiveFloatVar = nullptr, llvm::BasicBlock** primitiveBoolBlock = nullptr, llvm::Value** primitiveBoolVar = nullptr, int refWeight = 100, int intWeight = 50, int floatWeight = 40, int boolWeight = 30);
			int GenerateRefOrPrimitiveValueSwitch(NomBuilder& builder, llvm::BasicBlock** refValueBlock, llvm::BasicBlock** intBlock, llvm::BasicBlock** floatBlock, int refWeight, int intWeight = 50, int floatWeight = 40, int boolWeight = 30);

		};
	}
}

