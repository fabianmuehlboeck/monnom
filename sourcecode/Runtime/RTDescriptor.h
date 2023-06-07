#pragma once
#include "Defs.h"
#include "llvm/IR/Constant.h"
#include "llvm/IR/Constants.h"
#include "ARTRep.h"
#include "NomBuilder.h"

#define DICTIONARYTYPE POINTERTYPE

namespace Nom
{
	namespace Runtime
	{

		/// <summary>
		/// Also bit-encoded as follows:
		/// LSB = 0 -> type arguments recorded in instance
		/// </summary>
		enum class RTDescriptorKind : unsigned char { Class = 0, Lambda = 1, OptimizedLambda = 2, Record = 3, OptimizedStruct = 4, PartialApplication = 5, MultiCast = 7 };

	}
}