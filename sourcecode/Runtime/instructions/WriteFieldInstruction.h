#pragma once
#include "../NomInstruction.h"

namespace Nom
{
	namespace Runtime
	{
		class WriteFieldInstruction : public NomInstruction
		{
		public:
			const RegIndex ValueRegister;
			const RegIndex Receiver;
			const ConstantID FieldName;
			const ConstantID ReceiverType; //Class
			WriteFieldInstruction(const RegIndex receiver, const RegIndex value, const ConstantID fieldName, const ConstantID type);
			~WriteFieldInstruction() override;

			// Inherited via NomInstruction
			virtual void Compile(NomBuilder& builder, CompileEnv* env, size_t lineno) override;

			// Inherited via NomInstruction
			virtual void Print(bool resolve = false) override;

			virtual void FillConstantDependencies(NOM_CONSTANT_DEPENCENCY_CONTAINER& result) override;
		};

	}
}
