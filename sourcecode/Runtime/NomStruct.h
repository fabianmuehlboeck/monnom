#pragma once
#include <vector>
#include "NomCallable.h"
#include "NomTypeDecls.h"
#include "NomDescriptor.h"

namespace Nom
{
	namespace Runtime
	{
		class NomField;
		class NomStructMethod;
		class NomStructField;
		class NomSubstitutionContext;
		class NomStruct : public NomCallableLoaded, public NomDescriptor
		{
		private:
			//mutable std::vector<NomTypeRef> argtypes;
		public:
			const ConstantID StructID;
			//const ConstantID InitializerArgTypes;
			std::vector<NomStructField*> Fields;
			std::vector<NomStructMethod*> Methods;
			const RegIndex EndArgRegisterCount;

			NomStruct(ConstantID structID, NomMemberContext* context, ConstantID closureTypeParams, RegIndex registerCount, RegIndex endargregcount, ConstantID initializerArgTypes);
			virtual ~NomStruct() override = default;

			// Inherited via NomCallable
			virtual llvm::Function* createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const override;
			virtual NomTypeRef GetReturnType(const NomSubstitutionContext *context) const override;
			//virtual TypeList GetArgumentTypes(const NomSubstitutionContext* context) const override;
			//virtual int GetArgumentCount() const override;
			const NomField* GetField(NomStringRef name) const;
			NomStructMethod* AddMethod(std::string& name, std::string& qname, ConstantID typeParameters, ConstantID returnType, ConstantID argTypes, RegIndex regcount);
			NomStructField* AddField(const ConstantID name, const ConstantID type, bool isReadOnly, RegIndex valueRegister);
			static llvm::FunctionType* GetDynamicFieldLookupType();
			llvm::Function* GetDynamicFieldLookup(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const;
			static llvm::FunctionType* GetDynamicFieldStoreType();
			llvm::Function* GetDynamicFieldStore(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const;
			static llvm::StructType* GetDynamicDispatcherLookupResultType();
			static llvm::FunctionType* GetDynamicDispatcherLookupType();
			llvm::Function* GetDynamicDispatcherLookup(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const;
			void GenerateDictionaryEntries(llvm::Module& mod) const;
			virtual bool GetHasRawInvoke() const;
			// Inherited via NomCallableLoaded
			virtual llvm::FunctionType* GetLLVMFunctionType(const NomSubstitutionContext* context = nullptr) const override;
			virtual llvm::ArrayRef<NomTypeParameterRef> GetArgumentTypeParameters() const override;

			virtual void PushDependencies(std::set<ConstantID>& set) const override;
		};
	}
}