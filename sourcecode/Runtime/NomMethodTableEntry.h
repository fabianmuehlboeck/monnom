#pragma once
#include "AvailableExternally.h"
#include "Defs.h"

namespace Nom
{
	namespace Runtime
	{
		class NomMethod;
		class NomCallableVersion;
		class NomMethodTableEntry
		{
		public:
			NomMethodTableEntry(const NomMethod* method, llvm::FunctionType* functionType, size_t offset);

			const NomMethod* const Method;
			const NomCallableVersion* const CallableVersion;

			const size_t Offset;
			bool PerfectOverrideMatch(NomMethodTableEntry* other, TypeList outerSubstitutions);

		};
	}
}
