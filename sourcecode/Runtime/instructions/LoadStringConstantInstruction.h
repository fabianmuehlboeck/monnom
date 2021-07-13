#pragma once

#include "../NomValueInstruction.h"
#include "../Context.h"
#include "../NomConstants.h"
#include "../NomVMInterface.h"
#include "llvm/IR/Function.h"
#include <iostream>

namespace Nom
{
	namespace Runtime
	{

		class LoadStringConstantInstruction : public NomValueInstruction
		{
		private:
			const ConstantID constantID;
		public:
			LoadStringConstantInstruction(const RegIndex reg, const ConstantID constantID) : NomValueInstruction(reg, OpCode::LoadStringConstant), constantID(constantID)
			{

			}
			virtual ~LoadStringConstantInstruction()
			{
				
			}
			virtual void Compile(NomBuilder &builder, CompileEnv* env, int lineno) override;

			// Inherited via NomValueInstruction
			virtual void Print(bool resolve = false) override;


			virtual void FillConstantDependencies(NOM_CONSTANT_DEPENCENCY_CONTAINER& result) override;

			//{
			//	env->basicBlockTerminated = false;
			//	GenerateLLVMDebugPrint(builder, env->Module, "Loading String Constant\n");
			//	std::array<llvm::Value *, 1> args = { {llvm::ConstantInt::get(LLVMCONSTANTINDEXTYPE, constantID, false)} };
			//	llvm::Function * fun = GetCppLoadStringConstant(env->Module);
			//	llvm::FunctionType * ftype = fun->getFunctionType();
			//	for (int i = 0; i < ftype->getNumParams(); i++) {
			//		llvm::Type * ptype = ftype->getFunctionParamType(i);
			//		ptype->dump();
			//	}
			//	for (auto iter = fun->arg_begin(); iter != fun->arg_end(); iter++) {
			//		iter->getType()->dump();
			//	}
			//	RegisterValue(env, builder->CreateCall(fun, args));
			//}
		};

	}
}
