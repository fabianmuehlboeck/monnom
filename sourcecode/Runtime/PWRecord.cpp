#include "PWRecord.h"
#include "PWStructDict.h"
#include "RecordHeader.h"
#include "PWInt.h"

using namespace llvm;
namespace Nom
{
	namespace Runtime
	{
		llvm::Type* PWRecord::GetLLVMType()
		{
			return RecordHeader::GetLLVMType();
		}
		llvm::Type* PWRecord::GetWrappedLLVMType()
		{
			return NLLVMPointer(GetLLVMType());
		}
		llvm::Value* PWRecord::ReadField(NomBuilder& builder, PWInt32 fieldIndex)
		{
			return MakeLoad(builder, REFTYPE, builder->CreateGEP(GetLLVMType(), wrapped, { MakeInt32(0), MakeInt32(StructHeaderFields::Fields), fieldIndex.AShr(builder, MakePWInt32(3)), MakePWInt32(1), fieldIndex.Resize<3>(builder).Resize<32>(builder) }), "field");
		}
		PWInt8 PWRecord::ReadWrittenTag(NomBuilder& builder, PWInt32 fieldIndex)
		{
			return MakeLoad(builder, GetLLVMType(), wrapped, { MakeInt32(StructHeaderFields::Fields), fieldIndex.AShr(builder, MakePWInt32(3)), MakeInt32(0), fieldIndex.Resize<3>(builder).Resize<32>(builder) }, "writtenTag", llvm::AtomicOrdering::Acquire);
		}
		void PWRecord::WriteField(NomBuilder& builder, PWInt32 fieldIndex, llvm::Value* value)
		{
			MakeStore(builder, value, builder->CreateGEP(GetLLVMType(), wrapped, { MakeInt32(0), MakeInt32(StructHeaderFields::Fields), fieldIndex.AShr(builder, MakePWInt32(3)), MakeInt32(1), fieldIndex.Resize<3>(builder).Resize<32>(builder) }));
		}
		void PWRecord::WriteWrittenTag(NomBuilder& builder, PWInt32 fieldIndex)
		{
			MakeStore(builder, ConstantInt::get(inttype(8), 1), builder->CreateGEP(GetLLVMType(), wrapped, { MakeInt32(0), MakeInt32(StructHeaderFields::Fields), fieldIndex.AShr(builder, MakePWInt32(3)), MakeInt32(0), fieldIndex.Resize<3>(builder).Resize<32>(builder) }), llvm::AtomicOrdering::Release);
		}
		PWStructDict PWRecord::ReadStructDict(NomBuilder& builder)
		{
			return builder->CreateGEP(GetLLVMType(), wrapped, { MakeInt32(0), MakeInt32(StructHeaderFields::InstanceDictionary), MakeInt32(0) });
		}
	}
}
