#pragma once
#include "Defs.h"
#include "llvm/IR/Function.h"
#include "TypeList.h"
#include "NomTypeDecls.h"
#include "NomConstants.h"
#include "llvm/ADT/StringRef.h"
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

			virtual NomTypeRef GetReturnType(const NomSubstitutionContext* context) const = 0;
			virtual TypeList GetArgumentTypes(const NomSubstitutionContext *context) const = 0;
			virtual int GetArgumentCount() const = 0;
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
					return NomSignature(GetArgumentTypes(context), GetReturnType(context));
				}
				if (!signatureInitialized)
				{
					signature = NomSignature(GetArgumentTypes(context), GetReturnType(context));
					signatureInitialized = true;
				}
				return signature;
			}
			llvm::Function* GetLLVMFunction(llvm::Module* mod) const {
				return this->GetLLVMElement(*mod);
			}
			// Inherited via AvailableExternally
			virtual llvm::Function* findLLVMElement(llvm::Module& mod) const override;
			NomCallableVersion* GetVersion(llvm::FunctionType* ft) const;
		};

		class NomCallableLoaded : public virtual NomCallable, public NomMemberContextLoaded
		{
		private:
			mutable TypeList argTypes = TypeList((NomTypeRef*)nullptr, (size_t)0);
		protected:
			//const NomMemberContext *parent;
			const bool declOnly;
			const bool cppWrapper;
			const std::string name;
			const std::string qname;
			const RegIndex regcount;
			//const ConstantID typeArgs;
			const ConstantID argTypesID;

			const std::vector<NomInstruction *> * GetInstructions() const {
				return &instructions;
			}

		private:
			std::vector<NomInstruction *> instructions;
			//bool typeVariablesInitialized = false;
			//llvm::ArrayRef<NomTypeVar *> typeParameters;
		
		public:

			NomCallableLoaded(const std::string& name, const NomMemberContext* parent, const std::string& qname, const RegIndex regcount, const ConstantID typeArgs, const ConstantID argTypes, bool declOnly = false, bool cppWrapper = false);
			//NomCallableLoaded(const std::string& name, const NomMemberContext* parent, const std::string& qname, const RegIndex regcount, const llvm::ArrayRef<NomTypeVarRef> typeParameters, bool declOnly = false, bool cppWrapper = false);
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
					phiNodes.push_back((PhiNode*)instruction);
				}
			}


			virtual TypeList GetArgumentTypes(const NomSubstitutionContext* context) const override;
			virtual int GetArgumentCount() const override;

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
			TypeList argTypes = TypeList((NomTypeRef*)nullptr, (size_t)0);
			const std::string name;
			const std::string qname;
		protected:
			NomCallableInternal(const std::string &name, const std::string &qname, const NomMemberContext *parent = nullptr) : NomMemberContextInternal(parent), name(name), qname(qname) {}
		public:
			virtual ~NomCallableInternal() override {}

			void SetArgumentTypes(TypeList argTypes);
			void SetArgumentTypes();
			virtual TypeList GetArgumentTypes(const NomSubstitutionContext* context) const override;
			virtual int GetArgumentCount() const override;

			const std::string& GetName() const override
			{
				return name;
			}
			const std::string& GetQName() const override
			{
				return qname;
			}
		};

		//template<class T>
		//class NomLoadedReturnType
		//{
		//protected:
		//	const ConstantID returnType;
		//public:
		//	NomLoadedReturnType(const ConstantID returnType) : returnType(returnType) {}
		//	~NomLoadedReturnType() {}

		//	NomTypeRef GetReturnType() const override {
		//		return NomConstants::GetType((T*)this, returnType);
		//	}
		//};

		//template<class T>
		//class NomLoadedArgumentTypes
		//{
		//
		//public:
		//	NomLoadedArgumentTypes(const ConstantID argumentTypes) : argumentTypes(argumentTypes) {}
		//	~NomLoadedArgumentTypes() {}
		//protected:
		//	const ConstantID argumentTypes;
		//public:
		//	TypeList GetArgumentTypes() const override {
		//		return NomConstants::GetTypeList(argumentTypes)->GetTypeList((T*)this);
		//	}

		//	int GetArgumentCount() const override
		//	{
		//		return GetArgumentTypes().size();
		//	}
		//};
	}
}