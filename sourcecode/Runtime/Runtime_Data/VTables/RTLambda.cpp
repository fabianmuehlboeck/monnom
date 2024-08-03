#include "../../Runtime_Data/VTables/RTLambda.h"
#include "../../Runtime_Data/RTDescriptor.h"
#include "../../Intermediate_Representation/Data/NomLambda.h"
#include "../../Common/CompileHelpers.h"
#include "../../Runtime_Data/RTSignature.h"
#include "../../Runtime_Data/VTables/RTVTable.h"
#include "../../Intermediate_Representation/Callables/NomPartialApplication.h"

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
			return ConstantStruct::get(GetLLVMType(), { RTVTable::CreateConstant(RTDescriptorKind::Lambda, interfaceMethodTable, dynamicDispatcherTable,fieldRead, fieldWrite, MakeInt32(1)) });
		}
	}
}