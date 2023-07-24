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
			PWLambda(llvm::Value* _wrapped) : PWStructVal(_wrapped)
			{

			}
			llvm::Value* ReadField(NomBuilder& builder, size_t fieldIndex);
			llvm::Value* WriteField(NomBuilder& builder, size_t fieldIndex, llvm::Value* value);
		};

		class PWLambdaPrecise : public PWLambda
		{
		public : 
			const NomLambda* const lambda;
			PWLambdaPrecise(llvm::Value* _wrapped, const NomLambda* _lambda) : PWLambda(_wrapped), lambda(_lambda)
			{

			}
			llvm::Value* ReadLambdaField(NomBuilder& builder, size_t fieldIndex);

		};
	}
}
