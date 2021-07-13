#include "DebugInstruction.h"
#include "../NomConstants.h"
#include <iostream>
#include "../NomVMInterface.h"

using namespace std;
namespace Nom
{
	namespace Runtime
	{

		DebugInstruction::DebugInstruction(const ConstantID message) : NomInstruction(OpCode::Debug), messageConstant(message)
		{
		}


		DebugInstruction::~DebugInstruction()
		{
		}

		void DebugInstruction::Compile(NomBuilder &builder, CompileEnv* env, int lineno)
		{
			GenerateLLVMDebugPrint(builder, env->Module, NomConstants::GetString(messageConstant)->GetText()->ToStdString());
			/*env->basicBlockTerminated = false;
			std::string * textptr = new std::string();
			std::array<llvm::Value *, 1> args = { { llvm::ConstantInt::get(Type::getIntNTy(LLVMCONTEXT, bitsin(uint64_t)), reinterpret_cast<uint64_t>(textptr), false) } };
			llvm::Function * fun = GetPrint(env->Module);*/
		}
		void DebugInstruction::Print(bool resolve)
		{
			cout << "DEBUG ";
			NomConstants::PrintConstant(messageConstant, resolve);
			cout << "\n";
		}
		void DebugInstruction::FillConstantDependencies(NOM_CONSTANT_DEPENCENCY_CONTAINER& result)
		{
			result.push_back(messageConstant);
		}
	}
}
