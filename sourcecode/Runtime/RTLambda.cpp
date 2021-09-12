#include "RTLambda.h"
#include "RTDescriptor.h"
#include "NomLambda.h"
#include "CompileHelpers.h"
#include "RTSignature.h"
#include "RTVTable.h"
#include "NomPartialApplication.h"

using namespace llvm;
using namespace std;
namespace Nom
{
	namespace Runtime
	{
		llvm::StructType* RTLambda::GetLLVMType()
		{
			static StructType* ltype = StructType::create(LLVMCONTEXT, "RT_NOM_Lambda");
			static bool once = true;
			if (once)
			{
				once = false;
				ltype->setBody(
					RTVTable::GetLLVMType()		//common parts of VTables
				);
			}
			return ltype;
		}
		llvm::Constant* RTLambda::CreateConstant(const NomLambda* lambda, llvm::Constant* interfaceMethodTable, llvm::Constant* dynamicDispatcherTable, llvm::Function* fieldRead, llvm::Function* fieldWrite)
		{
			return ConstantStruct::get(GetLLVMType(), { RTVTable::CreateConstant(RTDescriptorKind::Lambda, interfaceMethodTable, dynamicDispatcherTable,fieldRead, fieldWrite) });
		}
	}
}