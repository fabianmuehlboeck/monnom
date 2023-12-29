#include "PWTypeVar.h"
#include "CompileHelpers.h"
#include "XRTInstanceType.h"
#include "PWInstanceType.h"
#include "PWInterface.h"
#include "PWTypeArr.h"
#include "llvm/IR/Constants.h"
#include "PWNomType.h"
#include "PWCastFunction.h"

using namespace llvm;
using namespace std;
namespace Nom
{
	namespace Runtime
	{
		llvm::Type* PWInstanceType::GetLLVMType()
		{
			return XRTInstanceType::GetLLVMType();
		}
		llvm::Type* PWInstanceType::GetWrappedLLVMType()
		{
			return NLLVMPointer(GetLLVMType());
		}
		void PWInstanceType::InitializeInstanceType(NomBuilder& builder, llvm::Value* hash, PWNomType nomtypeptr, PWInterface rtclassdesc, PWTypeArr ptrToTypeArgs)
		{
			InitializeType(builder, TypeKind::TKInstance, hash, nomtypeptr, ConstantPointerNull::get(GetCastFunctionType()->getPointerTo()));
			MakeInvariantStore(builder, rtclassdesc, GetLLVMType(), wrapped, MakeInt32(XRTInstanceTypeFields::Class));
			MakeInvariantStore(builder, ptrToTypeArgs, GetLLVMType(), wrapped, MakeInt32(XRTInstanceTypeFields::TypeArgs));
		}
		PWInterface PWInstanceType::ReadClassDescriptorLink(NomBuilder& builder)
		{
			return MakeInvariantLoad(builder, GetLLVMType(), wrapped, MakeInt32(XRTInstanceTypeFields::Class), "classDescriptor");
		}
		PWTypeArr PWInstanceType::TypeArgumentsPtr(NomBuilder& builder)
		{
			return MakeInvariantLoad(builder, GetLLVMType(), wrapped, MakeInt32(XRTInstanceTypeFields::TypeArgs), "typeArguments");
		}
	}
}
