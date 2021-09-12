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
			auto rettype = builder->GetInsertBlock()->getParent()->getFunctionType()->getReturnType();
			if (rettype != REFTYPE && rettype != POINTERTYPE)
			{
				throw new std::exception();
			}
			builder->CreateRet(llvm::ConstantExpr::getPointerCast(NomVoidObject::GetInstance()->GetLLVMElement(*(env->Module)), rettype));
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
