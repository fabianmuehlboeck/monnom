#include "NomMethodTableEntry.h"
#include "NomMethod.h"
#include "NomCallableVersion.h"

using namespace llvm;
using namespace std;
namespace Nom
{
	namespace Runtime
	{
		NomMethodTableEntry::NomMethodTableEntry(const NomMethod* method, llvm::FunctionType* functionType, size_t offset) : Method(method), CallableVersion(method->GetVersion(functionType)), Offset(offset)
		{
		}
		bool NomMethodTableEntry::PerfectOverrideMatch([[maybe_unused]] NomMethodTableEntry* other, [[maybe_unused]] TypeList outerSubstitutions)
		{
			return false;
		}
	}
}
