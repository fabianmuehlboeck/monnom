#include "ErrorInstruction.h"
#include <iostream>
#include "../RTOutput.h"
#include "../CompileHelpers.h"
#include "../CompileEnv.h"

using namespace std;
using namespace llvm;
namespace Nom
{
	namespace Runtime
	{
		ErrorInstruction::ErrorInstruction(RegIndex reg) : NomInstruction(OpCode::Error), Register(reg)
		{
		}
		void ErrorInstruction::Compile(NomBuilder& builder, CompileEnv* env, [[maybe_unused]] size_t lineno)
		{
			static const char* generic_errorMessage = "ERROR!";
			builder->CreateCall(RTOutput_Fail::GetLLVMElement(*env->Module), GetLLVMPointer(generic_errorMessage))->setCallingConv(RTOutput_Fail::GetLLVMElement(*env->Module)->getCallingConv());
			CreateDummyReturn(builder, env->Function);
		}
		void ErrorInstruction::Print([[maybe_unused]] bool resolve)
		{
			cout << "ERROR <- " << std::dec << Register << "\n";
		}
		void ErrorInstruction::FillConstantDependencies([[maybe_unused]] NOM_CONSTANT_DEPENCENCY_CONTAINER& result)
		{
		}
	}
}
