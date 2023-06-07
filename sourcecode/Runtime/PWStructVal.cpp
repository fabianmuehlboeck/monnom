#include "PWStructVal.h"
#include "StructuralValueHeader.h"
#include "CompileHelpers.h"
#include "PWTypeArr.h"
#include "PWCastData.h"
#include "llvm/Support/AtomicOrdering.h"

using namespace llvm;
using namespace std;
namespace Nom
{
	namespace Runtime
	{
		llvm::Value* PWStructVal::ReadTypeArgument(NomBuilder& builder, int32_t index)
		{
			return ReadTypeArgument(builder, MakeInt32(index));
		}
		llvm::Value* PWStructVal::ReadTypeArgument(NomBuilder& builder, llvm::Value* index)
		{
			auto targAddress = builder->CreateGEP(StructuralValueHeader::GetLLVMType(), wrapped, { MakeInt32(0), MakeInt32(StructuralValueHeaderFields::TypeArgs), builder->CreateSub(MakeInt32(-1), builder->CreateZExtOrTrunc(index, inttype(32))) });
			auto loadInst = MakeInvariantLoad(builder, TYPETYPE, targAddress, "typeArgument");
			return loadInst;
		}

		PWTypeArr PWStructVal::PointerToTypeArgs(NomBuilder& builder)
		{
			return PWTypeArr(builder->CreateGEP(StructuralValueHeader::GetLLVMType(), wrapped, { MakeInt32(0), MakeInt32(StructuralValueHeaderFields::TypeArgs) }));
		}

		PWCastData PWStructVal::ReadCastData(NomBuilder& builder)
		{
			auto loadInst = MakeLoad(builder, StructuralValueHeader::GetLLVMType(), wrapped, MakeInt32(StructuralValueHeaderFields::CastData), "castData", AtomicOrdering::Acquire);
			return PWCastData(loadInst);
		}

		llvm::Value* PWStructVal::WriteCastDataCMPXCHG(NomBuilder& builder, PWCastData olddata, PWCastData newdata)
		{
			auto argPtr = builder->CreateGEP(StructuralValueHeader::GetLLVMType(), wrapped, { MakeInt32(0), MakeInt32(StructuralValueHeaderFields::CastData) });
			return builder->CreateAtomicCmpXchg(argPtr, olddata, newdata, llvm::MaybeAlign(8), AtomicOrdering::AcquireRelease, AtomicOrdering::Acquire);
		}


		
	}
}