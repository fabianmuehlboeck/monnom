#pragma once

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
		};

	}
}
