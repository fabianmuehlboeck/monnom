#pragma once
#include "NomValueInstruction.h"

namespace Nom
{
	namespace Runtime
	{
		enum class UnaryOperation : unsigned char
		{
			Negate,
			Not
		};

		const char* GetUnaryOpName(UnaryOperation op);

		class UnaryOpInstruction : public NomValueInstruction
		{
		public:
			const UnaryOperation Operation;
			const RegIndex Arg;
			UnaryOpInstruction(const UnaryOperation op, const RegIndex arg, const RegIndex reg);
			virtual ~UnaryOpInstruction() override;
			// Inherited via NomValueInstruction
			virtual void Compile(NomBuilder& builder, CompileEnv* env, size_t lineno) override;
			virtual void Print(bool resolve = false) override;
			virtual void FillConstantDependencies(NOM_CONSTANT_DEPENCENCY_CONTAINER& result) override;
		};
	}
}
