#pragma once
#include "RTTypeHead.h"
#include "RTConcreteType.h"
#include "llvm/IR/IRBuilder.h"
#include "CompileEnv.h"
#include "ARTRep.h"

namespace Nom
{
	namespace Runtime
	{
		class NomTypeVar;
		enum class RTTypeVarFields : unsigned char { Head = 0, Index = 1, LowerBound = 2, UpperBound = 3 };
		class RTTypeVar : public RTConcreteType, public ARTRep<RTTypeVar, RTTypeVarFields>
		{
		private:
			//intptr_t index;
		public:
			static llvm::StructType *GetLLVMType();
			RTTypeVar(const char * entry/*, intptr_t offset = 0*/) : ARTRep<RTTypeVar, RTTypeVarFields>(entry/*, offset*/)
			{
			}
			RTTypeVar(const void * entry/*, intptr_t offset = 0*/) : ARTRep<RTTypeVar, RTTypeVarFields>(entry/*, offset*/)
			{
			}
			static llvm::Constant *GetConstant(int index, llvm::Constant *lowerBound, llvm::Constant *upperBound, const NomTypeVar *type);
			static int32_t HeadOffset();
			//static int32_t IndexOffset()
			//{
			//	static const int32_t offset = GetLLVMLayout()->getElementOffset(1); return offset;
			//}
			static int32_t LowerBoundOffset();
			static int32_t UpperBoundOffset();

			static llvm::Value * FromHead(NomBuilder &builder, llvm::Value *head);

			static llvm::Value* GenerateLoadIndex(NomBuilder& builder, llvm::Value* type);

			//RTTypeVar(intptr_t index) : index(index)
			//{

			//}
			//intptr_t GetIndex() const
			//{
			//	return index;
			//}
			//void* operator new  (std::size_t count, void* mem)
			//{
			//	return mem;
			//}
			//bool IsSubtype(const RTTypeHead& other) const;
			//bool IsSubtype(const RTClassType * const other) const
			//{
			//	return false;
			//}
			//bool IsSubtype(const RTIsectType * const other) const
			//{
			//	return false;
			//}
			//bool IsSubtype(const RTUnionType * const other) const
			//{
			//	return false;
			//}
			//bool IsSubtype(const RTTypeVar * const other) const
			//{
			//	return false;
			//}
			//RTTypeHead SubstituteSubtyping(const RTTypeHead * const args, size_t argcount) const;
			//RTTypeHead GetIntersectedForm() const;
			//std::list<const std::tuple<unsigned int, intptr_t>> GetVariableReferences(intptr_t start) const;
			
			//static RTTypeVar *GetInstance(intptr_t index)
			//{
			//	static std::map<intptr_t, RTTypeVar> vars;
			//	if (vars.count(index) == 0)
			//	{
			//		vars.insert_or_assign(index, RTTypeVar(index));
			//	}
			//	return &vars[index];
			//}

			//void WriteType(NomBuilder builder, CompileEnv* env) const;
		};
	}
}
