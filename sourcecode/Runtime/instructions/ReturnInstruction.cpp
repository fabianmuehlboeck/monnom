#include "ReturnInstruction.h"
#include "llvm/IR/IRBuilder.h"
#include "../TypeOperations.h"
#include <iostream>

using namespace Nom::Runtime;
using namespace std;

void Nom::Runtime::ReturnInstruction::Compile(NomBuilder& builder, CompileEnv* env, int lineno)
{
	auto rtype = env->Function->getReturnType();
	auto rval = *((*env)[this->reg]);
	if (rval->getType() != rtype)
	{
		rval = EnsurePackedUnpacked(builder, rval, rtype);
	}
	//if (rtype == INTTYPE && rval->getType() != INTTYPE)
	//{
	//	rval = UnpackInt(builder, rval);
	//}
	//else if (rtype == FLOATTYPE && rval->getType() != FLOATTYPE)
	//{
	//	rval = UnpackFloat(builder, rval);
	//}
	//else if (rtype == BOOLTYPE && rval->getType() != BOOLTYPE)
	//{
	//	rval = UnpackBool(builder, rval);
	//}
	//else if (rtype != INTTYPE && rval->getType() == INTTYPE)
	//{
	//	rval = PackInt(builder, rval);
	//	rval = builder->CreatePointerCast(rval, rtype);
	//}
	//else if (rtype != FLOATTYPE && rval->getType() == FLOATTYPE)
	//{
	//	rval = PackFloat(builder, rval);
	//	rval = builder->CreatePointerCast(rval, rtype);
	//}
	//else if (rtype != BOOLTYPE && rval->getType() == BOOLTYPE)
	//{
	//	rval = PackBool(builder, rval);
	//	rval = builder->CreatePointerCast(rval, rtype);
	//}
	//else if(rtype == POINTERTYPE && rval->getType() == REFTYPE)
	//{
	//	rval = builder->CreatePointerCast(rval, rtype);
	//}
	else
	{
		if ((*env)[this->reg].IsFunctionCall())
		{
			if (((llvm::CallInst*)(*(*env)[this->reg]))->getCallingConv() == llvm::CallingConv::Fast)
			{
				((llvm::CallInst*)(*(*env)[this->reg]))->setTailCallKind(llvm::CallInst::TailCallKind::TCK_Tail);
			}
		}
	}
	builder->CreateRet(rval);
	env->basicBlockTerminated = true;
}

ReturnInstruction::~ReturnInstruction()
{
}

void Nom::Runtime::ReturnInstruction::Print(bool resolve)
{
	cout << "Return #" << std::dec << reg;
	cout << "\n";
}

void Nom::Runtime::ReturnInstruction::FillConstantDependencies(NOM_CONSTANT_DEPENCENCY_CONTAINER& result)
{
}
