#pragma once
#include "NomInstruction.h"
#include "NomBuilder.h"
#include "CompileEnv.h"
#include "Defs.h"

namespace Nom
{
	namespace Runtime
	{
		class EnsureDynamicMethodInstruction : public NomInstruction
		{
		public:
			const ConstantID MethodName;
			const RegIndex Receiver;
			EnsureDynamicMethodInstruction(ConstantID methodNameID, RegIndex receiver);
			virtual ~EnsureDynamicMethodInstruction() override;

			static llvm::Value* GenerateGetBestInvokeDispatcherDyn(NomBuilder& builder, RTValuePtr receiver);

			// Inherited via NomInstruction
			virtual void Compile(NomBuilder& builder, CompileEnv* env, size_t lineno) override;
			virtual void Print(bool resolve = false) override;
			virtual void FillConstantDependencies(NOM_CONSTANT_DEPENCENCY_CONTAINER& result) override;
		};
	}
}
