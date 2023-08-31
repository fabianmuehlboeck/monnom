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

		void LoadIntConstantInstruction::Compile(NomBuilder &builder, CompileEnv* env, [[maybe_unused]] size_t lineno)
		{
			RegisterValue(env, RTValue::GetValue(builder, builder->getInt64(Value), NomIntClass::GetInstance()->GetType()));
		}
		void LoadIntConstantInstruction::Print([[maybe_unused]] bool resolve)
		{
			cout << "IntConstant " << std::dec << Value;
			cout << " -> #" << std::dec << WriteRegister;
			cout << "\n";
		}
		void LoadIntConstantInstruction::FillConstantDependencies([[maybe_unused]] NOM_CONSTANT_DEPENCENCY_CONTAINER& result)
		{
		}
	}
}
