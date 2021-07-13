#include "RTPartialAppType.h"
#include "Context.h"
#include "RTTypeHead.h"

namespace Nom
{
	namespace Runtime
	{
		llvm::StructType* RTPartialAppType::GetLLVMType()
		{
			static llvm::StructType* stype = llvm::StructType::create(LLVMCONTEXT, { RTTypeHead::GetLLVMType() }, "NOM_RT_PartialAppType");
			return stype;
		}
	}
}