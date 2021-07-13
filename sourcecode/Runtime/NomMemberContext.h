#pragma once
#include "llvm/ADT/ArrayRef.h"
#include "Defs.h"
#include "NomSubstitutionContext.h"

namespace Nom
{
	namespace Runtime
	{

		class NomTypeVar;
		class NomTypeParameter;
		using NomTypeParameterRef = const NomTypeParameter*;

		class NomMemberContext
		{
		private:
			mutable llvm::ArrayRef<NomTypeParameterRef> allParameters = llvm::ArrayRef<NomTypeParameterRef>((NomTypeParameterRef*)nullptr, (size_t)0);
			mutable llvm::ArrayRef<NomTypeRef> allVariables = llvm::ArrayRef<NomTypeRef>((NomTypeRef*)nullptr, (size_t)0);
			mutable llvm::ArrayRef<NomTypeRef> directVariables = llvm::ArrayRef<NomTypeRef>((NomTypeRef*)nullptr, (size_t)0);
		protected:
			NomMemberContext() {}
		public:
			virtual ~NomMemberContext() {}

			NomMemberContext(NomMemberContext& other) = delete;
			NomMemberContext(const NomMemberContext& other) = delete;
			NomMemberContext(NomMemberContext&& other) = delete;
			NomMemberContext(const NomMemberContext&& other) = delete;

			const llvm::ArrayRef<NomTypeRef> GetAllTypeVariables() const;
			const llvm::ArrayRef<NomTypeParameterRef> GetAllTypeParameters() const;
			size_t GetTypeParametersCount() const;
			size_t GetTypeParametersStart() const;
			NomTypeParameterRef GetTypeParameter(int index) const;

			const llvm::ArrayRef<NomTypeRef> GetDirectTypeVariables() const;
			virtual const llvm::ArrayRef<NomTypeParameterRef> GetDirectTypeParameters() const = 0;
			virtual size_t GetDirectTypeParametersCount() const = 0;
			virtual NomTypeParameterRef GetLocalTypeParameter(int index) const = 0;

			virtual const std::string* GetSymbolName() const = 0;
			virtual const NomMemberContext* GetParent() const = 0;
		};

		class NomMemberContextLoaded : public virtual NomMemberContext
		{
		protected:
			const NomMemberContext *parent;
			const ConstantID typeParametersID;
			mutable bool initialized = false;
			mutable llvm::ArrayRef<NomTypeParameterRef> TypeParameters;
		public:
			NomMemberContextLoaded(const NomMemberContext *parent, ConstantID typeParametersID);
			virtual ~NomMemberContextLoaded() override;

			const llvm::ArrayRef<NomTypeParameterRef> GetDirectTypeParameters() const override;
			size_t GetDirectTypeParametersCount() const override;

			NomTypeParameterRef GetLocalTypeParameter(int index) const override;
			//virtual const std::string * GetSymbolName() const = 0;
			const NomMemberContext* GetParent() const override { return parent; }
		};

		class NomMemberContextList : public virtual NomMemberContext
		{
		private:
			const llvm::ArrayRef<NomTypeParameterRef> typeParameters;
			const NomMemberContext* const parent;
		public:
			NomMemberContextList(llvm::ArrayRef<NomTypeParameterRef> typeParameters, const NomMemberContext* parent = nullptr) : typeParameters(typeParameters), parent(parent)
			{

			}
			virtual ~NomMemberContextList() override = default;

			virtual const std::string* GetSymbolName() const override
			{
				throw new std::exception(); //this is not a thing you should ask of this version of MemberContext
			}

			// Inherited via NomMemberContext
			virtual const llvm::ArrayRef<NomTypeParameterRef> GetDirectTypeParameters() const override
			{
				return typeParameters;
			}

			virtual size_t GetDirectTypeParametersCount() const override
			{
				return typeParameters.size();
			}

			virtual NomTypeParameterRef GetLocalTypeParameter(int index) const override
			{
				return typeParameters[index];
			}

			virtual const NomMemberContext* GetParent() const override
			{
				return parent;
			}


		};

		class NomMemberContextInternal : public virtual NomMemberContext
		{
		private:
			llvm::ArrayRef<NomTypeParameterRef> typeParameters = llvm::ArrayRef<NomTypeParameterRef>((NomTypeParameterRef *)nullptr, (size_t)0);
			const NomMemberContext* const parent;
		public:
			//One version of SetDirectTypeParameters must be called eventually (TypeParameters need to be able to access "this" context object to set parent)
			NomMemberContextInternal(const NomMemberContext* parent = nullptr) : parent(parent)
			{

			}
			virtual ~NomMemberContextInternal() override = default;

			void SetDirectTypeParameters(llvm::ArrayRef<NomTypeParameterRef> typeParameters)
			{
				if (this->typeParameters.data() != nullptr || typeParameters.data()==nullptr)
				{
					throw new std::exception();
				}
				this->typeParameters = typeParameters;
			}
			//Used to set type parameters to empty array point to self
			void SetDirectTypeParameters()
			{
				this->typeParameters = llvm::ArrayRef<NomTypeParameterRef>((NomTypeParameterRef*)this, (size_t)0);
			}

			// Inherited via NomMemberContext
			virtual const llvm::ArrayRef<NomTypeParameterRef> GetDirectTypeParameters() const override
			{
				return typeParameters;
			}

			virtual size_t GetDirectTypeParametersCount() const override
			{
				return typeParameters.size();
			}

			virtual NomTypeParameterRef GetLocalTypeParameter(int index) const override
			{
				return typeParameters[index];
			}

			virtual const NomMemberContext* GetParent() const override
			{
				return parent;
			}

		};

		//class SimpleNomMemberContext : public NomMemberContext
		//{
		//private:
		//	const std::string symbolName;
		//public:
		//	SimpleNomMemberContext(const NomMemberContext* parent, ConstantID typeParametersID, const std::string &symbolName);
		//	SimpleNomMemberContext(const NomMemberContext* parent, llvm::ArrayRef<NomTypeVarRef> typeParameters, const std::string& symbolName);
		//	// Inherited via NomMemberContext
		//	virtual const std::string* GetSymbolName() const override;
		//};

	}
}
