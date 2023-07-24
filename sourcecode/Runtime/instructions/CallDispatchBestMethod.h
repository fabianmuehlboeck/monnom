#pragma once
#include "../NomValueInstruction.h"
#include "../Defs.h"
#include "../NomValue.h"

namespace Nom
{
	namespace Runtime
	{
		class CallDispatchBestMethod : public NomValueInstruction
		{
		public:
			const RegIndex Receiver;
			const ConstantID MethodName;
			const ConstantID TypeArguments;
			CallDispatchBestMethod(RegIndex reg, RegIndex receiver, ConstantID methodName, ConstantID typeArgs);
			virtual ~CallDispatchBestMethod() override;
			virtual void Compile(NomBuilder &builder, CompileEnv* env, size_t lineno) override;

			// Inherited via NomValueInstruction
			virtual void Print(bool resolve = false) override;

			virtual void FillConstantDependencies(NOM_CONSTANT_DEPENCENCY_CONTAINER& result) override;
		};

	}
}
