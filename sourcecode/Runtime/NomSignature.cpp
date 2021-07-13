#include "NomSignature.h"
#include "NomType.h"
#include "NomSubstitutionContext.h"

namespace Nom
{
	namespace Runtime
	{
		NomSignature::NomSignature():ReturnType(NomType::AnythingRef)
		{
		}
		NomSignature::NomSignature(const std::vector<NomTypeRef>& argTypes, const NomTypeRef returnType) : ReturnType(returnType), ArgumentTypes(argTypes)
		{
		}
		NomSignature::~NomSignature()
		{
		}
		NomSignature NomSignature::Substitute(const NomSubstitutionContext *context) const
		{
			std::vector<NomTypeRef> ntr;
			for (auto argT : ArgumentTypes)
			{
				ntr.push_back(argT->SubstituteSubtyping(context));
			}
			return NomSignature(ntr, ReturnType->SubstituteSubtyping(context));
		}
		bool NomSignature::SatisfiesArguments(const NomSignature& other, bool optimistic) const
		{
			auto i = ArgumentTypes.size();
			bool ret = other.ArgumentTypes.size() == i;
			while (ret && i > 0)
			{
				i--;
				ret = ret && (other.ArgumentTypes[i]->IsSubtype(ArgumentTypes[i], optimistic));
			}
			return ret;
		}
		bool NomSignature::Satisfies(const NomSignature &other, bool optimistic) const
		{
			bool ret = (ReturnType->IsSubtype(other.ReturnType, optimistic)) && SatisfiesArguments(other, optimistic);
			return ret;
		}
		bool NomSignature::HasPrimitiveUncertainty() const
		{
			for (auto ntr : ArgumentTypes)
			{
				if (ntr->UncertainlyPrimitive())
				{
					return true;
				}
			}
			return ReturnType->UncertainlyPrimitive();
		}
	}
}