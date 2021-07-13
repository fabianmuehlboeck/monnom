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
			//std::vector<NomInterface *> ret;
			//for (auto ifc = Interfaces.begin(); ifc != Interfaces.end(); ifc++)
			//{
			//	ret.push_back(&(*ifc));
			//}
			//return ret;
			return Interfaces;
		}
		std::list<const NomClass *> &NomModule::GetClasses()
		{
			//std::vector<NomClass *> ret;
			//for (auto cls = Classes.begin(); cls != Classes.end(); cls++)
			//{
			//	ret.push_back(&(*cls));
			//}
			//return ret;
			return Classes;
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
			//llvm::SmallVector<const NomInterfaceInternal*, 4> ifacebuffer;
			//cls->GetInterfaceDependencies(ifacebuffer);
			//for (auto dep : ifacebuffer)
			//{
			//	AddInternalInterface(dep);
			//}
			//llvm::SmallVector<const NomClassInternal*, 4> clsbuffer;
			//cls->GetClassDependencies(clsbuffer);
			//for (auto dep : clsbuffer)
			//{
			//	AddInternalClass(dep);
			//}
			//Classes.push_back(cls);
			if (!cls->AddOnce())
			{
				Classes.push_back(cls);
			}
		}
		void NomModule::AddInternalInterface(const NomInterfaceInternal* iface)
		{
			//llvm::SmallVector<const NomInterfaceInternal*, 4> ifacebuffer;
			//iface->GetInterfaceDependencies(ifacebuffer);
			//for (auto dep : ifacebuffer)
			//{
			//	AddInternalInterface(dep);
			//}
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