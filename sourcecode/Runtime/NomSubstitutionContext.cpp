#include "NomSubstitutionContext.h"
#include "NomMemberContext.h"
#include "NomTypeVar.h"

namespace Nom
{
	namespace Runtime
	{
		NomSubstitutionContextMemberContext::NomSubstitutionContextMemberContext(const NomMemberContext* _context) : context(_context)
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
		NomTypeRef NomSubstitutionContextMemberContext::GetTypeVariable(size_t index) const
		{
			return context->GetAllTypeVariables()[index];
		}
		const NomSubstitutionContextList& NomSubstitutionContextList::EmptyContext()
		{
			[[clang::no_destroy]] static NomSubstitutionContextList nscl(TypeList(static_cast<NomType**>(nullptr), static_cast<size_t>(0)));
			return nscl;
		}
		NomSingleSubstitutionContext::NomSingleSubstitutionContext(NomTypeVarRef _typeVar, NomTypeRef _replacement): typeVar(_typeVar), replacement(_replacement)
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
		NomTypeRef NomSingleSubstitutionContext::GetTypeVariable(size_t index) const
		{
			if (index == typeVar->GetIndex())
			{
				return replacement;
			}
			return nullptr;
		}
	}
}

