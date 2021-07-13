#pragma once
#include <string>
#include "StringDict.h"

namespace Nom
{
	namespace Runtime
	{
		class NomClass;
		class NomInterface;
		class NomModule;
		class NomProgram;
		class ComponentSource
		{
		protected:
			NomStringDict<const NomClass *> classes;
			NomStringDict<const NomInterface *> interfaces;

			virtual const NomClass * LoadClass(NomStringRef name, NomModule * mod) = 0;
			virtual const NomInterface * LoadInterface(NomStringRef name, NomModule * mod) = 0;
		public:
			NomProgram * const Program;
			ComponentSource(NomProgram * program);
			virtual ~ComponentSource();

			const NomClass * GetClass(NomStringRef name, NomModule * mod)
			{
				auto res = classes.find(name);
				if (res == classes.end())
				{
					const NomClass * const cls = LoadClass(name, mod);
					classes[name] = cls;
					return cls;
				}
				return res->second;
			}
			const NomInterface * GetInterface(NomStringRef name, NomModule * mod)
			{
				auto res = interfaces.find(name);
				if (res == interfaces.end())
				{
					const NomInterface * const iface = LoadInterface(name, mod);
					interfaces[name] = iface;
					return iface;
				}
				return res->second;
			}
		};

	}
}
