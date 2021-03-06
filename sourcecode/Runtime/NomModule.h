#pragma once
#include "NomInterface.h"
#include "NomClass.h"
#include <list>

namespace Nom
{
	namespace Runtime
	{
		class NomProgram;
		class NomModule
		{
		private:
			NomProgram * program;
		public:
			std::list<const NomInterface *> Interfaces;
			std::list<const NomClass *> Classes;

			std::list<const NomInterface *> &GetInterfaces();
			std::list<const NomClass *> &GetClasses();

			NomClassLoaded* AddClass(ConstantID name, const ConstantID typeParameters, ConstantID superClass, ConstantID superInterfaces);

			NomInterfaceLoaded * AddInterface(ConstantID name, const ConstantID typeParameters, ConstantID superInterfaces);

			void AddInternalClass(const NomClassInternal* cls);
			void AddInternalInterface(const NomInterfaceInternal* iface);

			void EnsureDependenciesResolved();

			NomModule(NomProgram * program);
			~NomModule();
		};

	}
}
