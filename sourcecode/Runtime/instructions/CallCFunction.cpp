#include "CallCFunction.h"
#include "../NomAlloc.h"
#include "../NomStaticMethod.h"
#include "../CompileHelpers.h"
#include "CastInstruction.h"
#include <iostream>
#include "../CastStats.h"
#include "../NomConstants.h"
#include "../CompileEnv.h"

using namespace std;
using namespace llvm;
namespace Nom
{
	namespace Runtime
	{

		CallCFunction::CallCFunction(const ConstantID function, const ConstantID typeArgs, RegIndex reg) : NomValueInstruction(reg, OpCode::CallCFun), Function(function), TypeArgs(typeArgs)
		{
		}


		CallCFunction::~CallCFunction()
		{
		}
		void CallCFunction::Compile(NomBuilder& builder, CompileEnv* env, int lineno)
		{
			if (NomCastStats)
			{
				builder->CreateCall(GetIncCFunctionCalls(*builder->GetInsertBlock()->getParent()->getParent()), {});
			}
			NomCFunctionConstant* ncfc = NomConstants::GetCFunction(Function);
			NomTypeListConstant* ntlc = NomConstants::GetTypeList(TypeArgs);
			llvm::Function* func = ncfc->GetCFunction(*env->Module);
			NomSubstitutionContextMemberContext nscmc(env->Context);
			auto typeArgs = ntlc->GetTypeList(&nscmc);

			NomSubstitutionContextList nscl(typeArgs);
;

			llvm::Value* argsarr[func->arg_size()];

			int argpos = 0;
			for (; argpos < typeArgs.size(); argpos++) {
				argsarr[argpos] = typeArgs[argpos]->GenerateRTInstantiation(builder, env);
			}
			for (; argpos < func->arg_size(); argpos++) {
				argsarr[argpos] = EnsurePackedUnpacked(builder, env->GetArgument(argpos - typeArgs.size()), func->getFunctionType()->getParamType(argpos));
			}

			auto callResult = builder->CreateCall(func, llvm::ArrayRef<llvm::Value*>(argsarr, func->arg_size()), "cFuncCall");
			callResult->setCallingConv(CallingConv::C);
			RegisterValue(env, NomValue(callResult, ncfc->GetReturnType(&nscl), true));
			env->ClearArguments();
		}
		void CallCFunction::Print(bool resolve)
		{
			cout << "CCall ";
			NomConstants::PrintConstant(Function, resolve);
			cout << " -> #" << std::dec << WriteRegister;
			cout << "\n";
		}
		void CallCFunction::FillConstantDependencies(NOM_CONSTANT_DEPENCENCY_CONTAINER& result)
		{
			result.push_back(Function);
			if (TypeArgs != 0)
			{
				result.push_back(TypeArgs);
			}
		}
	}
}