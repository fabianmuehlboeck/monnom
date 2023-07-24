#pragma once
#include "Defs.h"
PUSHDIAGSUPPRESSION
#include "llvm/IR/Function.h"
#include "llvm/ADT/StringRef.h"
POPDIAGSUPPRESSION
#include "TypeList.h"
#include "NomTypeDecls.h"
#include "NomConstants.h"
#include "NomMemberContext.h"
#include "AvailableExternally.h"
#include "instructions/PhiNode.h"
#include "NomSignature.h"
#include <map>
#include <set>

namespace Nom
{
	namespace Runtime
	{
		class NomCallableVersion;
		class NomInstruction;

		class NomCallable : public virtual NomMemberContext, public AvailableExternally<llvm::Function>
		{
		private:
			mutable bool signatureInitialized = false;
			mutable NomSignature signature;
			mutable std::map<llvm::FunctionType*, NomCallableVersion*> versions;
			mutable llvm::FunctionType* llvmType = nullptr;
			mutable std::string symname = "";
		protected:
			std::vector<PhiNode*> phiNodes;
		protected:
			NomCallable() {}
			NomCallable(NomCallable& m) = delete;
			NomCallable(const NomCallable& m) = delete;
			NomCallable(NomCallable&& m) = delete;
			NomCallable(const NomCallable&& m) = delete;


			void InitializePhis(NomBuilder& builder, llvm::Function* fun, CompileEnv* env) const
			{
				for (auto& phi : phiNodes)
				{
					phi->Initialize(builder, fun, env);
				}
			}


		public:
			virtual ~NomCallable() override {}

			virtual NomTypeRef GetReturnType(const NomSubstitutionContext* context = nullptr) const = 0;
			virtual TypeList GetArgumentTypes(const NomSubstitutionContext *context) const = 0;
			virtual size_t GetArgumentCount() const = 0;
			virtual llvm::FunctionType* GetLLVMFunctionType(const NomSubstitutionContext* context = nullptr) const = 0;
			virtual const std::string& GetName() const = 0;
			virtual const std::string& GetQName() const = 0;
			virtual llvm::ArrayRef<NomTypeParameterRef> GetArgumentTypeParameters() const = 0;
			bool Satisfies(const NomSubstitutionContext* context, const TypeList typeArgs, const TypeList argTypes) const;
			const std::string* GetSymbolName() const override;
			virtual const NomSignature Signature(const NomSubstitutionContext* context = nullptr) const
			{
				if (context != nullptr && context->GetTypeArgumentCount() > 0)
				{
					return NomSignature(GetArgumentTypeParameters(), GetArgumentTypes(context), GetReturnType(context));
				}
				if (!signatureInitialized)
				{
					signature = NomSignature(GetArgumentTypeParameters(), GetArgumentTypes(context), GetReturnType(context));
					signatureInitialized = true;
				}
				return signature;
			}
			llvm::Function* GetLLVMFunction(llvm::Module* mod) const {
				return this->GetLLVMElement(*mod);
			}
			virtual llvm::Function* findLLVMElement(llvm::Module& mod) const override;
			NomCallableVersion* GetVersion(llvm::FunctionType* ft) const;
		};

		class NomCallableLoaded : public virtual NomCallable, public NomMemberContextLoaded
		{
		private:
			mutable TypeList argTypes = TypeList(static_cast<NomTypeRef*>(nullptr), static_cast<size_t>(0));
		protected:
			const bool declOnly;
			const bool cppWrapper;
			const std::string name;
			const std::string qname;
			const RegIndex regcount;
			const ConstantID argTypesID;

			const std::vector<NomInstruction *> * GetInstructions() const {
				return &instructions;
			}

		private:
			std::vector<NomInstruction *> instructions;
		
		public:

			NomCallableLoaded(const std::string& name, const NomMemberContext* parent, const std::string& qname, const RegIndex regcount, const ConstantID typeArgs, const ConstantID argTypes, bool declOnly = false, bool cppWrapper = false);
			virtual ~NomCallableLoaded() override;

			const std::string &GetName() const override
			{
				return name;
			}
			const std::string& GetQName() const override
			{
				return qname;
			}


			void AddInstruction(NomInstruction* instruction)
			{
				instructions.push_back(instruction);
				if (instruction->GetOpCode() == OpCode::PhiNode)
				{
					phiNodes.push_back(static_cast<PhiNode*>(instruction));
				}
			}


			virtual TypeList GetArgumentTypes(const NomSubstitutionContext* context) const override;
			virtual size_t GetArgumentCount() const override;

			virtual void PushDependencies(std::set<ConstantID> &set) const
			{
				set.insert(argTypesID);
				set.insert(typeParametersID);
				NOM_CONSTANT_DEPENCENCY_CONTAINER depBuf;
				for (auto instr : instructions)
				{
					depBuf.clear();
					instr->FillConstantDependencies(depBuf);
					for (auto cid : depBuf)
					{
						set.insert(cid);
					}
				}
			}
		};

		class NomCallableInternal : public virtual NomCallable, public NomMemberContextInternal
		{
		private:
			TypeList argTypes = TypeList(static_cast<NomTypeRef*>(nullptr), static_cast<size_t>(0));
			const std::string name;
			const std::string qname;
		protected:
			NomCallableInternal(const std::string &_name, const std::string &_qname, const NomMemberContext *_parent = nullptr) : NomMemberContextInternal(_parent), name(_name), qname(_qname) {}
		public:
			virtual ~NomCallableInternal() override {}

			void SetArgumentTypes(TypeList argTypes);
			void SetArgumentTypes();
			virtual TypeList GetArgumentTypes(const NomSubstitutionContext* context) const override;
			virtual size_t GetArgumentCount() const override;

			const std::string& GetName() const override
			{
				return name;
			}
			const std::string& GetQName() const override
			{
				return qname;
			}
		};
	}
}
