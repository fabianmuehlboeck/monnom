#pragma once
#include "ClassTypeList.h"
#include "RTClass.h"
PUSHDIAGSUPPRESSION
#include "llvm/ADT/SmallVector.h"
#include "llvm/IR/Module.h"
POPDIAGSUPPRESSION
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
				[[clang::no_destroy]] static std::unordered_map<NomStringRef, NomNamed*, NomStringHash, NomStringEquality> val; return val;
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

			virtual size_t GetSuperClassCount() const = 0;

			virtual const llvm::SmallVector<NomClassTypeRef, 16> GetSuperNameds() const = 0;
			virtual NomClassTypeRef GetInstantiation(NomTypeRef type) const;
			intptr_t GetRTElement() const;
			virtual llvm::Constant* GetInterfaceDescriptor(llvm::Module& mod) const = 0;
		};

		class NomNamedLoaded : public virtual NomNamed, public NomMemberContextLoaded
		{

		protected:
			const ConstantID name; // String Constant
			mutable std::string namestr = "";
		public:

			NomNamedLoaded(const ConstantID name, const ConstantID typeArgs, const NomMemberContext* parent);
			virtual ~NomNamedLoaded() override = default;

			NomStringRef GetName() const override;


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

