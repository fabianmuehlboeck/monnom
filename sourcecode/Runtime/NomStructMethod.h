#pragma once
#include "NomCallable.h"

namespace Nom
{
	namespace Runtime
	{
		class NomStruct;
		class NomInstruction;
		class NomStructMethod : public NomCallableLoaded
		{
		public:
			const NomStruct* Container;
			//ConstantID ArgumentTypes;
			ConstantID ReturnType;

			NomStructMethod(const NomStruct* container, std::string& name, std::string& qname, ConstantID typeParameters, ConstantID returnType, ConstantID argTypes, RegIndex regcount);
			virtual ~NomStructMethod() override = default;
			// Inherited via NomCallable
			virtual llvm::Function* createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const override;
			virtual NomTypeRef GetReturnType(const NomSubstitutionContext* context) const override;
			//virtual TypeList GetArgumentTypes(const NomSubstitutionContext* context) const override;
			//virtual int GetArgumentCount() const override;
			virtual llvm::FunctionType* GetLLVMFunctionType(const NomSubstitutionContext* context = nullptr) const override;

			// Inherited via NomCallableLoaded
			virtual llvm::ArrayRef<NomTypeParameterRef> GetArgumentTypeParameters() const override;
			virtual void PushDependencies(std::set<ConstantID>& set) const override
			{
				NomCallableLoaded::PushDependencies(set);
				set.insert(ReturnType);
			}
		};
	}
}