#pragma once
#include "NomCallable.h"
#include "GloballyNamed.h"
#include <vector>
#include "llvm/IR/Function.h"

namespace Nom
{
	namespace Runtime
	{
		class NomClass;

		class NomConstructor : public virtual NomCallable, public GloballyNamed
		{
		protected:
			NomConstructor();
		public:
			virtual ~NomConstructor() override = default;

			virtual const NomClass* GetClass() const = 0;

			virtual llvm::FunctionType* GetLLVMFunctionType(const NomSubstitutionContext* context = nullptr) const override;
		};

		class NomConstructorLoaded : public NomConstructor, public NomCallableLoaded
		{
		private:
			mutable NomTypeRef returnTypeBuf;
			std::vector<NomInstruction *> preInstructions;
			std::vector<RegIndex> superConstructorArgs;
		public:
			const NomClass * const Class;
			NomConstructorLoaded(const NomClass * cls, const std::string &name, const std::string &qname, const ConstantID arguments, const RegIndex regcount, const ConstantID typeArgs, bool declOnly = false, bool cppWrapper=false);
			virtual ~NomConstructorLoaded() override;

			void AddPreInstruction(NomInstruction *instr)
			{
				preInstructions.push_back(instr);
				if (instr->GetOpCode() == OpCode::PhiNode)
				{
					phiNodes.push_back((PhiNode*)instr);
				}
			}
			void AddSuperConstructorArgReg(RegIndex reg)
			{
				superConstructorArgs.push_back(reg);
			}
			// Inherited via NomCallable
			virtual llvm::Function * createLLVMElement(llvm::Module & mod, llvm::GlobalValue::LinkageTypes linkage) const override;

			// Inherited via NomCallable
			virtual NomTypeRef GetReturnType(const NomSubstitutionContext * context = nullptr) const override;

			virtual const NomClass* GetClass() const override
			{
				return Class;
			}

			// Inherited via NomConstructor
			virtual llvm::ArrayRef<NomTypeParameterRef> GetArgumentTypeParameters() const override;

			virtual void PushDependencies(std::set<ConstantID>& set) const override
			{
				NomCallableLoaded::PushDependencies(set);
				NOM_CONSTANT_DEPENCENCY_CONTAINER depBuf;
				for (auto inst : preInstructions)
				{
					depBuf.clear();
					inst->FillConstantDependencies(depBuf);
					for (auto cid : depBuf)
					{
						set.insert(cid);
					}
				}
			}
		};

		class NomConstructorInternal : public NomConstructor, public NomCallableInternal
		{
		public:
			const NomClass* const Container;
			NomConstructorInternal(const std::string &qname, const NomClass *cls);
			virtual ~NomConstructorInternal() override;

			// Inherited via NomCallable
			virtual llvm::Function* createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const override;

			// Inherited via NomConstructor
			virtual NomTypeRef GetReturnType(const NomSubstitutionContext* context) const override;
			virtual llvm::ArrayRef<NomTypeParameterRef> GetArgumentTypeParameters() const override;
			virtual const NomClass* GetClass() const override;
		};

	}
}

