#pragma once
#include "../instructions/NomInstruction.h"
#include "../Common/NomBuilder.h"
#include "../Common/CompileEnv.h"
#include "../Common/Defs.h"

namespace Nom
{
	namespace Runtime
	{
		class EnsureCheckedMethodInstruction : public NomInstruction
		{
		public:
			const ConstantID MethodName;
			const RegIndex Receiver;
			EnsureCheckedMethodInstruction(ConstantID methodNameID, RegIndex receiver);
			virtual ~EnsureCheckedMethodInstruction();

			// Inherited via NomInstruction
			virtual void Compile(NomBuilder& builder, CompileEnv* env, int lineno) override;
			virtual void Print(bool resolve = false) override;
			virtual void FillConstantDependencies(NOM_CONSTANT_DEPENCENCY_CONTAINER& result) override;
		};
	}
}