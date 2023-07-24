#include "PWSubstStack.h"
#include "PWType.h"
#include "PWTypeVar.h"
#include "RTSubstStack.h"
#include "CompileHelpers.h"
#include "NomJITLight.h"

namespace Nom
{
	namespace Runtime
	{
		llvm::Type* PWSubstStack::GetLLVMType()
		{
			return RTSubstStack::GetLLVMType();
		}
		llvm::Type* PWSubstStack::GetWrappedLLVMType()
		{
			return NLLVMPointer(GetLLVMType());
		}
		PWSubstStack PWSubstStack::Alloca(NomBuilder& builder, PWSubstStack substStack, PWTypeArr args, llvm::Twine name)
		{
			llvm::Value* alloced = builder->CreateAlloca(GetLLVMType(), MakeInt32(1), name);
			builder->CreateIntrinsic(llvm::Intrinsic::lifetime_start, { POINTERTYPE }, { MakeInt<uint64_t>(GetNomJITDataLayout().getTypeAllocSize(GetLLVMType())), alloced });
			MakeInvariantStore(builder, substStack.wrapped == nullptr ? llvm::ConstantPointerNull::get(NLLVMPointer(GetLLVMType())) : substStack.wrapped, GetLLVMType(), alloced, MakeInt32(TypeArgumentListStackFields::Next));
			MakeInvariantStore(builder, args, GetLLVMType(), alloced, MakeInt32(TypeArgumentListStackFields::Types));
			auto invariantID = builder->CreateIntrinsic(llvm::Intrinsic::invariant_start, { POINTERTYPE }, { MakeInt<uint64_t>(GetNomJITDataLayout().getTypeAllocSize(GetLLVMType())), alloced });
			return PWSubstStack(alloced, invariantID);
		}
		PWSubstStack::PWSubstStack(llvm::Value* _wrapped, llvm::Value* _invariantID) : PWrapper(_wrapped), invariantID(_invariantID)
		{

		}
		PWType PWSubstStack::Pop(NomBuilder& builder, PWInt32 varIndex, PWSubstStack* newStackAddr) const
		{
			if (newStackAddr != nullptr)
			{
				*newStackAddr = PWSubstStack(MakeInvariantLoad(builder, GetLLVMType(), wrapped, MakeInt32(TypeArgumentListStackFields::Next)));
			}
			auto typeList = MakeInvariantLoad(builder, GetLLVMType(), wrapped, MakeInt32(TypeArgumentListStackFields::Types));
			return MakeInvariantLoad(builder, TYPETYPE, builder->CreateGEP(TYPETYPE, typeList, builder->CreateSub(MakeInt32(-1), varIndex)));
		}
		PWType PWSubstStack::Pop(NomBuilder& builder, PWTypeVar var, PWSubstStack* newStackAddr) const
		{
			return Pop(builder, var.ReadIndex(builder), newStackAddr);
		}
		PWSubstStack PWSubstStack::ReadSuccStack(NomBuilder& builder, llvm::Twine name) const
		{
			return PWSubstStack(MakeLoad(builder, GetLLVMType(), wrapped, MakeInt32(TypeArgumentListStackFields::Next), name), nullptr);
		}
		PWTypeArr PWSubstStack::ReadSubstitutions(NomBuilder& builder, llvm::Twine name) const
		{
			return MakeLoad(builder, GetLLVMType(), wrapped, MakeInt32(TypeArgumentListStackFields::Types), name);
		}
		void PWSubstStack::Release(NomBuilder& builder) const
		{
			if (invariantID == nullptr)
			{
				throw new std::exception();
			}
			builder->CreateIntrinsic(llvm::Intrinsic::invariant_end, { POINTERTYPE }, { invariantID, MakeInt<uint64_t>(GetNomJITDataLayout().getTypeAllocSize(RTSubstStack::GetLLVMType())), wrapped });
			builder->CreateIntrinsic(llvm::Intrinsic::lifetime_end, { POINTERTYPE }, { MakeInt<uint64_t>(GetNomJITDataLayout().getTypeAllocSize(RTSubstStack::GetLLVMType())), wrapped });
		}
	}
}
