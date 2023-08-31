#include "CreateClosure.h"
PUSHDIAGSUPPRESSION
#include "llvm/IR/Function.h"
POPDIAGSUPPRESSION
#include "../NomAlloc.h"
#include "../NomVMInterface.h"
#include "../NomLambda.h"
#include "../NomConstants.h"
#include "../RTLambda.h"
#include "../LambdaHeader.h"
#include "../NomDynamicType.h"
#include <iostream>
#include "../CallingConvConf.h"
#include "../CompileHelpers.h"

using namespace llvm;
using namespace std;
namespace Nom
{
	namespace Runtime
	{
		CreateClosure::CreateClosure(RegIndex reg, ConstantID lambda, ConstantID typeArgs) : NomValueInstruction(reg, OpCode::CreateClosure), Lambda(lambda), TypeArgsID(typeArgs)
		{
		}
		void CreateClosure::Compile(NomBuilder& builder, CompileEnv* env, [[maybe_unused]] size_t lineno)
		{
			NomLambda* lambda = NomConstants::GetLambda(Lambda)->GetLambda();
			auto targcount = lambda->GetTypeParametersCount();
			llvm::Value** argbuf = makealloca(llvm::Value*, env->GetArgCount()+targcount);
			Function* constructorFun = lambda->GetLLVMElement(*env->Module);
			FunctionType* cft = constructorFun->getFunctionType();

			NomSubstitutionContextMemberContext nscmc(env->Context);
			auto argtypes = lambda->GetArgumentTypes(&nscmc);
			auto typeArgs = NomConstants::GetTypeList(TypeArgsID)->GetTypeList(&nscmc);
			for (decltype(targcount) i = 0; i < targcount; i++)
			{
				auto tparam = typeArgs[i]->GetLLVMElement(*builder->GetInsertBlock()->getParent()->getParent());
				argbuf[i] = tparam;
			}

			for (size_t i = 0; i < env->GetArgCount(); i++)
			{
				RTValuePtr nv = env->GetArgument(i);
				argbuf[i+targcount] = nv->EnsureType(builder, env, argtypes[i], cft->getParamType(static_cast<unsigned int>(i+targcount)));
				//argbuf[i] = env->GetArgument(i);
			}
			auto callinst = builder->CreateCall(constructorFun, llvm::ArrayRef<llvm::Value*>(argbuf, env->GetArgCount()+targcount), "closure");
			callinst->setCallingConv(NOMCC);
			env->ClearArguments();
			RegisterValue(env, RTValue::GetValue(builder, callinst, &NomDynamicType::Instance(), true));
		}
		void CreateClosure::Print(bool resolve)
		{
			cout << "Closure ";
			NomConstants::PrintConstant(Lambda, resolve);
			cout << " -> #" << std::dec << WriteRegister;
			cout << "\n";
		}
		void CreateClosure::FillConstantDependencies(NOM_CONSTANT_DEPENCENCY_CONTAINER& result)
		{
			result.push_back(this->Lambda);
		}
	}
}
