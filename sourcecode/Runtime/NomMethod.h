#pragma once

#include "NomInstruction.h"
#include "TypeList.h"
#include <string>
#include "llvm/IR/Function.h"
#include "NomType.h"
#include <vector>
#include <memory>
#include "Defs.h"
#include "NomString.h"
#include "NomConstants.h"
#include "NomCallable.h"

namespace Nom
{
	namespace Runtime
	{

		class NomMethod : public virtual NomCallable
		{
		private:
			size_t offset = 0;
			bool offsetSet = false;
		protected:
			NomMethod() {}
			NomMethod(NomMethod& m) = delete;
			NomMethod(const NomMethod& m) = delete;
			NomMethod(NomMethod&& m) = delete;
			NomMethod(const NomMethod&& m) = delete;
		public:
			virtual ~NomMethod() override = default;

			size_t GetIMTIndex() const;

			bool IsOffsetSet();
			bool SetOffset(size_t offset);
			size_t GetOffset() const;
			bool Overrides(const NomMethod* other) const;
			virtual bool IsFinal() const = 0;

			virtual llvm::FunctionType* GetRawInvokeLLVMFunctionType(const NomSubstitutionContext* context = nullptr) const;
			virtual llvm::FunctionType* GetLLVMFunctionType(const NomSubstitutionContext* context = nullptr) const override;
			virtual const NomInterface* GetContainer() const = 0;
		};

		class NomMethodLoaded : public virtual NomMethod, public NomCallableLoaded
		{
		private:
			mutable NomTypeRef returnTypeBuf = nullptr;
			const ConstantID returnType;
			const bool isFinal;
		public:
			const NomInterface * const Container;
			NomMethodLoaded(const NomInterface * container, const std::string &name, const std::string &qname, const ConstantID returnType, const ConstantID argTypes, RegIndex regcount, ConstantID typeParameters, bool isFinal);
			virtual ~NomMethodLoaded() override = default;
			// Inherited via NomCallable
			virtual llvm::Function* createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const override;

			virtual bool IsFinal() const override { return isFinal; }

			// Inherited via NomCallable
			virtual NomTypeRef GetReturnType(const NomSubstitutionContext* context) const override;

			// Inherited via NomMethod
			virtual llvm::ArrayRef<NomTypeParameterRef> GetArgumentTypeParameters() const override;
			virtual const NomInterface* GetContainer() const override;

			virtual void PushDependencies(std::set<ConstantID>& set) const override
			{
				NomCallableLoaded::PushDependencies(set);
				set.insert(returnType);
			}
		};

		class NomMethodDeclLoaded : public virtual NomMethod, public virtual NomCallableLoaded
		{
		private:
			mutable NomTypeRef returnType;
			const ConstantID returnTypeID;
		public:
			NomMethodDeclLoaded(const NomInterface* container, const std::string& name, const std::string& qname, const ConstantID returnType, const ConstantID argTypes, ConstantID typeParameters);
			virtual ~NomMethodDeclLoaded() override = default;

			// Inherited via NomCallable
			virtual NomTypeRef GetReturnType(const NomSubstitutionContext* context) const override;
			virtual llvm::Function* createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const override;
			virtual bool IsFinal() const override { return false; }

			virtual void PushDependencies(std::set<ConstantID>& set) const override
			{
				NomCallableLoaded::PushDependencies(set);
				set.insert(returnTypeID);
			}
		};

		class NomMethodInternal : public virtual NomMethod, public NomCallableInternal
		{
		private:
			NomTypeRef returnType = nullptr;
			const bool isFinal;
		public:
			const NomInterface* const Container;
			NomMethodInternal(const NomInterface* container, const std::string& name, const std::string& qname, bool isFinal);
			virtual ~NomMethodInternal() override = default;

			void SetReturnType(NomTypeRef returnType);
			virtual bool IsFinal() const override { return isFinal; }
			// Inherited via NomCallable
			virtual NomTypeRef GetReturnType(const NomSubstitutionContext* context) const override;

			virtual llvm::Function* createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const override;

			// Inherited via NomMethod
			virtual llvm::ArrayRef<NomTypeParameterRef> GetArgumentTypeParameters() const override;
			virtual const NomInterface* GetContainer() const override;
		};

		class NomMethodDeclInternal : public virtual NomMethod, public NomCallableInternal
		{
		private:
			NomTypeRef returnType = nullptr;
		public:
			const NomInterface* const Container;

			NomMethodDeclInternal(NomInterface* container, const std::string& name, const std::string& qname);
			virtual ~NomMethodDeclInternal() override = default;

			void SetReturnType(NomTypeRef returnType);
			// Inherited via NomCallable
			virtual llvm::Function* createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const override;

			// Inherited via NomMethod
			virtual NomTypeRef GetReturnType(const NomSubstitutionContext* context) const override;
			virtual llvm::ArrayRef<NomTypeParameterRef> GetArgumentTypeParameters() const override;
			virtual const NomInterface* GetContainer() const override;
			virtual bool IsFinal() const override { return false; }
		};

	}
}
