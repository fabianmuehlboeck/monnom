#pragma once
PUSHDIAGSUPPRESSION
#include "llvm/IR/Value.h"
POPDIAGSUPPRESSION
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
		void _nomValueContainerDebugCheck(NomTypeRef type, llvm::Type * valType);
		template<typename T>
		class NomValueContainer
		{
		protected:
			T * val;
			NomTypeRef type;
			bool isFunctionCall;
		public:
			NomValueContainer() : val(nullptr), type(nullptr) {}
			NomValueContainer(T *v, NomTypeRef t, bool isfc=false) : val(v), type(t), isFunctionCall(isfc) {
				#ifdef _DEBUG
				_nomValueContainerDebugCheck(type, val->getType());
				#endif
			}
			NomValueContainer(T* v, bool isfc = false) : val(v), type(GetDynOrPrimitiveType(v)), isFunctionCall(isfc) {
				#ifdef _DEBUG
				_nomValueContainerDebugCheck(type, val->getType());
				#endif
			}

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
			NomValue(llvm::Value* v, NomTypeRef t, bool isfc = false) : NomValueContainer(v, t, isfc)
			{

			}
			NomValue(llvm::Value* v, bool isfc = false) : NomValueContainer(v, isfc)
			{

			}
			PWRefValue operator->() const;
			PWRefValue AsPWRef() const;
			int GenerateRefOrPrimitiveValueSwitch(NomBuilder& builder, llvm::BasicBlock** refValueBlock, llvm::BasicBlock** intBlock, llvm::BasicBlock** floatBlock, bool unpackPrimitives = false, llvm::BasicBlock** primitiveIntBlock = nullptr, llvm::Value** primitiveIntVar = nullptr, llvm::BasicBlock** primitiveFloatBlock = nullptr, llvm::Value** primitiveFloatVar = nullptr, llvm::BasicBlock** primitiveBoolBlock = nullptr, llvm::Value** primitiveBoolVar = nullptr, uint64_t refWeight = 100, uint64_t intWeight = 50, uint64_t floatWeight = 40, uint64_t boolWeight = 30);
			int GenerateRefOrPrimitiveValueSwitch(NomBuilder& builder, llvm::BasicBlock** refValueBlock, llvm::BasicBlock** intBlock, llvm::BasicBlock** floatBlock, uint64_t refWeight, uint64_t intWeight = 50, uint64_t floatWeight = 40, uint64_t boolWeight = 30);

		};
	}
}

