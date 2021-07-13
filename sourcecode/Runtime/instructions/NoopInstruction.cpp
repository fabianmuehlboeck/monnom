#include "NoopInstruction.h"
#include <iostream>

using namespace Nom::Runtime;
using namespace std;
void Nom::Runtime::NoopInstruction::Print(bool resolve)
{
	cout << "Noop\n";
}

void Nom::Runtime::NoopInstruction::FillConstantDependencies(NOM_CONSTANT_DEPENCENCY_CONTAINER& result)
{
}
