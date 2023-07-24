#include "ReturnInstruction.h"
#include "../TypeOperations.h"
#include <iostream>

using namespace Nom::Runtime;
using namespace std;

void Nom::Runtime::ReturnInstruction::Compile([[maybe_unused]] NomBuilder& builder, [[maybe_unused]] CompileEnv* env, [[maybe_unused]] size_t lineno)
{
	auto rtype = env->Function->getReturnType();
	auto rval = *((*env)[this->reg]);
	if (rval->getType() != rtype)
	{
		rval = EnsurePackedUnpacked(builder, rval, rtype);
	}
	else
	{
		if ((*env)[this->reg].IsFunctionCall())
		{
			if ((static_cast<llvm::CallInst*>(*(*env)[this->reg]))->getCallingConv() == llvm::CallingConv::Fast)
			{
				(static_cast<llvm::CallInst*>(*(*env)[this->reg]))->setTailCallKind(llvm::CallInst::TailCallKind::TCK_Tail);
			}
		}
	}
	builder->CreateRet(rval);
	env->basicBlockTerminated = true;
}

ReturnInstruction::~ReturnInstruction()
{
}

void Nom::Runtime::ReturnInstruction::Print([[maybe_unused]] bool resolve)
{
	cout << "Return #" << std::dec << reg;
	cout << "\n";
}

void Nom::Runtime::ReturnInstruction::FillConstantDependencies([[maybe_unused]] NOM_CONSTANT_DEPENCENCY_CONTAINER& result)
{
}
