#pragma once
#include "../NomValueInstruction.h"
#include "../NomInstantiationRef.h"
#include "../Defs.h"

namespace Nom
{
	namespace Runtime
	{
		class CallCheckedStaticMethod : public NomValueInstruction
		{
		public:
			const ConstantID Method;
			const ConstantID TypeArgs;
			CallCheckedStaticMethod(const ConstantID method, const ConstantID typeArgs, RegIndex reg);
			virtual ~CallCheckedStaticMethod();
			virtual void Compile(NomBuilder &builder, CompileEnv* env, int lineno) override;
			void CompileActual(NomInstantiationRef<const NomStaticMethod> method, TypeList typeArgs, NomBuilder& builder, CompileEnv* env, int lineno);
			// Inherited via NomValueInstruction
			virtual void Print(bool resolve = false) override;


			virtual void FillConstantDependencies(NOM_CONSTANT_DEPENCENCY_CONTAINER& result) override;
		};
	}
}