#include "LoadIntConstantInstruction.h"
#include "../IntClass.h"
#include "../NomClassType.h"
#include <iostream>

using namespace std;
namespace Nom
{
	namespace Runtime
	{

		LoadIntConstantInstruction::LoadIntConstantInstruction(const uint64_t value, const RegIndex reg) : NomValueInstruction(reg, OpCode::LoadIntConstant), Value(value)
		{
		}


		LoadIntConstantInstruction::~LoadIntConstantInstruction()
		{
		}

		void LoadIntConstantInstruction::Compile(NomBuilder &builder, CompileEnv* env, int lineno)
		{
			RegisterValue(env, NomValue(builder->getInt64(Value), NomIntClass::GetInstance()->GetType()));
		}
		void LoadIntConstantInstruction::Print(bool resolve)
		{
			cout << "IntConstant " << std::dec << Value;
			cout << " -> #" << std::dec << WriteRegister;
			cout << "\n";
		}
		void LoadIntConstantInstruction::FillConstantDependencies(NOM_CONSTANT_DEPENCENCY_CONTAINER& result)
		{
		}
	}
}
