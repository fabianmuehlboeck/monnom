#include "RTStructType.h"
#include "Context.h"
#include "RTTypeHead.h"

namespace Nom
{
	namespace Runtime
	{
		llvm::StructType* RTStructType::GetLLVMType()
		{
			static llvm::StructType* stype = llvm::StructType::create(LLVMCONTEXT, { RTTypeHead::GetLLVMType() }, "NOM_RT_StructType");
			return stype;
		}
	}
}