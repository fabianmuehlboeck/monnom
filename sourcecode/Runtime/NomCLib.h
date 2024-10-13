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
#include "TypeOperations.h"
#include <algorithm>

namespace Nom {
	namespace Runtime {
		class NomCLib : public AvailableExternally<llvm::Function>
		{ 
		protected:
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
		public:
			NomTypeRef GetReturnType(const NomSubstitutionContext* context) const;
			TypeList GetArgumentTypes(const NomSubstitutionContext* context) const;
			int GetArgumentCount() const;

			const std::string* GetSymbolName() const
			{
				return &CMethodName;
			}
			static llvm::Type* GetCPointerType() 
			{
				return llvm::Type::getInt8Ty(LLVMCONTEXT)->getPointerTo();
			}
			static llvm::Type* GetLLVMCastedType(NomTypeRef type)
			{
				if (type->GetLLVMType() == REFTYPE) {
					return GetCPointerType();
				}
				else {
					return type->GetLLVMType();
				}
			}
			void createCastedReturn(NomBuilder &builder, CompileEnv* env) const
			{
				/*
				Uses the builder to create a return, based on the type of the return argument.
				Does any casting as necessary.
				*/
				if (GetReturnType(nullptr)->GetLLVMType() == REFTYPE) {
					builder->CreateRet(builder->CreateBitCast(*((*env)[0]), GetCPointerType()));
				}
				else if (GetReturnType(nullptr)->GetLLVMType() == FLOATTYPE) {
					builder->CreateRet(EnsurePackedUnpacked(builder, *((*env)[0]), FLOATTYPE));
				}
				else {
					builder->CreateRet(EnsurePackedUnpacked(builder, *((*env)[0]), INTTYPE));
				}
			}

			NomCLib(std::string CMethodName, llvm::ArrayRef<NomTypeRef> argTypes, const llvm::ArrayRef<NomTypeParameterRef> typeArgs, NomTypeRef returnType) 
			{
				this->CMethodName = CMethodName;
				this->argTypes = argTypes;
				this->typeArgs = typeArgs;
				this->returnType = returnType;
				regcount = std::max((int)(argTypes.size()), 1);
			}
			~NomCLib() {
			}		

			llvm::Function* findLLVMElement(llvm::Module& mod) const override;
			virtual llvm::Function* createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const override { return nullptr; };
			llvm::FunctionType* GetLLVMFunctionType(const NomSubstitutionContext* context = nullptr) const;
		};
		class NomCLibStatic : public NomCLib
		{
		private:
			NomInstantiationRef<const NomStaticMethod> method;
		public:
			NomCLibStatic(std::string CMethodName, NomInstantiationRef<const NomStaticMethod> method,
				llvm::ArrayRef<NomTypeRef> argTypes, const llvm::ArrayRef<NomTypeParameterRef> typeArgs, NomTypeRef returnType) 
				: NomCLib(CMethodName, argTypes, typeArgs, returnType)
			{
				this->method = method;
			}
			~NomCLibStatic() {
			}
			llvm::Function* createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const override;
		};
		class NomCLibInstance : public NomCLib
		{
		private:
			NomClassTypeRef receiver;
			NomInstantiationRef<const NomMethod> method;
		public:
			NomCLibInstance(std::string CMethodName, NomInstantiationRef<const NomMethod> method, NomClassTypeRef receiver
				, llvm::ArrayRef<NomTypeRef> argTypes, 
				const llvm::ArrayRef<NomTypeParameterRef> typeArgs, NomTypeRef returnType)
				: NomCLib(CMethodName, argTypes, typeArgs, returnType)
			{
				this->receiver = receiver;
				this->method = method;
				regcount = argTypes.size()+1;
			}
			~NomCLibInstance() {
			}

			llvm::Function* createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const override;
			llvm::FunctionType* GetLLVMFunctionType(const NomSubstitutionContext* context = nullptr) const;
		};
		class NomCLibConstructor : public NomCLib
		{
		private:
			NomInstantiationRef<NomClass> classRef;
		public:
			NomCLibConstructor(std::string CMethodName, NomInstantiationRef<NomClass> classRef,
				llvm::ArrayRef<NomTypeRef> argTypes, const llvm::ArrayRef<NomTypeParameterRef> typeArgs, NomTypeRef returnType)
				: NomCLib(CMethodName, argTypes, typeArgs, returnType)
			{
				this->classRef = classRef;
			}
			~NomCLibConstructor() {
			}
			llvm::Function* createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const override;
		};
	}
}

