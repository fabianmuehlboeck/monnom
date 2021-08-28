#include "NomSubstitutionContext.h"
#include "NomMemberContext.h"
#include "NomTypeVar.h"

namespace Nom
{
	namespace Runtime
	{
		NomSubstitutionContextMemberContext::NomSubstitutionContextMemberContext(const NomMemberContext* context) : context(context)
		{

		}
		llvm::ArrayRef<NomTypeRef> NomSubstitutionContextMemberContext::GetTypeParameters() const
		{
			return context->GetAllTypeVariables();
		}
		size_t NomSubstitutionContextMemberContext::GetTypeArgumentCount() const
		{
			return context->GetTypeParametersCount();
		}
		NomTypeRef NomSubstitutionContextMemberContext::GetTypeVariable(int index) const
		{
			return context->GetAllTypeVariables()[index];
		}
		const NomSubstitutionContextList& NomSubstitutionContextList::EmptyContext()
		{
			static NomTypeRef arr = nullptr;
			static NomSubstitutionContextList nscl(TypeList(&arr, (size_t)0));
			return nscl;
		}
		NomSingleSubstitutionContext::NomSingleSubstitutionContext(NomTypeVarRef typeVar, NomTypeRef replacement): typeVar(typeVar), replacement(replacement)
		{
		}
		llvm::ArrayRef<NomTypeRef> NomSingleSubstitutionContext::GetTypeParameters() const
		{
			throw new std::exception();
		}
		size_t NomSingleSubstitutionContext::GetTypeArgumentCount() const
		{
			return typeVar->GetIndex() + 1;
		}
		NomTypeRef NomSingleSubstitutionContext::GetTypeVariable(int index) const
		{
			if (index == typeVar->GetIndex())
			{
				return replacement;
			}
			return nullptr;
		}
	}
}

