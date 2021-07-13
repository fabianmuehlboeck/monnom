#include "ArgumentInstruction.h"
#include <iostream>

using namespace std;
namespace Nom
{
	namespace Runtime
	{

		ArgumentInstruction::ArgumentInstruction(RegIndex reg) : NomInstruction(OpCode::Argument), Register(reg)
		{
		}


		ArgumentInstruction::~ArgumentInstruction()
		{
		}

		void ArgumentInstruction::Print(bool resolve)
		{
			cout << "Argument #" << std::dec << Register;
			cout << "\n";
		}

		void ArgumentInstruction::FillConstantDependencies(NOM_CONSTANT_DEPENCENCY_CONTAINER & result)
		{
		}

	}
}
