#include "NomNamed.h"
#include "NomConstants.h"
#include "NomClassType.h"
#include "NomJIT.h"
#include "NomBottomType.h"

namespace Nom
{
	namespace Runtime
	{
		NomNamedLoaded::NomNamedLoaded(const ConstantID name, ConstantID typeArgs, const NomMemberContext *parent) : NomMemberContextLoaded(parent, typeArgs), name(name)
		{
			NomNamed::Register(NomConstants::GetString(name)->GetText(), this);
		}

		NomStringRef NomNamedLoaded::GetName() const
		{
			return NomConstants::GetString(name)->GetText();
		}
		void NomNamed::Register(NomStringRef name, NomNamed * instance)
		{
			namedTypes()[name] = instance;
		}
		NomNamed *NomNamed::GetNamed(NomStringRef name)
		{
			return namedTypes()[name];
		}
		const std::string NomNamed::GetSymbolRep() const
		{
			return GetName()->ToStdString();
		}
		intptr_t NomNamed::GetRTElement() const
		{
			return (intptr_t)(void*)NomJIT::Instance().lookup("NOM_CD_" + (GetName()->ToStdString()))->getValue();
		}
		NomClassTypeRef NomNamed::GetInstantiation(NomTypeRef type) const
		{
			return type->GetClassInstantiation(this);
		}
		NomClassTypeRef NomNamed::GetType(llvm::ArrayRef<NomTypeRef> args) const {
			if (args.size() != GetTypeParametersCount())
			{
				throw new std::exception();
			}
			if (instances.count(args) == 0) {
				NomTypeRef *argsarr = (NomTypeRef*)(nmalloc(sizeof(NomTypeRef)*args.size()));
				memcpy(argsarr, args.data(), sizeof(NomTypeRef)*args.size());
				auto argsarrref = llvm::ArrayRef<NomTypeRef>(argsarr, args.size());
				instances[argsarrref] = new NomClassType(this, argsarrref);
			}
			return instances[args];
		}
		const std::string* NomNamedLoaded::GetSymbolName() const
		{
			if (namestr.empty())
			{
				namestr = NomConstants::GetString(name)->GetText()->ToStdString();
			}
			return &namestr;
		}
		const std::string* NomNamedInternal::GetSymbolName() const
		{
			return &symname;
		}
		NomNamedInternal::NomNamedInternal(NomStringRef name, const NomMemberContext* parent) : NomMemberContextInternal(parent), name(name), symname(name->ToStdString())
		{
		}
		NomStringRef NomNamedInternal::GetName() const
		{
			return name;
		}
	}
}