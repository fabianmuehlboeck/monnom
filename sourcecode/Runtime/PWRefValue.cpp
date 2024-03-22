#include "Defs.h"
#include "CompileHelpers.h"
#include "PWRefValue.h"
#include "RTConfig.h"
#include "IMT.h"
#include "IntClass.h"
#include "FloatClass.h"
#include "BoolClass.h"
#include "PWIMTFunction.h"

using namespace std;
using namespace llvm;
namespace Nom
{
	namespace Runtime
	{
		PWVTable PWRefValue::ReadVTable(NomBuilder& builder) const
		{
			return PWVTable(MakeInvariantLoad(builder, RefValueHeader::GetLLVMType(), wrapped, MakeInt32(RefValueHeaderFields::InterfaceTable)));
		}
		void PWRefValue::WriteVTable(NomBuilder& builder, PWVTable vtbl) const
		{
			MakeInvariantStore(builder, vtbl, RTVTable::GetLLVMType(), wrapped, MakeInt32(RefValueHeaderFields::InterfaceTable));
		}
		llvm::Value* PWRefValue::WriteVTableCMPXCHG(NomBuilder& builder, PWVTable vtbl, PWVTable orig) const
		{
			auto vtableAddress = builder->CreateGEP(RefValueHeader::GetLLVMType(), wrapped, { MakeInt32(0), MakeInt32(RefValueHeaderFields::InterfaceTable) });
			return builder->CreateAtomicCmpXchg(vtableAddress, orig, vtbl, MaybeAlign(8), AtomicOrdering::AcquireRelease, AtomicOrdering::Acquire);
		}
		llvm::Value* PWRefValue::ReadRawInvoke(NomBuilder& builder) const
		{
			if (NomLambdaOptimizationLevel == 0)
			{
				throw new std::exception();
			}
			return MakeInvariantLoad(builder, RefValueHeader::GetLLVMType(), wrapped, { MakeInt32(RefValueHeaderFields::RawInvoke),MakeInt32(0) });
		}
		void PWRefValue::WriteRawInvoke(NomBuilder& builder, llvm::Value* rawinvoke) const
		{
			if (NomLambdaOptimizationLevel == 0)
			{
				throw new std::exception();
			}
			MakeStore(builder, rawinvoke, RefValueHeader::GetLLVMType(), wrapped, { MakeInt32(RefValueHeaderFields::RawInvoke),MakeInt<int32_t>(0) });
		}
		llvm::AtomicCmpXchgInst* PWRefValue::WriteRawInvokeCMPXCHG(NomBuilder& builder, llvm::Value* rawinvoke, llvm::Value* orig) const
		{
			if (NomLambdaOptimizationLevel == 0)
			{
				throw new std::exception();
			}
			return builder->CreateAtomicCmpXchg(builder->CreateGEP(RefValueHeader::GetLLVMType(),wrapped, { MakeInt32(0), MakeInt32(RefValueHeaderFields::RawInvoke),MakeInt<int32_t>(0) }),
				orig,
				rawinvoke,
				MaybeAlign(8),
				llvm::AtomicOrdering::Monotonic,
				llvm::AtomicOrdering::Monotonic);
		}
	}
}
