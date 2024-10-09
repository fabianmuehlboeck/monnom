#include "NomModule.h"
#include "NomProgram.h"
#include "IEnumerableInterface.h"
#include "ObjectClass.h"
#include "NullClass.h"
#include "VoidClass.h"
#include "IntClass.h"
#include "FloatClass.h"
#include "BoolClass.h"
#include "StringClass.h"
#include "NomJIT.h"

namespace Nom
{
	namespace Runtime
	{

		NomModule::NomModule(NomProgram * program) : program(program)
		{
			AddInternalInterface(IEnumerableInterface::GetInstance());
			AddInternalClass(NomObjectClass::GetInstance());
			AddInternalClass(NomVoidClass::GetInstance());
			AddInternalClass(NomNullClass::GetInstance());
			AddInternalClass(NomIntClass::GetInstance());
			AddInternalClass(NomFloatClass::GetInstance());
			AddInternalClass(NomBoolClass::GetInstance());
			AddInternalClass(NomStringClass::GetInstance());
		}


		NomModule::~NomModule()
		{
		}

		std::list<const NomInterface *> &NomModule::GetInterfaces()
		{
			return Interfaces;
		}
		std::list<const NomClass *> &NomModule::GetClasses()
		{
			return Classes;
		}

		void NomModule::AddBinaries(const NativeLib* lib, const std::string basepath)
		{
			for (auto entry : lib->Binaries) {
				if (entry.OS == NATIVEOS && entry.Platform == NATIVEPLATFORM)
				{
					bool found = false;
					for (auto ne : NativeEntries) {
						if (ne.Type == entry.Type && ne.Path == basepath+"/"+entry.Path && ne.Version == entry.Version)
						{
							found = true;
							break;
						}
					}
					if (!found) {
						NativeEntries.emplace_back(entry.Type, basepath+"/"+entry.Path, entry.Platform, entry.OS, entry.Version);
					}
				}
			}
		}

		NomClassLoaded* NomModule::AddClass(ConstantID name, ConstantID typeParameters, ConstantID superClass, ConstantID superInterfaces) {
			NomClassLoaded* cls = program->AddClass(name, typeParameters, superClass, superInterfaces, nullptr);
			this->Classes.push_back(cls);
			return cls;
		}

		NomInterfaceLoaded * NomModule::AddInterface(ConstantID name, ConstantID typeParameters, ConstantID superInterfaces) {
			NomInterfaceLoaded *iface = program->AddInterface(name, typeParameters, superInterfaces, nullptr);
			this->Interfaces.push_back(iface);
			return iface;
		}
		void NomModule::AddInternalClass(const NomClassInternal* cls)
		{
			if (!cls->AddOnce())
			{
				Classes.push_back(cls);
			}
		}
		void NomModule::AddInternalInterface(const NomInterfaceInternal* iface)
		{
			if (!iface->AddOnce())
			{
				Interfaces.push_back(iface);
			}
		}
		void NomModule::EnsureDependenciesResolved()
		{
			bool allDone = false;
			while(!allDone)
			{
				allDone = true;
				for (auto iface : Interfaces)
				{
					if (!iface->EnsureDependenciesResolved(this))
					{
						allDone = false;
						break;
					}
				}
				for (auto cls : Classes)
				{
					if (!cls->EnsureDependenciesResolved(this))
					{
						allDone = false;
						break;
					}
				}
			} 
		}
	}
}