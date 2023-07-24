#pragma once
PUSHDIAGSUPPRESSION
#include "llvm/IR/Value.h"
POPDIAGSUPPRESSION
#include "../NomValueInstruction.h"
#include "../NomConstants.h"

namespace Nom
{
	namespace Runtime
	{
		class RTTypeHead;
		class CallConstructor : public NomValueInstruction
		{
		public:
			const ConstantID ClassSuperClass;
			const ConstantID TypeArgs;
			CallConstructor(const RegIndex reg, ConstantID cls, ConstantID typeArgs);
			~CallConstructor() override;

			virtual void Compile(NomBuilder& builder, CompileEnv* env, size_t lineno) override;

			// Inherited via NomValueInstruction
			virtual void Print(bool resolve = false) override;


			virtual void FillConstantDependencies(NOM_CONSTANT_DEPENCENCY_CONTAINER& result) override;
		};

		llvm::Function* GenerateCConstructor(llvm::Module& mod, const NomConstructor* cnstrctr);

	}
}
