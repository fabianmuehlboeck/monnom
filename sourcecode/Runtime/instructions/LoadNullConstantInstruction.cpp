#include "../instructions/LoadNullConstantInstruction.h"
#include <iostream>
#include "../Library/NullClass.h"
#include "../Intermediate_Representation/Types/NomClassType.h"

using namespace std;

namespace Nom
{
	namespace Runtime
	{
		void LoadNullConstantInstruction::Compile(NomBuilder& builder, CompileEnv* env, int lineno)
		{
			RegisterValue(env, NomValue(
				NomNullObject::GetInstance()->GetLLVMElement(*builder->GetInsertBlock()->getParent()->getParent()),
				NomNullClass::GetInstance()->GetType(), false));
		}
		void LoadNullConstantInstruction::Print(bool resolve)
		{
			cout << "LoadNull ";
			cout << " -> #" << std::dec << WriteRegister;
			cout << "\n";
		}
		void LoadNullConstantInstruction::FillConstantDependencies(NOM_CONSTANT_DEPENCENCY_CONTAINER& result)
		{
		}
	}
}