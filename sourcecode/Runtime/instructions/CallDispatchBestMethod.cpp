#include "CallDispatchBestMethod.h"
#include "../NomConstants.h"
#include "../NomNameRepository.h"
#include "../RTDictionary.h"
#include "../CompileHelpers.h"
#include "../RTOutput.h"
#include "../NomPartialApplication.h"
#include "../RTVTable.h"
#include "../RefValueHeader.h"
#include "../ObjectHeader.h"
#include "../RTClass.h"
#include "../LambdaHeader.h"
#include "../RTLambda.h"
#include "../NomDynamicType.h"
#include <iostream>
#include "../IntClass.h"
#include "../FloatClass.h"
#include "../CallingConvConf.h"
#include "../BoolClass.h"
#include "../CastStats.h"
#include "../NomLambdaCallTag.h"
#include "../IMT.h"

using namespace llvm;
using namespace std;
namespace Nom
{
	namespace Runtime
	{
		CallDispatchBestMethod::CallDispatchBestMethod(RegIndex reg, RegIndex receiver, ConstantID methodName, ConstantID typeArgs) : NomValueInstruction(reg, OpCode::CallDispatchBest), Receiver(receiver), MethodName(methodName), TypeArguments(typeArgs)
		{
		}


		CallDispatchBestMethod::~CallDispatchBestMethod()
		{
		}

		void CallDispatchBestMethod::Compile(NomBuilder& builder, CompileEnv* env, int lineno)
		{
			auto methodName = NomConstants::GetString(MethodName)->GetText()->ToStdString();
			NomSubstitutionContextMemberContext nscmc(env->Context);
			auto typeargs = NomConstants::GetTypeList(TypeArguments)->GetTypeList(&nscmc);

			int typeargcount = typeargs.size();
			int argcount = env->GetArgCount();

			auto dispatcherPair = env->PopDispatchPair();

			size_t dispargcount = typeargcount + argcount;
			Value** argbuf = makealloca(Value*, 2 + RTConfig_NumberOfVarargsArguments);
			argbuf[0] = NomLambdaCallTag::GetCallTag(typeargcount, argcount)->GetLLVMElement(*builder->GetInsertBlock()->getParent()->getParent());
			argbuf[1] = builder->CreateExtractValue(dispatcherPair, { 1 });
			for (decltype(RTConfig_NumberOfVarargsArguments) i = 0; i < RTConfig_NumberOfVarargsArguments; i++)
			{
				argbuf[i+2] = ConstantPointerNull::get(POINTERTYPE);
			}
			size_t argbufpos = 0;
			Value* extArgArray = nullptr;
			if (dispargcount > RTConfig_NumberOfVarargsArguments)
			{
				extArgArray = builder->CreateAlloca(POINTERTYPE, dispargcount - 2);
				argbuf[RTConfig_NumberOfVarargsArguments + 2] = builder->CreatePointerCast(extArgArray, POINTERTYPE);
			}
			while (argbufpos < typeargcount)
			{
				Value* targ = builder->CreatePointerCast(typeargs[argbufpos]->GetLLVMElement(*(env->Module)), POINTERTYPE);
				if (argbufpos < RTConfig_NumberOfVarargsArguments - (dispargcount <= RTConfig_NumberOfVarargsArguments ? 0 : 1))
				{
					argbuf[argbufpos + 2] = targ;
				}
				else
				{
					MakeStore(builder, targ, builder->CreateGEP(extArgArray, MakeInt32(argbufpos - (RTConfig_NumberOfVarargsArguments - 1))));
				}
				argbufpos++;
			}
			while (argbufpos < dispargcount)
			{
				Value* arg = builder->CreatePointerCast(EnsurePacked(builder, env->GetArgument(argbufpos - typeargcount)), POINTERTYPE);
				if (argbufpos < RTConfig_NumberOfVarargsArguments - (dispargcount <= RTConfig_NumberOfVarargsArguments ? 0 : 1))
				{
					argbuf[argbufpos + 2] = arg;
				}
				else
				{
					MakeStore(builder, arg, builder->CreateGEP(extArgArray, MakeInt32(argbufpos - (RTConfig_NumberOfVarargsArguments - 1))));
				}
				argbufpos++;
			}

			auto dispatcherCallInst = builder->CreateCall(GetIMTFunctionType(), builder->CreateExtractValue(dispatcherPair, { 0 }), llvm::ArrayRef<Value*>(argbuf, 2 + RTConfig_NumberOfVarargsArguments), methodName + "()");
			dispatcherCallInst->setCallingConv(NOMCC);
			env->ClearArguments();
			RegisterValue(env, NomValue(EnsurePackedUnpacked(builder, dispatcherCallInst, REFTYPE), &NomDynamicType::Instance(), true));
		}



		void CallDispatchBestMethod::Print(bool resolve)
		{
			cout << "Dispatch #" << std::dec << Receiver;
			cout << " ";
			NomConstants::PrintConstant(MethodName, resolve);
			cout << "<";
			NomConstants::PrintConstant(TypeArguments, resolve);
			cout << "> -> #" << std::dec << WriteRegister;
			cout << "\n";
		}

		void CallDispatchBestMethod::FillConstantDependencies(NOM_CONSTANT_DEPENCENCY_CONTAINER& result)
		{
			result.push_back(MethodName);
			result.push_back(TypeArguments);
		}

	}
}
