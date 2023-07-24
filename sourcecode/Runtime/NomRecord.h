#pragma once
#include <vector>
#include "NomCallable.h"
#include "NomTypeDecls.h"

namespace Nom
{
	namespace Runtime
	{
		class NomField;
		class NomRecordMethod;
		class NomRecordField;
		class NomSubstitutionContext;
		class NomRecord : public NomCallableLoaded
		{
		public:
			const ConstantID StructID;
			std::vector<NomRecordField*> Fields;
			std::vector<NomRecordMethod*> Methods;
			const RegIndex EndArgRegisterCount;

			NomRecord(ConstantID structID, NomMemberContext* context, ConstantID closureTypeParams, RegIndex registerCount, RegIndex endargregcount, ConstantID initializerArgTypes);
			virtual ~NomRecord() override = default;

			// Inherited via NomCallable
			virtual llvm::Type* GetLLVMType() const;
			virtual llvm::Function* createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const override;
			virtual NomTypeRef GetReturnType(const NomSubstitutionContext *context) const override;
			const NomField* GetField(NomStringRef name) const;
			NomRecordMethod* AddMethod(std::string& name, std::string& qname, ConstantID typeParameters, ConstantID returnType, ConstantID argTypes, RegIndex regcount);
			NomRecordField* AddField(const ConstantID name, const ConstantID type, bool isReadOnly, RegIndex valueRegister);
			llvm::Function* GetDynamicFieldLookup(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const;
			llvm::Function* GetDynamicFieldStore(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const;
			llvm::Constant* GetInterfaceTableLookup(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const;
			llvm::Constant* GetDynamicDispatcherLookup(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage, llvm::StructType* stype) const;
			virtual bool GetHasRawInvoke() const;
			// Inherited via NomCallableLoaded
			virtual llvm::FunctionType* GetLLVMFunctionType(const NomSubstitutionContext* context = nullptr) const override;
			virtual llvm::ArrayRef<NomTypeParameterRef> GetArgumentTypeParameters() const override;

			virtual void PushDependencies(std::set<ConstantID>& set) const override;
		};
	}
}
