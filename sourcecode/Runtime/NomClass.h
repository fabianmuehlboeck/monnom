#pragma once

#include <string>
#include <vector>
#include <list>
#include <map>
#include <unordered_map>
#include "NomStaticMethod.h"
#include "NomConstructor.h"
#include "NomMethod.h"
#include "Defs.h"
#include "NomString.h"
#include "RTClass.h"
#include "llvm/ADT/StringMap.h"
#include "NomInstantiationRef.h"
#include "llvm/ADT/SmallVector.h"
#include "NomInterface.h"
#include "DispatchDictionaryEntry.h"
#include "NomConstructor.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"
#include "NomField.h"
#include "RTDictionary.h"
#include "NomLambda.h"
#include "NomStruct.h"

namespace Nom
{
	namespace Runtime {
		class NomInterface;

		class NomClass : public virtual NomInterface
		{
		private:
			static auto& classes() {
				static std::unordered_map < NomStringRef, NomClass*, NomStringHash, NomStringEquality > classes; return classes;
			}
		protected:
			std::unordered_map<NomStringRef, DispatchDictionaryEntry, NomStringHash, NomStringEquality, BoehmAllocator<std::pair<NomStringRef, DispatchDictionaryEntry>>> Dictionary;
			std::vector<NomTypedField*> Fields;
			mutable std::vector<NomTypedField*> AllFields;
			std::vector<NomLambda*> Lambdas;
			std::vector<NomStruct*> Structs;
			NomClass()
			{

			}

			static void RegisterClass(NomStringRef name, NomClass* cls)
			{
				classes()[name] = cls;
			}

		public:
			NomClass(NomClass&) = delete;
			NomClass(const NomClass&) = delete;
			NomClass(NomClass&&) = delete;
			virtual ~NomClass() override = default;

			static NomClass* getClass(NomStringRef name)
			{
				auto search = classes().find(name);
				if (search != classes().end())
				{
					return classes()[name];
				}
				throw name;
			}
			virtual bool IsInterface() const override { return false; }

			virtual const NomInstantiationRef<NomClass> GetSuperClass(const NomSubstitutionContext* context = nullptr) const = 0;


			std::vector<NomStaticMethod*> StaticMethods;
			std::vector<NomConstructor*> Constructors;

			virtual bool FindInstantiations(NomNamed* other, RecBufferTypeList& myArgs, InstantiationList& results) const override;

			const NomField* GetField(NomStringRef name) const;
			virtual size_t GetFieldCount() const;

			void CompileLLVM(llvm::Module* mod) const override;
			void PreprocessInheritance() const override;

			NomValue GenerateConstructorCall(NomBuilder& builder, CompileEnv* env, const TypeList typeArgs, llvm::Value* objpointer, llvm::ArrayRef<NomValue> args) const;
			NomInstantiationRef<const NomStaticMethod> GetStaticMethod(NomStringRef methodName, const TypeList typeArgs, const TypeList argTypes) const;

			NomInstantiationRef<const NomConstructor> GetConstructor(const TypeList typeArgs, const TypeList argTypes) const;

		public:
			virtual llvm::GlobalVariable* GetSuperInstances(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const override;
			virtual llvm::ArrayType* GetSuperInstancesType(bool generic = true) const override;
			virtual const llvm::SmallVector<NomClassTypeRef, 16> GetSuperNameds(llvm::ArrayRef<NomTypeRef> args) const override;

			virtual llvm::Constant* GetInterfaceTableLookup(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const override;

			virtual llvm::Constant* createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const override;
			llvm::Function* GetMethodEnsureFunction(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const;
			virtual llvm::Constant* findLLVMElement(llvm::Module& mod) const override;
			static llvm::FunctionType* GetExpandoReaderType();
			llvm::Function* GetExpandoReaderFunction(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const;
			llvm::Function* GetRawInvokeFunction(llvm::Module& mod) const;
			llvm::Function* GetRawInvokeFunction(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const;

			size_t GetTypeArgOffset() const;
			size_t GenerateTypeArgInitialization(NomBuilder& builder, CompileEnv* env, llvm::Value* newObj, TypeList args) const;

			static llvm::FunctionType* GetDynamicDispatcherLookupType();
			static llvm::FunctionType* GetInterfaceTableLookupType();
			static llvm::FunctionType* GetDynamicFieldLookupType();
			static llvm::FunctionType* GetDynamicFieldStoreType();
			static llvm::StructType* GetDynamicDispatcherLookupResultType();


			void GenerateDictionaryEntries(llvm::Module& mod) const;

		protected:
			virtual llvm::Function* GetDynamicDispatcherLookup(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const;
			virtual llvm::Function* GetDynamicFieldLookup(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const;
			virtual llvm::Function* GetDynamicFieldStore(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const;
		public:
			void AddStaticMethod(NomStaticMethod* method)
			{
				StaticMethods.push_back(method);
			}
			void AddConstructor(NomConstructor* constructor)
			{
				Constructors.push_back(constructor);
			}
			void AddLambda(NomLambda* lambda)
			{
				Lambdas.push_back(lambda);
			}
			void AddStruct(NomStruct* strct)
			{
				Structs.push_back(strct);
			}
			void AddField(NomTypedField* field)
			{
				Fields.push_back(field);
			}
		};

		class NomClassLoaded : public virtual NomClass, public NomInterfaceLoaded
		{
		private:
			const ConstantID superClass;
			mutable NomInstantiationRef<NomClass> superClassRef;
			mutable std::vector<DICTKEYTYPE> dictionaryKeys;
		public:

			size_t getSize()
			{
				return 0;
			}
			NomClassLoaded(const ConstantID name, ConstantID typeArgs, ConstantID superClass, const ConstantID superInterfaces, const NomMemberContext *parent);
			virtual ~NomClassLoaded() override = default;

			virtual const NomInstantiationRef<NomClass> GetSuperClass(const NomSubstitutionContext* context = nullptr) const override;



			NomStaticMethodLoaded* AddStaticMethod(const std::string& name, const std::string& qname, const ConstantID typeArgs, const ConstantID returnType, const ConstantID arguments, const RegIndex regcount);

			NomConstructorLoaded* AddConstructor(const ConstantID arguments, const RegIndex regcount);

			NomTypedField* AddField(const ConstantID name, const ConstantID type, Visibility visibility, bool isReadOnly, bool isVolatile);

			NomLambda* AddLambda(const ConstantID lambdaID, int regcount, ConstantID closureTypeParams, ConstantID closureArguments, ConstantID typeParams, ConstantID argTypes, ConstantID returnType);
			NomStruct* AddStruct(const ConstantID structID, ConstantID closureTypeParams, RegIndex regcount, RegIndex endargregcount, ConstantID initializerArgTypes);


			virtual void PushDependencies(std::set<ConstantID>& set) const override
			{
				NomInterfaceLoaded::PushDependencies(set);
				set.insert(superClass);

				for (auto method : StaticMethods)
				{
					dynamic_cast<NomCallableLoaded*>(method)->PushDependencies(set);
				}
				for (auto cnstr : Constructors)
				{
					dynamic_cast<NomCallableLoaded*>(cnstr)->PushDependencies(set);
				}
				for (auto lambda : Lambdas)
				{
					dynamic_cast<NomCallableLoaded*>(lambda)->PushDependencies(set);
				}
				for (auto strct : Structs)
				{
					dynamic_cast<NomCallableLoaded*>(strct)->PushDependencies(set);
				}
				for (auto field : Fields)
				{
					field->PushDependencies(set);
				}
			}


		};

		class NomClassInternal : public virtual NomClass, public NomInterfaceInternal
		{
		private:
			const std::string symname;
			NomInstantiationRef<NomClass> superClass = NomInstantiationRef<NomClass>();
		protected:
			NomClassInternal(NomStringRef name, const NomMemberContext* parent = nullptr);
		public:
			virtual ~NomClassInternal() override {}

			void SetSuperClass(NomInstantiationRef<NomClass> superClass);
			void SetSuperClass();

			// Inherited via NomClass
			virtual const NomInstantiationRef<NomClass> GetSuperClass(const NomSubstitutionContext* context = nullptr) const override;

		};
	}
}

