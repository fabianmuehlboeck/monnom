#pragma once
#include "Defs.h"
#include "NomString.h"
#include "llvm/IR/IRBuilder.h"
#include "CompileEnv.h"
#include "NomValue.h"
#include <set>
#include "NomBuilder.h"

namespace Nom
{
	namespace Runtime
	{
		class NomClass;
		class NomLambda;
		class NomField
		{
		public:
			NomField();
			virtual ~NomField();
			virtual NomTypeRef GetType() const = 0;
			virtual NomStringRef GetName() const = 0;
			virtual NomValue GenerateRead(NomBuilder &builder, CompileEnv* env, NomValue receiver) const = 0;
			virtual void GenerateWrite(NomBuilder &builder, CompileEnv* env, NomValue receiver, NomValue value) const = 0;
			virtual bool IsReadOnly() const = 0;
			virtual bool IsVolatile() const = 0;
			virtual Visibility GetVisibility() const = 0;

			virtual void PushDependencies(std::set<ConstantID>& set) const = 0;
		};

		class NomTypedField : public NomField
		{
		public:
			const ConstantID Name;
			const ConstantID Type;
			
			NomClass *const Class;
			mutable int Index=-1;
		private:
			const Visibility visibility;
			const bool readonly;
			const bool isvolatile;
		public:
			NomTypedField(NomClass* cls, const ConstantID name, const ConstantID type, Visibility visibility, bool readonly, bool isvolatile);
			virtual ~NomTypedField() override;

			virtual NomTypeRef GetType() const override;
			virtual NomStringRef GetName() const override;
			virtual NomValue GenerateRead(NomBuilder& builder, CompileEnv* env, NomValue receiver) const override;
			virtual void GenerateWrite(NomBuilder& builder, CompileEnv* env, NomValue receiver, NomValue value) const override;
			virtual bool IsReadOnly() const override { return readonly; }
			virtual bool IsVolatile() const override { return isvolatile; }
			virtual Visibility GetVisibility() const override { return visibility; }

			void SetIndex(int index) const;
			virtual void PushDependencies(std::set<ConstantID>& set) const override
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

			virtual NomTypeRef GetType() const override;
			virtual NomStringRef GetName() const override;
			virtual NomValue GenerateRead(NomBuilder& builder, CompileEnv* env, NomValue receiver) const override;
			virtual void GenerateWrite(NomBuilder& builder, CompileEnv* env, NomValue receiver, NomValue value) const override;

			virtual bool IsReadOnly() const override { return false; }
			virtual bool IsVolatile() const override { return false; }
			virtual Visibility GetVisibility() const override { return Visibility::Public; }

			virtual void PushDependencies(std::set<ConstantID>& set) const override
			{
			}
		};

		class NomClosureField : public NomField
		{
		public:
			NomClosureField(NomLambda* lambda, const ConstantID name, const ConstantID type, const int index);
			virtual ~NomClosureField() override;

			const ConstantID Name;
			const ConstantID Type;
			NomLambda * const Lambda;
			const int Index;

			// Inherited via NomField
			virtual bool IsReadOnly() const override { return true; }
			virtual bool IsVolatile() const override { return false; }
			virtual Visibility GetVisibility() const override { return Visibility::Private; }
			virtual NomTypeRef GetType() const override;
			virtual NomStringRef GetName() const override;
			virtual NomValue GenerateRead(NomBuilder& builder, CompileEnv* env, NomValue receiver) const override;
			virtual void GenerateWrite(NomBuilder& builder, CompileEnv* env, NomValue receiver, NomValue value) const override;
			virtual void PushDependencies(std::set<ConstantID>& set) const override
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
			NomRecordField(NomRecord* structure, const ConstantID name, const ConstantID type, bool isReadOnly, const int index, RegIndex valueRegister);
			virtual ~NomRecordField() override;

			const ConstantID Name;
			const ConstantID Type;
			NomRecord* const Structure;
			const int Index;
			const RegIndex ValueRegister;

			// Inherited via NomField
			virtual bool IsReadOnly() const override { return readonly; }
			virtual bool IsVolatile() const override { return false; }
			virtual Visibility GetVisibility() const override { return Visibility::Public; }
			virtual NomTypeRef GetType() const override;
			virtual NomStringRef GetName() const override;
			virtual NomValue GenerateRead(NomBuilder& builder, CompileEnv* env, NomValue receiver) const override;
			virtual void GenerateWrite(NomBuilder& builder, CompileEnv* env, NomValue receiver, NomValue value) const override;
			virtual void PushDependencies(std::set<ConstantID>& set) const override
			{
				set.insert(Name);
				set.insert(Type);
			}
		};
	}
}