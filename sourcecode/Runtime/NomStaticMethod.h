#pragma once

#include "NomInstruction.h"
#include "Instructions.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Module.h"
#include "NomType.h"
#include "TypeList.h"
#include "Defs.h"
#include <vector>
#include <string>
#include "NomCallable.h"
#include "NomMemberContext.h"

namespace Nom
{
	namespace Runtime
	{
		class NomStaticMethod : public virtual NomCallable
		{
		protected:
			NomStaticMethod()
			{}
		public:
			virtual ~NomStaticMethod()
			{}

			virtual llvm::FunctionType* GetLLVMFunctionType(const NomSubstitutionContext* context = nullptr) const override;
			//bool Satisfies(const NomSubstitutionContext* context, const TypeList typeArgs, const TypeList argTypes) const;

			virtual const NomClass* GetContainer() const = 0;
		};

		class NomStaticMethodLoaded : public virtual NomStaticMethod, public NomCallableLoaded
		{
		private:
			//const std::string name;
			//const std::string qname;
			const ConstantID returnType;
			mutable NomTypeRef returnTypeBuf;
			//const int regcount;
			//const size_t typeArgCount;
			//std::vector<NomInstruction *> instructions;
			//mutable llvm::FunctionType * llvmType = nullptr;
			//mutable std::string symname = "";
			//llvm::Function *Compiled = nullptr;
		public:
			const NomClass* const Class;
			NomStaticMethodLoaded(const std::string& name, const NomClass* parent, const std::string& qname, const ConstantID returnType, const ConstantID typeArgs, const ConstantID arguments, const int regcount, bool declOnly = false);
			virtual ~NomStaticMethodLoaded() override = default;

			//llvm::Function *GetCompiled()
			//{
			//	return Compiled;
			//}

			//void AddInstruction(NomInstruction * instruction)
			//{
			//	instructions.push_back(instruction);
			//}

			//llvm::FunctionType *GetLLVMFunctionType() const;

			//llvm::Function *GetLLVMFunction(llvm::Module *mod) const;

			virtual llvm::Function * createLLVMElement(llvm::Module &mod, llvm::GlobalValue::LinkageTypes linkage) const override;

			

			// Inherited via NomCallable
			//virtual int GetArgumentCount() const override;


			// Inherited via NomStaticMethod
			virtual NomTypeRef GetReturnType(const NomSubstitutionContext* context) const override;

			//virtual TypeList GetArgumentTypes(const NomSubstitutionContext* context) const override;

			virtual llvm::ArrayRef<NomTypeParameterRef> GetArgumentTypeParameters() const override;


			// Inherited via NomStaticMethod
			virtual const NomClass* GetContainer() const override;

			/*const int GetArgumentCount() const
			{
				return GetArgumentTypes()->size();
			}*/
			virtual void PushDependencies(std::set<ConstantID>& set) const override
			{
				NomCallableLoaded::PushDependencies(set);
				set.insert(returnType);
			}
		};


		class NomStaticMethodInternal : public virtual NomStaticMethod, public NomCallableInternal
		{
		private:
			NomTypeRef returnType = nullptr;
		public:
			const NomClass* const Container;
			NomStaticMethodInternal(const std::string& name, const std::string& qname, const NomClass* cls);
			virtual ~NomStaticMethodInternal() override;

			// Inherited via NomCallable
			virtual llvm::Function* createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const override;

			// Inherited via NomConstructor
			virtual NomTypeRef GetReturnType(const NomSubstitutionContext* context) const override;
			virtual llvm::ArrayRef<NomTypeParameterRef> GetArgumentTypeParameters() const override;
			virtual const NomClass* GetContainer() const override;


			void SetReturnType(NomTypeRef returnType);
		};

	}
}
