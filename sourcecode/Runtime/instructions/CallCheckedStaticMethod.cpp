#include "CallCheckedStaticMethod.h"
#include "../NomAlloc.h"
#include "../NomStaticMethod.h"
#include "../CompileHelpers.h"
#include "CastInstruction.h"
#include <iostream>
#include "../CastStats.h"
#include "../NomClass.h"

using namespace std;
namespace Nom
{
	namespace Runtime
	{

		CallCheckedStaticMethod::CallCheckedStaticMethod(const ConstantID method, const ConstantID typeArgs, RegIndex reg) : NomValueInstruction(reg, OpCode::CallCheckedStatic), Method(method), TypeArgs(typeArgs)
		{
		}


		CallCheckedStaticMethod::~CallCheckedStaticMethod()
		{
		}
		void CallCheckedStaticMethod::Compile(NomBuilder& builder, CompileEnv* env, int lineno)
		{
			if (NomCastStats)
			{	
				builder->CreateCall(GetIncStaticMethodCalls(*builder->GetInsertBlock()->getParent()->getParent()), {});
			}

			env->basicBlockTerminated = false;

			/*
			Substitutes generic types for the actual types based on the context of the function call
			and the context of the generic types.
			*/

			NomSubstitutionContextMemberContext nscmc(env->Context);
			auto typeArgs = NomConstants::GetTypeList(this->TypeArgs)->GetTypeList(&nscmc);

			NomSubstitutionContextList substC = NomSubstitutionContextList(typeArgs);
			NomInstantiationRef<const NomStaticMethod> method = NomConstants::GetStaticMethod(Method)->GetStaticMethod(&substC);
			
			/*
			The signature of the method, i.e. name and types
			*/
			auto signature = method.Elem->Signature(&substC);

			/*
			Argument counts, type counts and total
			*/
			auto argcount = method.Elem->GetArgumentCount();
			auto targcount = typeArgs.size();

			auto oargcount = argcount + targcount;

			if (env->GetArgCount() != argcount)
			{
				throw new std::exception();
			}

			/*
			Array for arguments and their types.
			*/
			auto argarr = makealloca(Value*, method.Elem->GetArgumentCount() + typeArgs.size());

			/*
			Pointer to the function and the parameter array.
			*/
			llvm::Function* func = method.Elem->GetLLVMFunction(env->Module);
			llvm::ArrayRef<llvm::Type*> funcparams = func->getFunctionType()->params();

			for (int i = argcount - 1; i >= 0; i--)
			{
				/*
				Gets the value of the argument and converts the type from NomTypes to matching LLVM Types.
				*/
				NomValue arg = env->GetArgument(i);
				argarr[i + targcount] = CastInstruction::MakeCast(builder, env, arg, signature.ArgumentTypes[i]);
			}
			for (int i = targcount - 1; i >= 0; i--)
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

			/*
			Clear arguments, set the new arguments to be the modified arguments, and generate the function call 
			*/
			env->ClearArguments();
			auto args = llvm::ArrayRef<llvm::Value*>(argarr, method.Elem->GetArgumentCount() + typeArgs.size());
			auto call = GenerateFunctionCall(builder, *(env->Module), func, args, true);

			/*
			Return register
			*/
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



		void CallCheckedStaticMethod::CompileDirectly(NomString* className, NomString* methodName, llvm::ArrayRef<NomTypeRef> typeArgs, llvm::ArrayRef<NomTypeRef> argTypes, NomBuilder& builder, CompileEnv* env, int lineno)
		{
			if (NomCastStats)
			{
				builder->CreateCall(GetIncStaticMethodCalls(*builder->GetInsertBlock()->getParent()->getParent()), {});
			}

			env->basicBlockTerminated = false;

			/*
			Substitutes generic types for the actual types based on the context of the function call
			and the context of the generic types.
			*/


			


			NomSubstitutionContextMemberContext nscmc(env->Context);

			//TypeList list;
			NomSubstitutionContextList substC = NomSubstitutionContextList(argTypes);
			NomClass* c = NomClass::getClass(className);
			NomInstantiationRef<const NomStaticMethod> method = c->GetStaticMethod(methodName, typeArgs, argTypes);

			/*
			The signature of the method, i.e. name and types
			*/
			auto signature = method.Elem->Signature(&substC);

			/*
			Argument counts, type counts and total
			*/
			auto argcount = method.Elem->GetArgumentCount();
			auto targcount = typeArgs.size();

			auto oargcount = argcount + targcount;

			if (env->GetArgCount() != argcount)
			{
				throw new std::exception();
			}

			/*
			Array for arguments and their types.
			*/
			auto argarr = makealloca(Value*, method.Elem->GetArgumentCount() + typeArgs.size());

			/*
			Pointer to the function and the parameter array.
			*/
			llvm::Function* func = method.Elem->GetLLVMFunction(env->Module);
			llvm::ArrayRef<llvm::Type*> funcparams = func->getFunctionType()->params();

			for (int i = argcount - 1; i >= 0; i--)
			{
				/*
				Gets the value of the argument and converts the type from NomTypes to matching LLVM Types.
				*/
				NomValue arg = env->GetArgument(i);
				argarr[i + targcount] = CastInstruction::MakeCast(builder, env, arg, signature.ArgumentTypes[i]);
			}
			for (int i = targcount - 1; i >= 0; i--)
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

			/*
			Clear arguments, set the new arguments to be the modified arguments, and generate the function call
			*/
			env->ClearArguments();
			auto args = llvm::ArrayRef<llvm::Value*>(argarr, method.Elem->GetArgumentCount() + typeArgs.size());
			auto call = GenerateFunctionCall(builder, *(env->Module), func, args, true);

			/*
			Return register
			*/
			RegisterValue(env, NomValue(call, method.Elem->GetReturnType(&substC), true));
		}
	}
}