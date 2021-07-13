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

		void LoadFloatConstantInstruction::Compile(NomBuilder& builder, CompileEnv* env, int lineno)
		{
			RegisterValue(env, NomValue(ConstantFP::get(FLOATTYPE, Value), NomFloatClass::GetInstance()->GetType()));
		}
		void LoadFloatConstantInstruction::Print(bool resolve)
		{
			cout << "FloatConstant " << std::dec << Value;
			cout << " -> #" << std::dec << WriteRegister;
			cout << "\n";
		}
		void LoadFloatConstantInstruction::FillConstantDependencies(NOM_CONSTANT_DEPENCENCY_CONTAINER& result)
		{
		}
	}
}
