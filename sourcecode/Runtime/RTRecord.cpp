#include "RTRecord.h"
#include "Context.h"
#include "RTVTable.h"
#include "NomInterface.h"
#include "CompileHelpers.h"
#include "NomRecord.h"
#include "RTSignature.h"
#include "NomPartialApplication.h"

using namespace llvm;
using namespace std;
namespace Nom
{
	namespace Runtime
	{
		llvm::StructType* RTRecord::GetLLVMType()
		{
			static auto rtst = StructType::create(LLVMCONTEXT, "RT_NOM_StructDescriptor");
			static bool once = true;
			if (once)
			{
				once = false;
				rtst->setBody(
					{ RTVTable::GetLLVMType(),						// common VTable parts
					numtype(size_t),								// field count
					numtype(size_t)									// typearg count
					});
			}
			return rtst;
		}

		llvm::Constant* RTRecord::CreateConstant(const NomRecord *record, llvm::Function* fieldRead, llvm::Function* fieldWrite, llvm::Constant* interfaceMethodTable, llvm::Constant* dynamicDispatcherTable)
		{
			return ConstantStruct::get(GetLLVMType(), RTVTable::CreateConstant(RTDescriptorKind::Record, interfaceMethodTable, dynamicDispatcherTable, fieldRead, fieldWrite, MakeInt32(record->GetHasRawInvoke()?1:0)),  MakeInt<size_t>(record->Fields.size()), MakeInt<size_t>(record->GetTypeParametersCount()));
		}

		llvm::Value* RTRecord::GenerateReadFieldCount(NomBuilder& builder, llvm::Value* descriptor)
		{
			return MakeInvariantLoad(builder, RTRecord::GetLLVMType(), descriptor, MakeInt32(RTStructFields::FieldCount), "fieldCount", AtomicOrdering::NotAtomic);
		}
		llvm::Value* RTRecord::GenerateReadTypeArgCount(NomBuilder& builder, llvm::Value* descriptor)
		{
			return MakeInvariantLoad(builder, RTRecord::GetLLVMType(), descriptor, MakeInt32(RTStructFields::TypeArgCount), "typeArgCount", AtomicOrdering::NotAtomic);
		}
	}
}
