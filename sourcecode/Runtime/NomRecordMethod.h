#pragma once
#include "NomCallable.h"

namespace Nom
{
	namespace Runtime
	{
		class NomRecord;
		class NomInstruction;
		class NomRecordMethod : public NomCallableLoaded
		{
		public:
			const NomRecord* Container;
			ConstantID ReturnType;

			NomRecordMethod(const NomRecord* container, std::string& name, std::string& qname, ConstantID typeParameters, ConstantID returnType, ConstantID argTypes, RegIndex regcount);
			virtual ~NomRecordMethod() override = default;
			// Inherited via NomCallable
			virtual llvm::Function* createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const override;
			virtual NomTypeRef GetReturnType(const NomSubstitutionContext* context) const override;
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
