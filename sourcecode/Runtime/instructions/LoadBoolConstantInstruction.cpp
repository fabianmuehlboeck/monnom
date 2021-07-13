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
		void LoadBoolConstantInstruction::Compile(NomBuilder& builder, CompileEnv* env, int lineno)
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
		void LoadBoolConstantInstruction::Print(bool resolve)
		{

			cout << "LoadBool ";
			cout << Value;
			cout << " -> #" << std::dec << WriteRegister;
			cout << "\n";
		}
		void LoadBoolConstantInstruction::FillConstantDependencies(NOM_CONSTANT_DEPENCENCY_CONTAINER& result)
		{
		}
	}
}