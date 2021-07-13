#pragma once

#include "../NomInstruction.h"
#include "../NomType.h"

namespace Nom
{
	namespace Runtime
	{

		class TypeArgumentInstruction : public NomInstruction
		{
		public:
			const NomType * const Type;
			TypeArgumentInstruction(const NomType * type);
			virtual ~TypeArgumentInstruction();
			virtual void Compile(NomBuilder &builder, CompileEnv* env, int lineno) override;

			virtual void FillConstantDependencies(NOM_CONSTANT_DEPENCENCY_CONTAINER& result) override;
		};


	}
}
