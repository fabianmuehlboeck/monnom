#pragma once

#include <string>
#include <vector>
#include <list>
#include <map>
PUSHDIAGSUPPRESSION
#include "llvm/ADT/StringMap.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/ADT/SmallVector.h"
POPDIAGSUPPRESSION
#include <unordered_map>
#include "NomStaticMethod.h"
#include "NomConstructor.h"
#include "NomMethod.h"
#include "Defs.h"
#include "NomString.h"
#include "RTClass.h"
#include "NomInstantiationRef.h"
#include "NomInterface.h"
#include "NomConstructor.h"
#include "NomField.h"
#include "RTDictionary.h"
#include "NomLambda.h"
#include "NomRecord.h"

namespace Nom
{
	namespace Runtime {
		class NomInterface;

		class NomClass : public virtual NomInterface
		{
		private:
			static auto& classes() {
				[[clang::no_destroy]] static std::unordered_map < NomStringRef, NomClass*, NomStringHash, NomStringEquality > classes; return classes;
			}
		protected:
			std::vector<NomTypedField*> Fields;
			mutable std::vector<NomTypedField*> AllFields;
			std::vector<NomLambda*> Lambdas;
			std::vector<NomRecord*> Structs;
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


			const NomField* GetField(NomStringRef name) const;
			virtual size_t GetFieldCount() const;

			void CompileLLVM(llvm::Module* mod) const override;
			void PreprocessInheritance() const override;

			NomValue GenerateConstructorCall(NomBuilder& builder, CompileEnv* env, const TypeList typeArgs, llvm::Value* objpointer, llvm::ArrayRef<NomValue> args) const;
			NomInstantiationRef<const NomStaticMethod> GetStaticMethod(NomStringRef methodName, const TypeList typeArgs, const TypeList argTypes) const;

			NomInstantiationRef<const NomConstructor> GetConstructor(const TypeList typeArgs, const TypeList argTypes) const;

		public:
			virtual llvm::Constant* GetSuperInstances(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage, llvm::GlobalVariable* gvar, llvm::StructType* stetype) const override;
			virtual llvm::ArrayType* GetSuperInstancesType(bool generic = true) const override;
			virtual const llvm::SmallVector<NomClassTypeRef, 16> GetSuperNameds() const override;

			llvm::Constant* GetInterfaceTableLookup(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const;

			virtual llvm::Constant* createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const override;
			virtual llvm::Constant* findLLVMElement(llvm::Module& mod) const override;
			llvm::Function* GetRawInvokeFunction(llvm::Module& mod) const;
			llvm::Function* GetRawInvokeFunction(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const;

			size_t GetTypeArgOffset() const;

			static llvm::FunctionType* GetInterfaceTableLookupType();
			virtual llvm::Constant* GetInterfaceDescriptor(llvm::Module& mod) const override;
			virtual llvm::Constant* GetCastFunction(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const override;
			virtual size_t GetSuperClassCount() const override;

		protected:
			virtual llvm::Constant* GetDynamicDispatcherLookup(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage, llvm::StructType* stype) const;
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
			void AddStruct(NomRecord* strct)
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

			NomLambda* AddLambda(const ConstantID lambdaID, RegIndex regcount, ConstantID closureTypeParams, ConstantID closureArguments, ConstantID typeParams, ConstantID argTypes, ConstantID returnType);
			NomRecord* AddStruct(const ConstantID structID, ConstantID closureTypeParams, RegIndex regcount, RegIndex endargregcount, ConstantID initializerArgTypes);


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

