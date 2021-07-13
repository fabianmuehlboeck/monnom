#pragma once
#include "../Defs.h"
#include "../NomInstruction.h"

namespace Nom
{
	namespace Runtime
	{
		class RTCmdInstruction : public NomInstruction
		{
		private:
			const ConstantID cmdConstant;
		public:
			RTCmdInstruction(const ConstantID cmd);
			virtual ~RTCmdInstruction() override;
			virtual void Compile(NomBuilder& builder, CompileEnv* env, int lineno) override;

			// Inherited via NomInstruction
			virtual void Print(bool resolve = false) override;

			virtual void FillConstantDependencies(NOM_CONSTANT_DEPENCENCY_CONTAINER& result) override;
		};
	}
}

