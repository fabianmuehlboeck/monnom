#pragma once
#include "Defs.h"
#include "NomString.h"
#include "NomAlloc.h"
#include "BoehmUncollectableAllocator.h"
#include <vector>
#include "ObjectHeader.h"
#include "Context.h"
#include "llvm/IR/Type.h"
#include "llvm/ADT/SmallVector.h"
#include <map>
#include "ClassTypeList.h"
#include "TypeList.h"
#include "NomInstantiationRef.h"
//#include "NomMethod.h"
//#include "NomStaticMethod.h"
//#include "NomVMInterface.h"
#include "boehmgcinterface.h"
#include "NomSubstitutionContext.h"

namespace Nom
{
	namespace Runtime
	{
		class NomClass;
		class NomInterface;
		class NomConstant
		{
		public:
			const NomConstantType Type;
			virtual void Print(bool resolve = false) = 0;

			NomConstant(NomConstantType type) : Type(type)
			{

			}

			NomConstant(NomConstant &other) = delete;

			virtual ~NomConstant() = default;


			virtual void FillConstantDependencies(NOM_CONSTANT_DEPENCENCY_CONTAINER& result) = 0;
		};

		class NomClass;
		class RTClass;
		class NomMethod;
		class NomStaticMethod;
		class NomConstructor;
		class NomMemberContext;
		class NomLambda;
		class NomStruct;
		class NomModule;
	}
}

namespace llvm
{
	class GlobalVariable;
}

extern const Nom::Runtime::NomClass * const _NomStringClassNC;
extern const Nom::Runtime::RTClass * const _RTStringClassRTC;

namespace Nom
{
	namespace Runtime
	{
		class NomStringConstant : public NomConstant
		{
		private:
			llvm::Constant * obj = nullptr;
		public:
			const NomString Text;

			void* operator new(size_t count)
			{
				return bgc_malloc_uncollectable(count);
			}

			NomStringConstant(NomString text) : NomConstant(NomConstantType::CTString), Text(text)
			{

			}

			NomStringConstant(NomStringConstant&) = delete;
			NomStringConstant(const NomStringConstant&) = delete;
			NomStringConstant(NomStringConstant&&) = delete;
			virtual ~NomStringConstant() override = default;

			const NomString * GetText()
			{
				return &Text;
			}

			llvm::Constant * getObject(llvm::Module &mod);

			// Inherited via NomConstant
			virtual void Print(bool resolve = false) override;
			virtual void FillConstantDependencies(NOM_CONSTANT_DEPENCENCY_CONTAINER& result) override {}
		};

		class NomClassConstant : public NomConstant
		{
		private:
			const NomClass * cls = nullptr;
			const ConstantID Library;
			const ConstantID Name;
		public:
			NomClassConstant(NomClassConstant&) = delete;
			NomClassConstant(const NomClassConstant&) = delete;
			NomClassConstant(NomClassConstant&&) = delete;
			NomClassConstant(const ConstantID library, const ConstantID name) : NomConstant(NomConstantType::CTClass), Library(library), Name(name)
			{

			}
			virtual ~NomClassConstant() override = default;
			const NomClass * GetClass();
			void EnsureClassLoaded(NomModule* mod);

			// Inherited via NomConstant
			virtual void Print(bool resolve = false) override;
			virtual void FillConstantDependencies(NOM_CONSTANT_DEPENCENCY_CONTAINER& result) override
			{
				result.push_back(Library);
				result.push_back(Name);
			}
		};

		class NomInterfaceConstant : public NomConstant
		{
		private:
			const ConstantID Library;
			const ConstantID Name;
			mutable const NomInterface * iface  = nullptr;
		public:

			NomInterfaceConstant(NomInterfaceConstant&) = delete;
			NomInterfaceConstant(const NomInterfaceConstant&) = delete;
			NomInterfaceConstant(NomInterfaceConstant&&) = delete;
			NomInterfaceConstant(const ConstantID library, const ConstantID name) : NomConstant(NomConstantType::CTInterface), Library(library), Name(name)
			{
			}
			virtual ~NomInterfaceConstant() override = default;
			const NomInterface * GetInterface() const;
			void EnsureInterfaceLoaded(NomModule *mod);

			// Inherited via NomConstant
			virtual void Print(bool resolve = false) override;
			virtual void FillConstantDependencies(NOM_CONSTANT_DEPENCENCY_CONTAINER& result) override
			{
				result.push_back(Library);
				result.push_back(Name);
			}
		};

		class NomTypeParameterConstant /*: public NomConstant*/
		{
		private:

		public:
			ParameterVariance Variance;
			ConstantID LowerBound;
			ConstantID UpperBound;
			//NomTypeParameterConstant(NomTypeParameterConstant&) = delete;
			//NomTypeParameterConstant(const NomTypeParameterConstant&) = delete;
			//NomTypeParameterConstant(NomTypeParameterConstant&&) = delete;
			NomTypeParameterConstant(Nom::Runtime::ParameterVariance variance, ConstantID lowerBound, ConstantID upperBound) : /*NomConstant(NomConstantType::CTTypeParameter),*/ Variance(variance), LowerBound(lowerBound), UpperBound(upperBound)
			{
			}
			virtual void FillConstantDependencies(NOM_CONSTANT_DEPENCENCY_CONTAINER& result)
			{
				result.push_back(LowerBound);
				result.push_back(UpperBound);
			}
		//	virtual ~NomTypeParameterConstant() override = default;
		};

		class NomLambdaConstant : public NomConstant
		{
		private:
			mutable NomLambda* lambda = nullptr;
		public:
			NomLambdaConstant(NomLambdaConstant&) = delete;
			NomLambdaConstant(const NomLambdaConstant&) = delete;
			NomLambdaConstant(NomLambdaConstant&&) = delete;
			NomLambdaConstant() : NomConstant(NomConstantType::CTLambda)
			{
			}
			virtual ~NomLambdaConstant() override = default;
			NomLambda* GetLambda() const;
			void SetLambda(NomLambda* lambda) const;

			// Inherited via NomConstant
			virtual void Print(bool resolve = false) override;
			virtual void FillConstantDependencies(NOM_CONSTANT_DEPENCENCY_CONTAINER& result) override
			{
				//Filled when going through parent class's dependencies
			}
		};

		class NomStructConstant : public NomConstant
		{
		private:
			mutable NomStruct* structure = nullptr;
		public:
			NomStructConstant(NomStructConstant&) = delete;
			NomStructConstant(const NomStructConstant&) = delete;
			NomStructConstant(NomStructConstant&&) = delete;
			NomStructConstant() : NomConstant(NomConstantType::CTStruct)
			{
			}
			virtual ~NomStructConstant() override = default;
			NomStruct* GetStruct() const;
			void SetStruct(NomStruct* structure) const;

			// Inherited via NomConstant
			virtual void Print(bool resolve = false) override;
			virtual void FillConstantDependencies(NOM_CONSTANT_DEPENCENCY_CONTAINER& result) override
			{
				//Filled when going through parent class's dependencies
			}
		};

		class NomSuperClassConstant : public NomConstant
		{
		private:
			const ConstantID SuperClass;
			const ConstantID Arguments;
			mutable NomInstantiationRef<NomClass> value;
		public:
			NomSuperClassConstant(NomSuperClassConstant&) = delete;
			NomSuperClassConstant(const NomSuperClassConstant&) = delete;
			NomSuperClassConstant(NomSuperClassConstant&&) = delete;
			NomSuperClassConstant(const ConstantID superclass, const ConstantID args) : NomConstant(NomConstantType::CTSuperClass), SuperClass(superclass), Arguments(args)
			{
			}
			virtual ~NomSuperClassConstant() override = default;
			NomInstantiationRef<NomClass>  GetClassType(NomSubstitutionContextRef context) const;

			// Inherited via NomConstant
			virtual void Print(bool resolve = false) override;

			virtual void FillConstantDependencies(NOM_CONSTANT_DEPENCENCY_CONTAINER& result) override
			{
				result.push_back(SuperClass);
				result.push_back(Arguments);
			}
		};


		class NomSuperInterfacesConstant : public NomConstant
		{
		private:
			const llvm::SmallVector<std::tuple<ConstantID, ConstantID>, 4> entries;
			mutable llvm::SmallVector<NomInstantiationRef<NomInterface>, 4> ifaces;
		public:
			NomSuperInterfacesConstant(NomSuperInterfacesConstant&) = delete;
			NomSuperInterfacesConstant(const NomSuperInterfacesConstant&) = delete;
			NomSuperInterfacesConstant(NomSuperInterfacesConstant&&) = delete;
			NomSuperInterfacesConstant(const llvm::SmallVector<std::tuple<ConstantID, ConstantID>, 4> &entries) : NomConstant(NomConstantType::CTSuperInterfaces), entries(entries)
			{
			}
			virtual ~NomSuperInterfacesConstant() override = default;
			const llvm::ArrayRef<NomInstantiationRef<NomInterface>> GetSuperInterfaces(NomSubstitutionContextRef context) const;

			// Inherited via NomConstant
			virtual void Print(bool resolve = false) override;

			virtual void FillConstantDependencies(NOM_CONSTANT_DEPENCENCY_CONTAINER& result) override
			{
				for (auto pair : entries)
				{
					result.push_back(std::get<0>(pair));
					result.push_back(std::get<1>(pair));
				}
			}
		};

		class NomTypeListConstant : public NomConstant
		{
		private:
			llvm::SmallVector<ConstantID, 8> types;
			mutable NomTypeRef *typeRefs = nullptr;
		public:

			NomTypeListConstant(NomTypeListConstant&) = delete;
			NomTypeListConstant(const NomTypeListConstant&) = delete;
			NomTypeListConstant(NomTypeListConstant&&) = delete;
			NomTypeListConstant(const llvm::ArrayRef<ConstantID> types) : NomConstant(NomConstantType::CTTypeList)
			{
				for (auto tid : types)
				{
					this->types.push_back(tid);
				}
			}
			virtual ~NomTypeListConstant() override = default;
			size_t GetSize() const;
			llvm::ArrayRef<NomTypeRef> GetTypeList(NomSubstitutionContextRef context) const;

			// Inherited via NomConstant
			virtual void Print(bool resolve = false) override;

			virtual void FillConstantDependencies(NOM_CONSTANT_DEPENCENCY_CONTAINER& result) override
			{
				for (auto type : types)
				{
					result.push_back(type);
				}
			}
		};

		class NomTypeParametersConstant : public NomConstant
		{
		private:
			llvm::SmallVector<NomTypeParameterConstant*, 8> typeParams;
		public:

			NomTypeParametersConstant(NomTypeParametersConstant&) = delete;
			NomTypeParametersConstant(const NomTypeParametersConstant&) = delete;
			NomTypeParametersConstant(NomTypeParametersConstant&&) = delete;
			NomTypeParametersConstant(const llvm::ArrayRef<NomTypeParameterConstant*> typeParams) : NomConstant(NomConstantType::CTTypeParameters)
			{
				for (auto tp : typeParams)
				{
					this->typeParams.push_back(tp);
				}
			}
			virtual ~NomTypeParametersConstant() override = default;
			size_t GetSize() const { return typeParams.size(); }
			llvm::ArrayRef<NomTypeParameterConstant*> GetParameters() const { return typeParams; }

			// Inherited via NomConstant
			virtual void Print(bool resolve = false) override;
			virtual void FillConstantDependencies(NOM_CONSTANT_DEPENCENCY_CONTAINER& result) override
			{
				for (auto param : typeParams)
				{
					param->FillConstantDependencies(result);
				}
			}
		};

		//class NomClassTypeListConstant : public NomConstant
		//{
		//private:
		//	const llvm::SmallVector<ConstantID, 8> types;
		//public:

		//	NomClassTypeListConstant(NomClassTypeListConstant&) = delete;
		//	NomClassTypeListConstant(const NomClassTypeListConstant&) = delete;
		//	NomClassTypeListConstant(NomClassTypeListConstant&&) = delete;
		//	NomClassTypeListConstant(const llvm::SmallVector<ConstantID, 8> &types) : NomConstant(NomConstantType::CTTypeList), types(types)
		//	{

		//	}
		//	ClassTypeList GetClassTypeList();
		//};

		class NomClassTypeConstant : public NomConstant
		{
		private:
			const ConstantID cls;
			const ConstantID args;
			mutable NomClassTypeRef value = nullptr;
		public:

			NomClassTypeConstant(NomClassTypeConstant&) = delete;
			NomClassTypeConstant(const NomClassTypeConstant&) = delete;
			NomClassTypeConstant(NomClassTypeConstant&&) = delete;
			NomClassTypeConstant(const ConstantID cls, const ConstantID args) : NomConstant(NomConstantType::CTClassType), cls(cls), args(args)
			{

			}
			virtual ~NomClassTypeConstant() override = default;
			NomClassTypeRef GetClassType(NomSubstitutionContextRef context) const;

			// Inherited via NomConstant
			virtual void Print(bool resolve = false) override;

			virtual void FillConstantDependencies(NOM_CONSTANT_DEPENCENCY_CONTAINER& result) override
			{
				result.push_back(cls);
				result.push_back(args);
			}
		};

		class NomTypeVarConstant : public NomConstant
		{
		private:
			const int index;
		public:
			NomTypeVarConstant(NomTypeVarConstant&) = delete;
			NomTypeVarConstant(const NomTypeVarConstant&) = delete;
			NomTypeVarConstant(NomTypeVarConstant&&) = delete;

			NomTypeVarConstant(const int index) : NomConstant(NomConstantType::CTTypeVar), index(index)
			{

			}
			virtual ~NomTypeVarConstant() override = default;
			NomTypeRef GetTypeRef(NomSubstitutionContextRef context) const;

			// Inherited via NomConstant
			virtual void Print(bool resolve = false) override;

			virtual void FillConstantDependencies(NOM_CONSTANT_DEPENCENCY_CONTAINER& result) override
			{
			}
		};

		class NomMaybeTypeConstant : public NomConstant
		{
		private:
			const ConstantID ptype;
			mutable NomMaybeTypeRef value = nullptr;
		public:
			NomMaybeTypeConstant(NomMaybeTypeConstant&) = delete;
			NomMaybeTypeConstant(const NomMaybeTypeConstant&) = delete;
			NomMaybeTypeConstant(NomMaybeTypeConstant&&) = delete;

			NomMaybeTypeConstant(const ConstantID ptype) : NomConstant(NomConstantType::CTMaybe), ptype(ptype)
			{

			}
			virtual ~NomMaybeTypeConstant() override = default;

			NomMaybeTypeRef GetTypeRef(NomSubstitutionContextRef context) const;

			// Inherited via NomConstant
			virtual void Print(bool resolve = false) override;

			virtual void FillConstantDependencies(NOM_CONSTANT_DEPENCENCY_CONTAINER& result) override;

		};

		class NomDynamicTypeConstant : public NomConstant
		{
		public:
			NomDynamicTypeConstant(NomDynamicTypeConstant&) = delete;
			NomDynamicTypeConstant(const NomDynamicTypeConstant&) = delete;
			NomDynamicTypeConstant(NomDynamicTypeConstant&&) = delete;

			NomDynamicTypeConstant() : NomConstant(NomConstantType::CTDynamic)
			{

			}
			virtual ~NomDynamicTypeConstant() override = default;
			// Inherited via NomConstant
			virtual void Print(bool resolve = false) override;
			virtual void FillConstantDependencies(NOM_CONSTANT_DEPENCENCY_CONTAINER& result) override
			{
			}
		};

		class NomBottomConstant : public NomConstant
		{
		public:
			NomBottomConstant(NomBottomConstant&) = delete;
			NomBottomConstant(const NomBottomConstant&) = delete;
			NomBottomConstant(NomBottomConstant&&) = delete;

			NomBottomConstant() : NomConstant(NomConstantType::CTBottom)
			{

			}
			virtual ~NomBottomConstant() override = default;
			// Inherited via NomConstant
			virtual void Print(bool resolve = false) override;
			virtual void FillConstantDependencies(NOM_CONSTANT_DEPENCENCY_CONTAINER& result) override
			{
			}
		};

		class NomMethodConstant : public NomConstant
		{
		private:
			const ConstantID classConstant;
			const ConstantID methodName;
			const ConstantID typeArgs;
			const ConstantID argTypes;
			mutable NomInstantiationRef<const NomMethod> method = NomInstantiationRef<const NomMethod>();
		public:

			NomMethodConstant(NomMethodConstant&) = delete;
			NomMethodConstant(const NomMethodConstant&) = delete;
			NomMethodConstant(NomMethodConstant&&) = delete;
			NomMethodConstant(const ConstantID cls, const ConstantID methodname, const ConstantID typeArgs, const ConstantID argTypes) : NomConstant(NomConstantType::CTMethod), classConstant(cls), methodName(methodname), typeArgs(typeArgs), argTypes(argTypes)
			{

			}
			virtual ~NomMethodConstant() override = default;
			NomInstantiationRef<const NomMethod> GetMethod(NomSubstitutionContextRef context) const;

			// Inherited via NomConstant
			virtual void Print(bool resolve = false) override;
			virtual void FillConstantDependencies(NOM_CONSTANT_DEPENCENCY_CONTAINER& result) override
			{
				result.push_back(classConstant);
				result.push_back(methodName);
				result.push_back(typeArgs);
				result.push_back(argTypes);
			}
		};

		class NomStaticMethodConstant : public NomConstant
		{
		private:
			mutable NomInstantiationRef<const NomStaticMethod> method = NomInstantiationRef<const NomStaticMethod>();
		public:
			const ConstantID ClassConstant;
			const ConstantID MethodName;
			const ConstantID TypeArgs;
			const ConstantID ArgTypes;
			NomStaticMethodConstant(NomStaticMethodConstant&) = delete;
			NomStaticMethodConstant(const NomStaticMethodConstant&) = delete;
			NomStaticMethodConstant(NomStaticMethodConstant&&) = delete;
			NomStaticMethodConstant(const ConstantID cls, const ConstantID methodname, const ConstantID typeArgs, const ConstantID argTypes) : NomConstant(NomConstantType::CTStaticMethod), ClassConstant(cls), MethodName(methodname), TypeArgs(typeArgs), ArgTypes(argTypes)
			{

			}
			virtual ~NomStaticMethodConstant() override = default;
			NomInstantiationRef<const NomStaticMethod> GetStaticMethod(NomSubstitutionContextRef context) const;

			// Inherited via NomConstant
			virtual void Print(bool resolve = false) override;
			virtual void FillConstantDependencies(NOM_CONSTANT_DEPENCENCY_CONTAINER& result) override
			{
				result.push_back(ClassConstant);
				result.push_back(MethodName);
				result.push_back(TypeArgs);
				result.push_back(ArgTypes);
			}
		};

		class NomConstructorConstant : public NomConstant
		{
		private:
			mutable NomInstantiationRef<const NomConstructor> constructor = NomInstantiationRef<const NomConstructor>();
		public:
			const ConstantID ClassConstant;
			const ConstantID TypeArgs;
			const ConstantID ArgTypes;
			NomConstructorConstant(NomConstructorConstant&) = delete;
			NomConstructorConstant(const NomConstructorConstant&) = delete;
			NomConstructorConstant(NomConstructorConstant&&) = delete;
			NomConstructorConstant(const ConstantID cls, const ConstantID typeArgs, const ConstantID argTypes) : NomConstant(NomConstantType::CTConstructor), ClassConstant(cls), TypeArgs(typeArgs), ArgTypes(argTypes)
			{

			}
			virtual ~NomConstructorConstant() override = default;
			NomInstantiationRef<const NomConstructor> GetConstructor(NomSubstitutionContextRef context) const;

			// Inherited via NomConstant
			virtual void Print(bool resolve = false) override;
			virtual void FillConstantDependencies(NOM_CONSTANT_DEPENCENCY_CONTAINER& result) override
			{
				result.push_back(ClassConstant);
				result.push_back(TypeArgs);
				result.push_back(ArgTypes);
			}
		};

		class NomConstantContext
		{
		private:
			std::map < LocalConstantID, ConstantID > idmap = { {0, 0} };

		public:
			NomConstantContext() {}
			NomConstantContext(NomConstantContext&) = delete;
			NomConstantContext(const NomConstantContext&) = delete;
			NomConstantContext(NomConstantContext&&) = default;
			ConstantID operator[](const LocalConstantID local) const;

			ConstantID GetGlobalID(const LocalConstantID local);
			ConstantID TryGetGlobalID(const LocalConstantID local) const;

			ConstantID AddString(LocalConstantID lid, const NomString& text);
			ConstantID AddClass(LocalConstantID lid, const LocalConstantID library, const LocalConstantID name);
			ConstantID AddInterface(LocalConstantID lid, const LocalConstantID library, const LocalConstantID name);
			ConstantID AddLambda(LocalConstantID lid);
			ConstantID AddStruct(LocalConstantID lid);
			ConstantID AddSuperClass(LocalConstantID lid, const LocalConstantID cls, const LocalConstantID args);
			ConstantID AddSuperInterfaces(LocalConstantID lid, const llvm::SmallVector<std::tuple<LocalConstantID, LocalConstantID>, 4>& entries);
			ConstantID AddMethod(LocalConstantID lid, const LocalConstantID cls, const LocalConstantID name, const LocalConstantID typeArgs, const LocalConstantID argTypes);
			ConstantID AddStaticMethod(LocalConstantID lid, const LocalConstantID cls, const LocalConstantID name, const LocalConstantID typeArgs, const LocalConstantID argTypes);
			ConstantID AddTypeList(LocalConstantID lid, const llvm::SmallVector<LocalConstantID, 8> types);
			ConstantID AddClassType(LocalConstantID lid, const LocalConstantID cls, const LocalConstantID typeArgs);
			ConstantID AddBottomType(LocalConstantID lid);
			ConstantID AddDynamicType(LocalConstantID lid);
			ConstantID AddTypeVar(LocalConstantID lid, int index);
			ConstantID AddTypeParameters(LocalConstantID lid, llvm::ArrayRef<NomTypeParameterConstant*> parameters);
			ConstantID AddConstructor(LocalConstantID lid, const LocalConstantID cls, const LocalConstantID typeArgs, const LocalConstantID argTypes);
			ConstantID AddMaybeType(LocalConstantID lid, const LocalConstantID ptype);
		};

		class NomConstants
		{
		private:
			static std::vector<NomConstant*>& constants() {
				static std::vector<NomConstant*> constants(1); return constants;
			};
			NomConstants() {}
			NomConstants(NomConstants&) = delete;
			NomConstants(const NomConstants&) = delete;
			NomConstants(NomConstants&&) = delete;
		public:
			const NomConstant* operator[](size_t index)
			{
				return constants()[index];
			}
			static NomConstant* Get(const ConstantID constant)
			{
				return constants()[constant];
			}
			static void PrintConstant(const ConstantID constant, bool resolve);
			static NomStringConstant* GetString(const ConstantID constant);
			static NomClassConstant* GetClass(const ConstantID constant);
			static NomLambdaConstant* GetLambda(const ConstantID constant);
			static NomStructConstant* GetStruct(const ConstantID constant);
			static NomMethodConstant* GetMethod(const ConstantID constant);
			static NomStaticMethodConstant* GetStaticMethod(const ConstantID constant);
			static NomConstructorConstant* GetConstructor(const ConstantID constant);
			static NomTypeListConstant* GetTypeList(const ConstantID constant);
			//static NomClassTypeListConstant * GetClassTypeList(const ConstantID constant)
			//{
			//	if (constant == 0)
			//	{
			//		static NomClassTypeListConstant defaultConst{ llvm::SmallVector<ConstantID, 8>() };
			//		return &defaultConst;
			//	}
			//	NomConstant *cnstnt = constants()[constant];
			//	if (cnstnt->Type != NomConstantType::CTClassTypeList)
			//	{
			//		throw new std::exception();
			//	}
			//	return (NomClassTypeListConstant *)cnstnt;
			//}
			static NomClassTypeConstant* GetClassType(const ConstantID constant);
			static NomSuperClassConstant* GetSuperClass(const ConstantID constant);
			static NomSuperInterfacesConstant* GetSuperInterfaces(const ConstantID constant);
			static NomInterfaceConstant* GetInterface(const ConstantID constant);
			static NomTypeParametersConstant* GetTypeParameters(const ConstantID constant);
			static NomTypeRef GetType(NomSubstitutionContextRef context, const ConstantID constant, bool defaultBottom = false);

			static ConstantID GetConstantID();

			static ConstantID AddString(const NomString& text, ConstantID cid = 0);
			static ConstantID AddClass(const ConstantID library, const ConstantID name, ConstantID cid = 0);
			static ConstantID AddInterface(const ConstantID library, const ConstantID name, ConstantID cid = 0);
			static ConstantID AddLambda(ConstantID cid = 0);
			static ConstantID AddStruct(ConstantID cid);
			static ConstantID AddMethod(const ConstantID cls, const ConstantID name, const ConstantID typeArgs, const ConstantID argTypes, ConstantID cid = 0);
			static ConstantID AddStaticMethod(const ConstantID cls, const ConstantID name, const ConstantID typeArgs, const ConstantID argTypes, ConstantID cid = 0);
			static ConstantID AddConstructor(const ConstantID cls, const ConstantID typeArgs, const ConstantID argTypes, ConstantID cid = 0);
			static ConstantID AddTypeList(const llvm::ArrayRef<ConstantID> types, ConstantID cid = 0);
			static ConstantID AddClassType(const ConstantID cls, const ConstantID typeArgs, ConstantID cid = 0);
			static ConstantID AddBottomType(ConstantID cid = 0);
			static ConstantID AddDynamicType(ConstantID cid = 0);
			static ConstantID AddMaybeType(const ConstantID ptype, ConstantID cid = 0);
			static ConstantID AddTypeVariable(int index, ConstantID cid = 0);
			//static ConstantID AddTypeParameter(ParameterVariance variance, const ConstantID lowerBound, const ConstantID upperBound, ConstantID cid = 0)
			//{
			//	if (cid == 0)
			//	{
			//		cid = GetConstantID();
			//	}
			//	constants()[cid] = (new NomTypeParameterConstant(variance, lowerBound, upperBound));
			//	return cid;
			//}
			static ConstantID AddTypeParameters(llvm::ArrayRef<NomTypeParameterConstant*> typeParameters, ConstantID cid = 0);
			static ConstantID AddSuperClass(const ConstantID cls, const ConstantID args, ConstantID cid = 0);
			static ConstantID AddSuperInterfaces(const llvm::SmallVector<std::tuple<ConstantID, ConstantID>, 4> & entries, ConstantID cid = 0);
		};
	}
}