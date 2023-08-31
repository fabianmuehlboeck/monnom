#pragma once
#include "Defs.h"
#include "NomString.h"
PUSHDIAGSUPPRESSION
#include "llvm/IR/IRBuilder.h"
POPDIAGSUPPRESSION
#include "CompileEnv.h"
#include "NomValue.h"
#include <set>
#include "NomBuilder.h"
#include "RTValuePtr.h"

namespace Nom
{
	namespace Runtime
	{
		class NomClass;
		class NomLambda;
		class NomRecord;
		class NomField
		{
		public:
			NomField();
			virtual ~NomField();
			NomField(const NomField&) = default;
			NomField(NomField&&) = default;
			virtual NomTypeRef GetType() const = 0;
			virtual NomStringRef GetName() const = 0;
			virtual RTValuePtr GenerateRead(NomBuilder &builder, CompileEnv* env, RTValuePtr receiver) const = 0;
			virtual void GenerateWrite(NomBuilder &builder, CompileEnv* env, RTValuePtr receiver, RTValuePtr value) const = 0;
			virtual bool IsReadOnly() const = 0;
			virtual bool IsVolatile() const = 0;
			virtual Visibility GetVisibility() const = 0;

			virtual void PushDependencies([[maybe_unused]] std::set<ConstantID>& set) const = 0;
		};

		class NomTypedField : public NomField
		{
		public:
			const ConstantID Name;
			const ConstantID Type;
			
			NomClass *const Class;
			mutable size_t Index=0;
			mutable bool IndexSet = false;
		private:
			const Visibility visibility;
			const bool readonly;
			const bool isvolatile;
		public:
			NomTypedField(NomClass* cls, const ConstantID name, const ConstantID type, Visibility visibility, bool readonly, bool isvolatile);
			virtual ~NomTypedField() override;
			NomTypedField(const NomTypedField& ) = default;
			NomTypedField(NomTypedField&&) = default;

			virtual NomTypeRef GetType() const override;
			virtual NomStringRef GetName() const override;
			virtual RTValuePtr GenerateRead(NomBuilder& builder, CompileEnv* env, RTValuePtr receiver) const override;
			virtual void GenerateWrite(NomBuilder& builder, CompileEnv* env, RTValuePtr receiver, RTValuePtr value) const override;
			virtual bool IsReadOnly() const override { return readonly; }
			virtual bool IsVolatile() const override { return isvolatile; }
			virtual Visibility GetVisibility() const override { return visibility; }

			void SetIndex(size_t index) const;
			virtual void PushDependencies([[maybe_unused]] std::set<ConstantID>& set) const override
			{
				set.insert(Name);
				set.insert(Type);
			}
		};

		class NomDictField : public NomField
		{
		private:
			NomStringRef Name;
			NomDictField(NomStringRef name);
		public:
			static NomDictField *GetInstance(NomStringRef name);
			virtual ~NomDictField() override;
			NomDictField(const NomDictField&) = default;
			NomDictField(NomDictField&&) = default;

			virtual NomTypeRef GetType() const override;
			virtual NomStringRef GetName() const override;
			virtual RTValuePtr GenerateRead(NomBuilder& builder, CompileEnv* env, RTValuePtr receiver) const override;
			virtual void GenerateWrite(NomBuilder& builder, CompileEnv* env, RTValuePtr receiver, RTValuePtr value) const override;

			virtual bool IsReadOnly() const override { return false; }
			virtual bool IsVolatile() const override { return false; }
			virtual Visibility GetVisibility() const override { return Visibility::Public; }

			virtual void PushDependencies([[maybe_unused]] std::set<ConstantID>& set) const override
			{
			}
		};

		class NomClosureField : public NomField
		{
		public:
			NomClosureField(NomLambda* lambda, const ConstantID name, const ConstantID type, const size_t index);
			virtual ~NomClosureField() override;
			NomClosureField(const NomClosureField&) = default;
			NomClosureField(NomClosureField&&) = default;

			const ConstantID Name;
			const ConstantID Type;
			NomLambda * const Lambda;
			const size_t Index;

			// Inherited via NomField
			virtual bool IsReadOnly() const override { return true; }
			virtual bool IsVolatile() const override { return false; }
			virtual Visibility GetVisibility() const override { return Visibility::Private; }
			virtual NomTypeRef GetType() const override;
			virtual NomStringRef GetName() const override;
			virtual RTValuePtr GenerateRead(NomBuilder& builder, CompileEnv* env, RTValuePtr receiver) const override;
			virtual void GenerateWrite(NomBuilder& builder, CompileEnv* env, RTValuePtr receiver, RTValuePtr value) const override;
			virtual void PushDependencies([[maybe_unused]] std::set<ConstantID>& set) const override
			{
				set.insert(Name);
				set.insert(Type);
			}
		};

		class NomRecordField : public NomField
		{
		private:
			const bool readonly;
		public:
			NomRecordField(NomRecord* structure, const ConstantID name, const ConstantID type, bool isReadOnly, const size_t index, RegIndex valueRegister);
			virtual ~NomRecordField() override;
			NomRecordField(const NomRecordField&) = default;
			NomRecordField(NomRecordField&&) = default;

			const ConstantID Name;
			const ConstantID Type;
			NomRecord* const Structure;
			const size_t Index;
			const RegIndex ValueRegister;

			// Inherited via NomField
			virtual bool IsReadOnly() const override { return readonly; }
			virtual bool IsVolatile() const override { return false; }
			virtual Visibility GetVisibility() const override { return Visibility::Public; }
			virtual NomTypeRef GetType() const override;
			virtual NomStringRef GetName() const override;
			virtual RTValuePtr GenerateRead(NomBuilder& builder, CompileEnv* env, RTValuePtr receiver) const override;
			virtual void GenerateWrite(NomBuilder& builder, CompileEnv* env, RTValuePtr receiver, RTValuePtr value) const override;
			virtual void PushDependencies([[maybe_unused]] std::set<ConstantID>& set) const override
			{
				set.insert(Name);
				set.insert(Type);
			}
		};
	}
}
