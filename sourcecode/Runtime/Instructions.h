#pragma once

#include "variant.h"

#include "instructions/ReturnInstruction.h"
#include "instructions/NoopInstruction.h"


namespace Nom
{
	namespace Runtime
	{
		typedef variant<NoopInstruction, ReturnInstruction> Instruction;
	}
}