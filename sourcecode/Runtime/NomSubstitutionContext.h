#pragma once
#include "NomTypeDecls.h"
#include "NomAlloc.h"

namespace Nom
{
	namespace Runtime
	{
		class NomMemberContext;
		class NomSubstitutionContext
		{
		protected:

			NomSubstitutionContext()
			{

			}

		public:
			virtual ~NomSubstitutionContext()
			{

			}

			virtual llvm::ArrayRef<NomTypeRef> GetTypeParameters() const = 0;
			virtual size_t GetTypeArgumentCount() const = 0;

			virtual NomTypeRef GetTypeVariable(size_t index) const = 0;
		};

		class NomSubstitutionContextCombination : public NomSubstitutionContext
		{
		protected:
			mutable llvm::ArrayRef<NomTypeRef> arr = llvm::ArrayRef<NomTypeRef>(static_cast<const NomTypeRef *>(nullptr), static_cast<size_t>(0));
			const NomSubstitutionContext* const left;
			const NomSubstitutionContext* const right;
		public:
			NomSubstitutionContextCombination(const NomSubstitutionContext* leftContext, const NomSubstitutionContext* rightContext) : left(leftContext), right(rightContext)
			{

			}
			virtual ~NomSubstitutionContextCombination() override
			{
				if (arr.data() != nullptr)
				{
					nmfree(const_cast<NomTypeRef *>(arr.data()));
				}
			}

			virtual llvm::ArrayRef<NomTypeRef> GetTypeParameters() const override
			{
				if (arr.data() == nullptr)
				{
					auto leftcount = left->GetTypeArgumentCount();
					auto rightcount = right->GetTypeArgumentCount();
					NomTypeRef* tarr = static_cast<NomTypeRef*>(nmalloc(sizeof(NomTypeRef) * (leftcount + rightcount)));
					for (decltype(leftcount) i = 0; i < leftcount + rightcount; i++)
					{
						if (i < leftcount)
						{
							tarr[i] = left->GetTypeVariable(i);
						}
						else
						{
							tarr[i] = right->GetTypeVariable(i - leftcount);
						}
					}
					arr = llvm::ArrayRef<NomTypeRef>(tarr, leftcount + rightcount);
				}
				return arr;
			}
			virtual size_t GetTypeArgumentCount() const override
			{
				return left->GetTypeArgumentCount()+right->GetTypeArgumentCount();
			}

			virtual NomTypeRef GetTypeVariable(size_t index) const override
			{
				if (index < left->GetTypeArgumentCount())
				{
					return left->GetTypeVariable(index);
				}
				return right->GetTypeVariable(index - left->GetTypeArgumentCount());
			}
		};

		class NomSubstitutionContextList : public NomSubstitutionContext
		{
		protected:

			mutable llvm::ArrayRef<NomTypeRef> TypeParameters;

		public:

			NomSubstitutionContextList(llvm::ArrayRef<NomTypeRef> typeParameters = nullptr) : TypeParameters(typeParameters)
			{

			}
			virtual ~NomSubstitutionContextList() override
			{

			}
			static const NomSubstitutionContextList& EmptyContext();
			virtual llvm::ArrayRef<NomTypeRef> GetTypeParameters() const override
			{
				return TypeParameters;
			}
			virtual size_t GetTypeArgumentCount() const override
			{
				return TypeParameters.size();
			}

			virtual NomTypeRef GetTypeVariable(size_t index) const override
			{
				return TypeParameters[static_cast<size_t>(index)];
			}
		};

		class NomSubstitutionContextMemberContext : public NomSubstitutionContext
		{
		protected:
			const NomMemberContext* context;
		public:
			NomSubstitutionContextMemberContext(const NomMemberContext* context);
			virtual ~NomSubstitutionContextMemberContext() override {}

			virtual llvm::ArrayRef<NomTypeRef> GetTypeParameters() const override;
			virtual size_t GetTypeArgumentCount() const override;

			virtual NomTypeRef GetTypeVariable(size_t index) const override;
		};

		using NomSubstitutionContextRef = const NomSubstitutionContext*;

		class NomSingleSubstitutionContext : public NomSubstitutionContext
		{
		protected:
			const NomTypeVarRef typeVar;
			const NomTypeRef replacement;

		public:
			NomSingleSubstitutionContext(NomTypeVarRef typeVar, NomTypeRef replacement);

			// Inherited via NomSubstitutionContext
			virtual llvm::ArrayRef<NomTypeRef> GetTypeParameters() const override;

			virtual size_t GetTypeArgumentCount() const override;

			virtual NomTypeRef GetTypeVariable(size_t index) const override;

		};
	}
}
