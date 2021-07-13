#include "ReturnVoidInstruction.h"
#include "../VoidClass.h"
#include "../Defs.h"
#include <iostream>

using namespace std;
namespace Nom
{
	namespace Runtime
	{

		ReturnVoidInstruction::ReturnVoidInstruction() : NomInstruction(OpCode::ReturnVoid)
		{
		}


		ReturnVoidInstruction::~ReturnVoidInstruction()
		{
		}

		void ReturnVoidInstruction::Compile(NomBuilder &builder, CompileEnv* env, int lineno)
		{
			builder->CreateRet(llvm::ConstantExpr::getPointerCast(NomVoidObject::GetInstance()->GetLLVMElement(*(env->Module)), REFTYPE));
			env->basicBlockTerminated = true;
		}
		void ReturnVoidInstruction::Print(bool resolve)
		{
			cout << "ReturnVoid\n";
		}
		void ReturnVoidInstruction::FillConstantDependencies(NOM_CONSTANT_DEPENCENCY_CONTAINER& result)
		{
		}
	}
}
