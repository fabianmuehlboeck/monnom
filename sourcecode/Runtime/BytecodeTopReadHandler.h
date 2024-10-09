#pragma once
#include <string>
#include <vector>
#include "Manifest.h"

namespace Nom
{
	namespace Runtime
	{
		class NomClass;
		class NomInterface;
		class BytecodeTopReadHandler
		{
		public:
			BytecodeTopReadHandler();
			virtual ~BytecodeTopReadHandler();

			virtual void ReadClass(NomClass * cls) = 0;
			virtual void ReadInterface(NomInterface * iface) = 0;
			virtual std::tuple<const NativeLib*, const std::string> RequireBinary(std::string name) = 0;
		};

	}
}
