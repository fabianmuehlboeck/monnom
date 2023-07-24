#include "CallCheckedStaticMethod.h"
#include "../NomAlloc.h"
#include "../NomStaticMethod.h"
#include "../CompileHelpers.h"
#include "CastInstruction.h"
#include <iostream>
#include "../CastStats.h"

using namespace std;
namespace Nom
{
	namespace Runtime
	{

		CallCheckedStaticMethod::CallCheckedStaticMethod(const ConstantID _method, const ConstantID _typeArgs, RegIndex _reg) : NomValueInstruction(_reg, OpCode::CallCheckedStatic), Method(_method), TypeArgs(_typeArgs)
		{
		}


		CallCheckedStaticMethod::~CallCheckedStaticMethod()
		{
		}
		void CallCheckedStaticMethod::Compile(NomBuilder& builder, CompileEnv* env, [[maybe_unused]] size_t lineno)
		{
			if (NomCastStats)
			{
				builder->CreateCall(GetIncStaticMethodCalls(*builder->GetInsertBlock()->getParent()->getParent()), {});
			}
			env->basicBlockTerminated = false;
			NomSubstitutionContextMemberContext nscmc(env->Context);
			auto typeArgs = NomConstants::GetTypeList(this->TypeArgs)->GetTypeList(&nscmc);

			NomSubstitutionContextList substC = NomSubstitutionContextList(typeArgs);
			NomInstantiationRef<const NomStaticMethod> method = NomConstants::GetStaticMethod(Method)->GetStaticMethod(&substC);
			
			auto signature = method.Elem->Signature(&substC);

			auto argcount = method.Elem->GetArgumentCount();
			auto targcount = typeArgs.size();

			auto oargcount = argcount + targcount;

			if (env->GetArgCount() != argcount)
			{
				throw new std::exception();
			}

			auto argarr = makealloca(Value*, method.Elem->GetArgumentCount() + typeArgs.size());
			llvm::Function* func = method.Elem->GetLLVMFunction(env->Module);
			llvm::ArrayRef<llvm::Type*> funcparams = func->getFunctionType()->params();
			for (size_t i = 0; i < argcount; i++)
			{
				NomValue arg = env->GetArgument(i);
				argarr[i + targcount] = CastInstruction::MakeCast(builder, env, arg, signature.ArgumentTypes[i]);
			}
			for (size_t i = 0; i < targcount; i++)
			{
				auto targ = typeArgs[i];
				if (targ->ContainsVariables())
				{
					if (targ->GetKind() == TypeKind::TKVariable)
					{
						argarr[i] = targ->GenerateRTInstantiation(builder, env);
					}
					else
					{
						throw new std::exception();
					}
				}
				else
				{
					argarr[i] = targ->GetLLVMElement(*(env->Module));
				}
			}
			env->ClearArguments();
			auto args = llvm::ArrayRef<llvm::Value*>(argarr, method.Elem->GetArgumentCount() + typeArgs.size());
			auto call = GenerateFunctionCall(builder, *(env->Module), func, args, true);

			RegisterValue(env, NomValue(call, method.Elem->GetReturnType(&substC), true));
		}
		void CallCheckedStaticMethod::Print(bool resolve)
		{
			cout << "SCall ";
			NomConstants::PrintConstant(Method, resolve);
			cout << " -> #" << std::dec << WriteRegister;
			cout << "\n";
		}
		void CallCheckedStaticMethod::FillConstantDependencies(NOM_CONSTANT_DEPENCENCY_CONTAINER& result)
		{
			result.push_back(Method);
			result.push_back(TypeArgs);
		}
	}
}
