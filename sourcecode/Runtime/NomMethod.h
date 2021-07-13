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
			int offset = -1;
			bool offsetSet = false;
		protected:
			NomMethod() {}
			NomMethod(NomMethod& m) = delete;
			NomMethod(const NomMethod& m) = delete;
			NomMethod(NomMethod&& m) = delete;
			NomMethod(const NomMethod&& m) = delete;
		public:
			virtual ~NomMethod() override = default;

			int GetIMTIndex() const;

			bool IsOffsetSet();
			bool SetOffset(int offset);
			int GetOffset() const;
			bool Overrides(const NomMethod* other) const;
			virtual bool IsFinal() const = 0;

			virtual llvm::FunctionType* GetRawInvokeLLVMFunctionType(const NomSubstitutionContext* context = nullptr) const;
			virtual llvm::FunctionType* GetLLVMFunctionType(const NomSubstitutionContext* context = nullptr) const override;
			virtual const NomInterface* GetContainer() const = 0;
		};

		class NomMethodLoaded : public virtual NomMethod, public NomCallableLoaded
		{
		private:
			//mutable llvm::ArrayRef<NomTypeRef> argumentTypes = llvm::ArrayRef<NomTypeRef>((NomTypeRef*)nullptr, (size_t)0);
			mutable NomTypeRef returnTypeBuf = nullptr;
			//const std::string name;
			///*const NomTypeRef returnType;
			//const TypeList arguments;
			const ConstantID returnType;
			const bool isFinal;
			//const ConstantID argTypes;
			//mutable llvm::Function *compiled;
			//const int regcount = 0;
			//std::vector<NomInstruction *> instructions;
		public:
			const NomInterface * const Container;
			NomMethodLoaded(const NomInterface * container, const std::string &name, const std::string &qname, const ConstantID returnType, const ConstantID argTypes, RegIndex regcount, ConstantID typeParameters, bool isFinal);
			/*NomMethod(const std::string &name, const NomType * const returnType, const TypeList &arguments) : name(name), returnType(returnType), arguments(arguments)
			{

			}*/
			virtual ~NomMethodLoaded() override = default;
			// Inherited via NomCallable
			virtual llvm::Function* createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const override;
			//std::string GetSymbolName() const;
			//virtual void Compile(NomBuilder &builder, llvm::Module *mod);
			//const NomType * const GetReturnType() const {
			//	return NomConstants::GetType(returnType);
			//}
			//const TypeList GetArgumentTypes() const {
			//	return NomConstants::GetTypeList(argTypes)->GetTypeList();
			//}


			//const std::string GetName() const
			//{
			//	return name;
			//}

			//llvm::FunctionType *GetLLVMFunctionType() const
			//{
			//	return GetType();
			//}
			//llvm::Function *GetLLVMFunction(llvm::Module *mod) const;

			//void AddInstruction(NomInstruction * instruction)
			//{
			//	this->instructions.push_back(instruction);
			//}

			virtual bool IsFinal() const override { return isFinal; }

			// Inherited via NomCallable
			virtual NomTypeRef GetReturnType(const NomSubstitutionContext* context) const override;

			// Inherited via NomMethod
			virtual llvm::ArrayRef<NomTypeParameterRef> GetArgumentTypeParameters() const override;
			virtual const NomInterface* GetContainer() const override;
			//virtual TypeList GetArgumentTypes(const NomSubstitutionContext* context) const override;
			//virtual int GetArgumentCount() const override;

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
			/*NomMethod(const std::string &name, const NomType * const returnType, const TypeList &arguments) : name(name), returnType(returnType), arguments(arguments)
			{

			}*/
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