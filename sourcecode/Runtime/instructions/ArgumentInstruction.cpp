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

		void ArgumentInstruction::Print([[maybe_unused]] bool resolve)
		{
			cout << "Argument #" << std::dec << Register;
			cout << "\n";
		}

		void ArgumentInstruction::FillConstantDependencies([[maybe_unused]] NOM_CONSTANT_DEPENCENCY_CONTAINER & result)
		{
		}

	}
}
