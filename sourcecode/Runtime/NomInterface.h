#pragma once
#include "RTClass.h"
#include "NomNamed.h"
#include "NomMethod.h"
#include <map>
#include <vector>
#include <set>
#include <unordered_map>
#include "NomConstants.h"
#include <unordered_map>
#include <mutex>

namespace Nom
{
	namespace Runtime
	{
		struct RTTypeArrHash
		{
			constexpr size_t operator()(const std::tuple<void**, int, size_t>& _keyval) const noexcept {
				return std::get<2>(_keyval);
			}
		};
		struct RTTypeArrEquals
		{
			constexpr bool operator()(const std::tuple<void**, int, size_t>& left, const std::tuple<void**, int, size_t>& right) const noexcept {
				if (std::get<2>(left) != std::get<2>(right) || std::get<1>(left) != std::get<1>(right))
				{
					return false;
				}

				void** leftArr = std::get<0>(left);
				void** rightArr = std::get<0>(right);
				for (int i = -std::get<1>(left); i < 0; i++)
				{
					if (leftArr[i] != rightArr[i])
					{
						return false;
					}
				}
				return true;
			}
		};
		using RuntimeInstantiationDictionary = std::unordered_map<std::tuple<void**, int, size_t>, void*, RTTypeArrHash, RTTypeArrEquals, BoehmAllocator<std::pair<const std::tuple<void**, int, size_t>, void*>>>;

		class NomModule;
		class NomMethodTableEntry;
		class NomClassInternal;
		class NomInterface : public virtual NomNamed
		{
		private:
			mutable bool hasResolvedDependencies = false;
			static InterfaceID idcounter();

			const InterfaceID id;
			mutable std::unordered_map<const NomInterface*, TypeList> instantiations;

			static std::unordered_map<NomStringRef, NomInterface*, NomStringHash, NomStringEquality>& interfaces() {
				[[clang::no_destroy]] static std::unordered_map<NomStringRef, NomInterface*, NomStringHash, NomStringEquality> instance; return instance;
			}
		protected:
			mutable bool preprocessed = false;
			mutable bool compiled = false;
			NomInterface();

			mutable llvm::ArrayType* methodTableType = nullptr;
			mutable llvm::StructType* classDescriptorType = nullptr;
			std::map<InterfaceID, size_t> InterfaceTableEntries;

			const std::unordered_map<const NomInterface*, TypeList>& GetInstantiations() { return instantiations; }

			virtual void ResolveDependencies(NomModule* mod) const = 0;
		public:
			RuntimeInstantiationDictionary runtimeInstantiations;
			std::mutex runtimeInstantiationsMutex;

			NomInterface(NomInterface&) = delete;
			NomInterface(const NomInterface&) = delete;
			NomInterface(NomInterface&&) = delete;

			virtual ~NomInterface() override = default;

			InterfaceID GetID() const { return id; }

			std::vector<NomMethod*> Methods;
			mutable std::vector<NomMethod*> AllMethods;
			mutable std::vector<NomMethodTableEntry*> MethodTable;

			virtual bool GetHasRawInvoke() const override;
			virtual bool IsInterface() const override { return true; }
			virtual bool IsFunctional() const override { return Methods.size() == 1 && Methods[0]->GetName().empty(); }
			virtual bool HasLambdaMethod() const override;
			virtual bool HasNoMethods() const override;
			virtual NomMethod* GetLambdaMethod() const override;


			static void Register(const NomString* name, NomInterface* iface)
			{
				interfaces()[name] = iface;
				NomNamed::Register(name, iface);
			}

			static NomInterface* GetInterface(const NomString* name)
			{
				auto search = interfaces().find(name);
				if (search != interfaces().end())
				{
					return interfaces()[name];
				}
				throw name;
			}

			bool EnsureDependenciesResolved(NomModule* mod) const
			{
				if (hasResolvedDependencies)
				{
					return true;
				}
				ResolveDependencies(mod);
				hasResolvedDependencies = true;
				return false;
			}
			virtual void PreprocessInheritance() const;
			virtual void CompileLLVM([[maybe_unused]] llvm::Module* mod) const {}

			void AddMethod(NomMethod* method)
			{
				Methods.push_back(method);
			}

			virtual const llvm::SmallVector<NomClassTypeRef, 16> GetSuperNameds() const override;
			virtual const llvm::ArrayRef<NomInstantiationRef<NomInterface>> GetSuperInterfaces(const NomSubstitutionContext *context=nullptr) const = 0;
			void AddInstantiation(const NomInstantiationRef<NomInterface> instantiation) const;
			const std::unordered_map<const NomInterface*, TypeList>& GetInstantiations() const;

			NomInstantiationRef<const NomMethod> GetMethod(const NomSubstitutionContext *context, NomStringRef methodName, const TypeList typeArgs, const TypeList argTypes) const;

			virtual llvm::Constant* GetMethodTable(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const;
			virtual llvm::ArrayType* GetMethodTableType(bool generic = true) const;

			virtual size_t GetSuperClassCount() const override;

			virtual llvm::Constant* GetSuperInstances(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage, llvm::GlobalVariable* gvar, llvm::StructType* stetype) const;
			virtual llvm::ArrayType* GetSuperInstancesType(bool generic = true) const;

			virtual llvm::Constant* createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const override;
			virtual llvm::Constant* findLLVMElement(llvm::Module& mod) const override;
			static llvm::FunctionType* GetGetUniqueInstantiationFunctionType();
			static llvm::Function* GetGetUniqueInstantiationFunction(llvm::Module& mod);
			virtual llvm::Constant* GetSignature(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const;
			virtual llvm::Constant* GetCheckReturnTypeFunction(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const;

			// Inherited via NomNamed
			virtual llvm::Constant* GetInterfaceDescriptor(llvm::Module& mod) const override;
};

		class NomInterfaceLoaded : public virtual NomInterface, public NomNamedLoaded
		{
		protected:
			const ConstantID superInterfaces;
			mutable llvm::ArrayRef<NomInstantiationRef<NomInterface>> superInterfacesBuf;
			
		public:
			NomInterfaceLoaded(NomInterfaceLoaded&) = delete;
			NomInterfaceLoaded(const NomInterfaceLoaded&) = delete;
			NomInterfaceLoaded(NomInterfaceLoaded&&) = delete;
			NomInterfaceLoaded(const ConstantID name, ConstantID typeArgs, const ConstantID superInterfaces, const NomMemberContext* parent);
			virtual ~NomInterfaceLoaded() override = default;

			NomMethodLoaded* AddMethod(const std::string& name, const std::string& qname, const ConstantID typeParameters, const ConstantID returnType, const ConstantID argumentTypes, const RegIndex regcount, bool isFinal);

			virtual const llvm::ArrayRef<NomInstantiationRef<NomInterface>> GetSuperInterfaces(const NomSubstitutionContext *context=nullptr) const override;
			
		protected:
			virtual void PushDependencies(std::set<ConstantID>& set) const;
			virtual void ResolveDependencies(NomModule* mod) const override;
		public:
			
		};

		class NomInterfaceInternal : public virtual NomInterface, public NomNamedInternal
		{
		private:
			mutable bool addedOnce = false;
			llvm::ArrayRef<NomInstantiationRef<NomInterface>> superInterfaces = llvm::ArrayRef<NomInstantiationRef<NomInterface>>(static_cast<NomInstantiationRef<NomInterface> *>(nullptr), static_cast<size_t>(0));
		protected:
			NomInterfaceInternal(NomStringRef name,  const NomMemberContext* parent=nullptr);
			virtual void ResolveDependencies(NomModule* mod) const override;
		public:
			virtual ~NomInterfaceInternal() override = default;

			// Inherited via NomInterface
			virtual const llvm::ArrayRef<NomInstantiationRef<NomInterface>> GetSuperInterfaces(const NomSubstitutionContext* context = nullptr) const override;

			void SetSuperInterfaces(llvm::ArrayRef<NomInstantiationRef<NomInterface>> superInterfaces);
			void SetSuperInterfaces();

			virtual void GetClassDependencies(llvm::SmallVector<const NomClassInternal*, 4>& results) const;
			virtual void GetInterfaceDependencies(llvm::SmallVector<const NomInterfaceInternal*, 4>& results) const;

			bool AddOnce() const
			{
				if (addedOnce)
				{
					return true;
				}
				addedOnce = true;
				return false;
			}

		};

		enum class SuperInstanceEntryFields : unsigned char {Class = 0, TypeArgs=1};
		llvm::StructType* SuperInstanceEntryType();
	}
}

extern "C" DLLEXPORT void* RT_NOM_GetUniqueInstantiation(Nom::Runtime::NomInterface * iface, void* rtinterface, void** typearr, size_t * hasharr, int arrsize);
