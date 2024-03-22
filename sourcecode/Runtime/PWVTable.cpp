#include "PWVTable.h"
#include "Defs.h"
#include "CompileHelpers.h"
#include "RTConfig_LambdaOpt.h"
#include "RTVTable.h"
#include "PWIMTFunction.h"

using namespace std;
using namespace llvm;
namespace Nom
{
	namespace Runtime
	{
		llvm::Type* PWVTable::GetLLVMType()
		{
			return RTVTable::GetLLVMType();
		}
		llvm::Type* PWVTable::GetWrappedLLVMType()
		{
			return NLLVMPointer(GetLLVMType());
		}
		PWBool PWVTable::CompareWith(NomBuilder& builder, PWVTable other) const
		{
			return builder->CreateICmpEQ(builder->CreatePtrToInt(wrapped, numtype(intptr_t)), builder->CreatePtrToInt(other.wrapped, numtype(intptr_t)), "vtablesEqual");
		}
		llvm::Value* Nom::Runtime::PWVTable::ReadHasRawInvoke(NomBuilder& builder) const
		{
			if (RTConfig_UseLambdaOffset)
			{
				return builder->CreateTrunc(builder->CreateLShr(builder->CreatePtrToInt(wrapped, numtype(intptr_t)), MakeInt<intptr_t>(3)), inttype(1));
			}
			else
			{
				auto flags = MakeInvariantLoad(builder, GetLLVMType(), wrapped, MakeInt32(RTVTableFields::Flags), "Flags", AtomicOrdering::NotAtomic);
				return builder->CreateICmpEQ(builder->CreateAnd(flags, MakeIntLike(flags, 1)), MakeIntLike(flags, 1));
			}
		}
		llvm::Value* PWVTable::ReadMethodPointer(NomBuilder& builder, const PWInt32 index) const
		{
			return MakeInvariantLoad(builder, GetLLVMType(), wrapped, { MakeInt32(0), MakeInt32(RTVTableFields::MethodTable), builder->CreateSub(MakeInt32(-1), index) }, "MethodPointer", AtomicOrdering::NotAtomic);
		}
		PWInt8 PWVTable::ReadKind(NomBuilder& builder) const
		{
			return MakeInvariantLoad(builder, GetLLVMType(), wrapped, MakeInt32(RTVTableFields::Kind), "VTableKind", AtomicOrdering::NotAtomic);
		}
		PWIMTFunction PWVTable::ReadIMTEntry(NomBuilder& builder, const PWInt32 index) const
		{
			return MakeInvariantLoad(builder, GetLLVMType(), wrapped, {MakeInt32(RTVTableFields::InterfaceMethodTable), index}, "IMTEntry", AtomicOrdering::NotAtomic);
		}
	}
}
