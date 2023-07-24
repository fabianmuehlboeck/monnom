#include "PWClass.h"
#include "Defs.h"
#include "CompileHelpers.h"
#include "RTClass.h"

using namespace std;
using namespace llvm;
namespace Nom
{
	namespace Runtime
	{
		llvm::Type* PWClass::GetLLVMType()
		{
			return RTClass::GetLLVMType();
		}
		llvm::Type* PWClass::GetWrappedLLVMType()
		{
			return NLLVMPointer(GetLLVMType());
		}
		PWClass::PWClass(llvm::Value* _wrapped) : PWVTable(_wrapped)
		{

		}
		PWClass PWClass::FromVTable(PWVTable vt)
		{
			return PWClass(vt.wrapped);
		}
		PWInterface PWClass::GetInterface(NomBuilder& builder) const
		{
			return PWInterface(builder->CreateGEP(RTClass::GetLLVMType(), wrapped, llvm::ArrayRef<Value*>({ MakeInt32(0), MakeInt32(RTClassFields::RTInterface) })));
		}

		PWInt64 PWClass::ReadFieldCount(NomBuilder& builder) const
		{
			return MakeInvariantLoad(builder,GetLLVMType(), wrapped, MakeInt32(RTClassFields::FieldCount), "FieldCount", AtomicOrdering::NotAtomic);
		}

	}
}
