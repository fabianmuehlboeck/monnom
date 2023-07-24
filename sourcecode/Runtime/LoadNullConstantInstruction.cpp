#include "LoadNullConstantInstruction.h"
#include <iostream>
#include "NullClass.h"
#include "NomClassType.h"

using namespace std;

namespace Nom
{
	namespace Runtime
	{
		void LoadNullConstantInstruction::Compile([[maybe_unused]] NomBuilder& builder, [[maybe_unused]] CompileEnv* env, [[maybe_unused]] size_t lineno)
		{
			RegisterValue(env, NomValue(
				NomNullObject::GetInstance()->GetLLVMElement(*builder->GetInsertBlock()->getParent()->getParent()),
				NomNullClass::GetInstance()->GetType(), false));
		}
		void LoadNullConstantInstruction::Print([[maybe_unused]] bool resolve)
		{
			cout << "LoadNull ";
			cout << " -> #" << std::dec << WriteRegister;
			cout << "\n";
		}
		void LoadNullConstantInstruction::FillConstantDependencies([[maybe_unused]] NOM_CONSTANT_DEPENCENCY_CONTAINER& result)
		{
		}
	}
}
