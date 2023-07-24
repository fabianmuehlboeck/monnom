#include "LoadFloatConstantInstruction.h"
#include "FloatClass.h"
#include "NomClassType.h"
#include <iostream>

using namespace std;
namespace Nom
{
	namespace Runtime
	{

		LoadFloatConstantInstruction::LoadFloatConstantInstruction(const double value, const RegIndex reg) : NomValueInstruction(reg, OpCode::LoadFloatConstant), Value(value)
		{
		}


		LoadFloatConstantInstruction::~LoadFloatConstantInstruction()
		{
		}

		void LoadFloatConstantInstruction::Compile([[maybe_unused]] NomBuilder& builder, [[maybe_unused]] CompileEnv* env, [[maybe_unused]] size_t lineno)
		{
			RegisterValue(env, NomValue(ConstantFP::get(FLOATTYPE, Value), NomFloatClass::GetInstance()->GetType()));
		}
		void LoadFloatConstantInstruction::Print([[maybe_unused]] bool resolve)
		{
			cout << "FloatConstant " << std::dec << Value;
			cout << " -> #" << std::dec << WriteRegister;
			cout << "\n";
		}
		void LoadFloatConstantInstruction::FillConstantDependencies([[maybe_unused]] NOM_CONSTANT_DEPENCENCY_CONTAINER& result)
		{
		}
	}
}
