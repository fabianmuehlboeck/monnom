#include "NoopInstruction.h"
#include <iostream>

using namespace Nom::Runtime;
using namespace std;
void Nom::Runtime::NoopInstruction::Print([[maybe_unused]] bool resolve)
{
	cout << "Noop\n";
}

void Nom::Runtime::NoopInstruction::FillConstantDependencies([[maybe_unused]] NOM_CONSTANT_DEPENCENCY_CONTAINER& result)
{
}
