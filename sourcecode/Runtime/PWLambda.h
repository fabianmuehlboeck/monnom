#pragma once
#include "PWrapper.h"
#include "PWRefValue.h"
#include "PWStructVal.h"
#include "NomLambda.h"

namespace Nom
{
	namespace Runtime
	{
		class PWLambda : public PWStructVal
		{
		public:
			PWLambda(llvm::Value* wrapped) : PWStructVal(wrapped)
			{

			}
			llvm::Value* ReadField(NomBuilder& builder, int32_t fieldIndex);
			llvm::Value* WriteField(NomBuilder& builder, int32_t fieldIndex, llvm::Value* value);
		};

		class PWLambdaPrecise : public PWLambda
		{
		public : 
			const NomLambda* const lambda;
			PWLambdaPrecise(llvm::Value* wrapped, const NomLambda* lambda) : PWLambda(wrapped), lambda(lambda)
			{

			}
			llvm::Value* ReadLambdaField(NomBuilder& builder, int32_t fieldIndex);

		};
	}
}