#pragma once
#include "../Defs.h"
#include "../NomInstruction.h"

namespace Nom
{
	namespace Runtime
	{

		class DebugInstruction : public NomInstruction
		{
		private:
			const ConstantID messageConstant;
		public:
			DebugInstruction(const ConstantID message);
			virtual ~DebugInstruction() override;
			virtual void Compile(NomBuilder &builder, CompileEnv* env, size_t lineno) override;

			// Inherited via NomInstruction
			virtual void Print(bool resolve = false) override;


			virtual void FillConstantDependencies(NOM_CONSTANT_DEPENCENCY_CONTAINER& result) override;
		};
	}
}


