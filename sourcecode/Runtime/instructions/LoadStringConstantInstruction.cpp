#include "LoadStringConstantInstruction.h"
#include "../StringClass.h"
#include "../NomClassType.h"

using namespace std;
namespace Nom
{
	namespace Runtime
	{
		void LoadStringConstantInstruction::Compile([[maybe_unused]] NomBuilder &builder, CompileEnv* env, [[maybe_unused]] size_t lineno)
		{
			env->basicBlockTerminated = false;
			RegisterValue(env, NomValue(NomConstants::GetString(this->constantID)->getObject(*(env->Module)), NomStringClass::GetInstance()->GetType()));
		}
		void LoadStringConstantInstruction::Print(bool resolve)
		{
			cout << "LoadString ";
			NomConstants::PrintConstant(constantID, resolve);
			cout << " -> #" << std::dec << WriteRegister;
			cout << "\n";
		}
		void LoadStringConstantInstruction::FillConstantDependencies(NOM_CONSTANT_DEPENCENCY_CONTAINER& result)
		{
			result.push_back(constantID);
		}
	}
}
