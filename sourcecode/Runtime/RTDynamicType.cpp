#include "RTDynamicType.h"
#include "RTTypeHead.h"
#include "NomDynamicType.h"
#include "CompileHelpers.h"

using namespace llvm;
using namespace std;
namespace Nom
{
	namespace Runtime
	{
		llvm::StructType* RTDynamicType::GetLLVMType()
		{
			static llvm::StructType* stype = llvm::StructType::create(LLVMCONTEXT, { RTTypeHead::GetLLVMType() }, "NOM_RT_DynamicType");
			return stype;
		}
		llvm::Constant* RTDynamicType::CreateConstant()
		{
			return ConstantStruct::get(GetLLVMType(), RTTypeHead::GetConstant(TypeKind::TKDynamic, MakeInt(NomDynamicType::Instance().GetHashCode()), &NomDynamicType::Instance()));
		}
	}
}