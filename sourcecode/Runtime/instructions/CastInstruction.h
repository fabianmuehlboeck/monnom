#pragma once
#include "../NomValueInstruction.h"
#include "../NomValue.h"
#include "../NomTypeDecls.h"
#include "llvm/IR/IRBuilder.h"

namespace Nom
{
	namespace Runtime
	{
		class CastInstruction : public NomValueInstruction
		{
		public:
			const RegIndex ValueRegister;
			const ConstantID TypeConstant;
			CastInstruction(const RegIndex reg, const RegIndex value, const ConstantID type);
			~CastInstruction();

			static NomValue MakeCast(NomBuilder &builder, CompileEnv* env, NomValue val, NomTypeRef type);

			// Inherited via NomValueInstruction
			virtual void Compile(NomBuilder& builder, CompileEnv* env, int lineno) override;

			// Inherited via NomValueInstruction
			virtual void Print(bool resolve = false) override;


			virtual void FillConstantDependencies(NOM_CONSTANT_DEPENCENCY_CONTAINER& result) override;
		};
	}
}