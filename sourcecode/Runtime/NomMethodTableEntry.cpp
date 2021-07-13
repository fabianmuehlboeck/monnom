#include "NomMethodTableEntry.h"
#include "NomMethod.h"
#include "NomCallableVersion.h"

using namespace llvm;
using namespace std;
namespace Nom
{
	namespace Runtime
	{
		NomMethodTableEntry::NomMethodTableEntry(const NomMethod* method, llvm::FunctionType* functionType, int offset) : Method(method), CallableVersion(method->GetVersion(functionType)), Offset(offset)
		{
		}
		bool NomMethodTableEntry::PerfectOverrideMatch(NomMethodTableEntry* other, TypeList outerSubstitutions)
		{
			return false;
		}
		//llvm::Function* NomMethodTableEntry::createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const
		//{
		//	return nullptr;
		//}
		//llvm::Function* NomMethodTableEntry::findLLVMElement(llvm::Module& mod) const
		//{
		//	return nullptr;
		//}
	}
}