#include "PWClass.h"
#include "Defs.h"
#include "CompileHelpers.h"

using namespace std;
using namespace llvm;
namespace Nom
{
	namespace Runtime
	{
		PWClass PWClass::FromVTable(PWVTable vt)
		{
			return PWClass(vt.wrapped);
		}
		inline PWInterface PWClass::GetInterface(NomBuilder& builder) const
		{
			return PWInterface(builder->CreateGEP(RTClass::GetLLVMType(), wrapped, llvm::ArrayRef<Value*>({ MakeInt32(0), MakeInt32(RTClassFields::RTInterface) })));
		}
	}
}