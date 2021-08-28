#pragma once
#include "TypeList.h"
#include "ClassTypeList.h"
#include "RTClass.h"
#include "llvm/ADT/SmallVector.h"
#include "RecursionBuffer.h"
#include "llvm/IR/Module.h"
#include "BoehmAllocator.h"
#include "AvailableExternally.h"
#include "NomMemberContext.h"

namespace Nom
{
	namespace Runtime
	{

		class ClassTypeList;

		class NomNamed : public AvailableExternally<llvm::Constant>, public virtual NomMemberContext
		{
		private:
			static std::unordered_map<NomStringRef, NomNamed*, NomStringHash, NomStringEquality>& namedTypes()
			{
				static std::unordered_map<NomStringRef, NomNamed*, NomStringHash, NomStringEquality> val; return val;
			}
			mutable std::unordered_map<llvm::ArrayRef<NomTypeRef>, NomClassTypeRef, NomTypeRefArrayRefHash, NomTypeRefArrayRefEquality, BoehmAllocator<std::pair<const llvm::ArrayRef<NomTypeRef>, NomClassTypeRef>>> instances;
		protected:
			NomNamed() {}
			static void Register(NomStringRef, NomNamed* instance);
		public:
			virtual ~NomNamed() override = default;

			static NomNamed* GetNamed(NomStringRef name);
			NomClassTypeRef GetType(llvm::ArrayRef<NomTypeRef> args = {}) const;

			virtual NomStringRef GetName() const = 0;

			virtual bool IsInterface() const = 0;
			virtual bool IsFunctional() const = 0;
			virtual bool HasLambdaMethod() const = 0;
			virtual bool HasNoMethods() const = 0;
			virtual bool GetHasRawInvoke() const = 0;
			virtual NomMethod* GetLambdaMethod() const = 0;

			const std::string GetSymbolRep() const;

			virtual bool FindInstantiations(NomNamed* other, RecBufferTypeList& myArgs, InstantiationList& results) const = 0;
			virtual const llvm::SmallVector<NomClassTypeRef, 16> GetSuperNameds(llvm::ArrayRef<NomTypeRef> args) const = 0;
			virtual NomClassTypeRef GetInstantiation(NomTypeRef type) const;
			intptr_t GetRTElement() const;
		};

		class NomNamedLoaded : public virtual NomNamed, public NomMemberContextLoaded
		{

		protected:
			const ConstantID name; // String Constant
			mutable std::string namestr = "";
		public:

			//const ConstantID TypeArgumentConstraints;
			NomNamedLoaded(const ConstantID name, const ConstantID typeArgs, const NomMemberContext* parent);
			virtual ~NomNamedLoaded() override = default;

			NomStringRef GetName() const override;
			//virtual TypeList GetInstantiation(NomNamed *other) const = 0;


			//virtual const RTClass * GetRTClass() const = 0;

			//virtual const std::list<NomClassTypeRef> GetSuperInterfaces(llvm::ArrayRef<NomTypeRef> args) const = 0;
			//virtual const llvm::SmallVector<NomClassTypeRef, 16> GetSuperNameds(llvm::ArrayRef<NomTypeRef> args) const = 0;

			//size_t GetTypeArgCount() const;

			//virtual std::list<std::pair<NomNamed *, TypeList>> &GetInstantiations() = 0;
			//void PruneInstantiations()
			//{
			//	std::unordered_map<NomNamed *, TypeList> instmap;
			//	for (auto tpair : GetInstantiations())
			//	{
			//		if (instmap.count(tpair.first) > 0)
			//		{
			//			instmap[tpair.first] = instmap[tpair.first].Meet(tpair.second);
			//		}
			//		else
			//		{
			//			instmap[tpair.first] = tpair.second;
			//		}
			//	}
			//	GetInstantiations().clear();
			//	for (auto tpair : instmap)
			//	{
			//		GetInstantiations().push_back(tpair);
			//	}
			//	
			//}

			//virtual llvm::GlobalVariable * GetClassDescriptorRef(llvm::Module &mod, bool refOnly = false) const = 0;


			//virtual llvm::GlobalVariable *createLLVMElement(llvm::Module *mod, llvm::GlobalValue::LinkageTypes linkage) const = 0;
			//virtual llvm::GlobalVariable *findLLVMElement(llvm::Module *mod) const = 0;

			//virtual llvm::StructType * GetClassDescriptorType(bool generic=true) const = 0;

			//NomClassTypeRef GetGeneralType() const;
			//NomClassTypeRef GetGeneralBottomType() const;


			const std::string* GetSymbolName() const override;
		};

		class NomNamedInternal : public virtual NomNamed, public NomMemberContextInternal
		{
		private:
			NomStringRef name;
			const std::string symname;
		protected:
			NomNamedInternal(NomStringRef name, const NomMemberContext* parent = nullptr);
		public:
			virtual ~NomNamedInternal() override = default;

			// Inherited via NomNamed
			virtual NomStringRef GetName() const override;
			virtual const std::string* GetSymbolName() const override;
		};
	}
}

