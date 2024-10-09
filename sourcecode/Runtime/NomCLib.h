#pragma once
#include "AvailableExternally.h"
#include "llvm/IR/Function.h"
#include "NomClass.h" 
#include "NomValueInstruction.h"
#include "instructions/CallCheckedStaticMethod.h"
#include "instructions/CallConstructor.h"
#include "instructions/CallCheckedInstanceMethod.h"
#include "VoidClass.h"
#include "BoolClass.h"
#include "IntClass.h"
#include "FloatClass.h"
#include "StringClass.h"
#include "NomClassType.h"
#include "ObjectClass.h"
#include "IComparableInterface.h"
#include "NomTypeParameter.h"
#include <algorithm>

namespace Nom {
	namespace Runtime {
		class NomCLib : public AvailableExternally<llvm::Function>
		{ 
		public:
			NomTypeRef GetReturnType(const NomSubstitutionContext* context) const;
			TypeList GetArgumentTypes(const NomSubstitutionContext* context) const;
			int GetArgumentCount() const;
			
			NomString* methodName;
			NomString* className;
			llvm::ArrayRef<NomTypeRef> argTypes; 
			llvm::ArrayRef<NomTypeParameterRef> typeArgs;
			NomTypeRef returnType;
			RegIndex regcount;
			std::string CMethodName;
			std::vector<PhiNode*> phiNodes;
			void InitializePhis(NomBuilder& builder, llvm::Function* fun, CompileEnv* env) const
			{
				for (auto& phi : phiNodes)
				{
					phi->Initialize(builder, fun, env);
				}
			}
			const std::string* GetSymbolName() const
			{
				return &CMethodName;
			}

			NomCLib(std::string CMethodName, std::string className, std::string callMethodName, 
				llvm::ArrayRef<NomTypeRef> argTypes, const llvm::ArrayRef<NomTypeParameterRef> typeArgs, NomTypeRef returnType) 
			{
				this->CMethodName = CMethodName;
				this->className = new NomString(className);
				this->methodName = new NomString(callMethodName);
				this->argTypes = argTypes;
				this->typeArgs = typeArgs;
				this->returnType = returnType;
				regcount = std::max((int)(argTypes.size()), 1);
			}
			~NomCLib() {
				delete methodName;
				delete className;
				delete& argTypes;
				delete& typeArgs;
			}		

			llvm::Function* findLLVMElement(llvm::Module& mod) const override;
			virtual llvm::Function* createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) {};
			llvm::FunctionType* GetLLVMFunctionType(const NomSubstitutionContext* context = nullptr) const;
		};
		class NomCLibStatic : public NomCLib
		{
		public:
			NomInstantiationRef<const NomStaticMethod> method;
			NomCLibStatic(std::string CMethodName, NomInstantiationRef<const NomStaticMethod> method,
				llvm::ArrayRef<NomTypeRef> argTypes, const llvm::ArrayRef<NomTypeParameterRef> typeArgs, NomTypeRef returnType) 
				: NomCLib(CMethodName, "", "", argTypes, typeArgs, returnType)
			{
				this->method = method;
			}
			~NomCLibStatic() {
				delete methodName;
				delete className;
				delete& argTypes;
				delete& typeArgs;
			}
			llvm::Function* createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const override;
		};
		class NomCLibInstance : public NomCLib
		{
		public:
			NomClassTypeRef receiver;
			NomInstantiationRef<const NomMethod> method;
			NomCLibInstance(std::string CMethodName, NomInstantiationRef<const NomMethod> method, NomClassTypeRef receiver
				, llvm::ArrayRef<NomTypeRef> argTypes, 
				const llvm::ArrayRef<NomTypeParameterRef> typeArgs, NomTypeRef returnType)
				: NomCLib(CMethodName, "", "", argTypes, typeArgs, returnType)
			{
				this->receiver = receiver;
				this->method = method;
				regcount = argTypes.size()+1;
			}
			~NomCLibInstance() {
				delete& methodName;
				delete className;
				delete& argTypes;
				delete& typeArgs;
			}

			llvm::Function* createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const override;
			llvm::FunctionType* GetLLVMFunctionType(const NomSubstitutionContext* context = nullptr) const;
		};
		class NomCLibConstructor : public NomCLib
		{
		public:
			NomInstantiationRef<NomClass> classRef;
			NomCLibConstructor(std::string CMethodName, NomInstantiationRef<NomClass> classRef,
				llvm::ArrayRef<NomTypeRef> argTypes, const llvm::ArrayRef<NomTypeParameterRef> typeArgs, NomTypeRef returnType)
				: NomCLib(CMethodName, "", "", argTypes, typeArgs, returnType)
			{
				this->classRef = classRef;
			}
			~NomCLibConstructor() {
				delete methodName;
				delete className;
				delete& argTypes;
				delete& typeArgs;
			}
			llvm::Function* createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const override;
		};

		/*
		Spec file:

		foSimple calls for static, instance, constructor


		*/
		//SPec file:
	
	}
}

