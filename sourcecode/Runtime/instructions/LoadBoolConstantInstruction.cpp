#include "LoadBoolConstantInstruction.h"
#include <iostream>
#include "../BoolClass.h"
#include "../NomValue.h"
#include "../NomClassType.h"

using namespace llvm;
using namespace std;
namespace Nom
{
	namespace Runtime
	{
		LoadBoolConstantInstruction::LoadBoolConstantInstruction(RegIndex reg, bool value) : NomValueInstruction(reg, OpCode::LoadBoolConstant), Value(value)
		{

		}
		void LoadBoolConstantInstruction::Compile([[maybe_unused]] NomBuilder& builder, CompileEnv* env, [[maybe_unused]] size_t lineno)
		{
			if (Value)
			{
				RegisterValue(env, NomValue(NomBoolObjects::GetTrue(*env->Module), NomBoolClass::GetInstance()->GetType(), false));
			}
			else
			{
				RegisterValue(env, NomValue(NomBoolObjects::GetFalse(*env->Module), NomBoolClass::GetInstance()->GetType(), false));
			}
		}
		void LoadBoolConstantInstruction::Print([[maybe_unused]] bool resolve)
		{

			cout << "LoadBool ";
			cout << Value;
			cout << " -> #" << std::dec << WriteRegister;
			cout << "\n";
		}
		void LoadBoolConstantInstruction::FillConstantDependencies([[maybe_unused]] NOM_CONSTANT_DEPENCENCY_CONTAINER& result)
		{
		}
	}
}
