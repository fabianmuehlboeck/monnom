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
#include "../RTStruct.h"
#include "../StructHeader.h"
#include "../CallingConvConf.h"
#include "../BoolClass.h"
#include "../CastStats.h"

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



		//llvm::Value* CallDispatchBestMethod::GenerateBestInvoke(NomBuilder& builder, llvm::Value* receiver, uint32_t typeargcount, uint32_t argcount, llvm::ArrayRef<llvm::Value*> args)
		//{
		//	if (NomCastStats)
		//	{
		//		builder->CreateCall(GetIncDynamicInvokes(*builder->GetInsertBlock()->getParent()->getParent()), {});
		//	}
		//	auto dispatcherPHI = GenerateGetBestInvokeDispatcherDyn(builder, receiver, MakeInt32(typeargcount), MakeInt32(argcount));
		//	auto dispatcherPtr = builder->CreatePointerCast(builder->CreateExtractValue(dispatcherPHI, { 0 }), NomPartialApplication::GetDynamicDispatcherType(typeargcount, argcount)->getPointerTo());
		//	auto callinst = builder->CreateCall(NomPartialApplication::GetDynamicDispatcherType(typeargcount, argcount), dispatcherPtr, args, "[INVOKE]");
		//	callinst->setCallingConv(NOMCC);
		//	return callinst;
		//}

		//NomValue NVGenerateBestInvoke(NomBuilder& builder, CompileEnv* env, llvm::Value* receiver, TypeList typeargs/*, intptr_t** outerbuffer*/)
		//{
		//	uint32_t typeargcount = typeargs.size();
		//	uint32_t argcount = typeargcount + env->GetArgCount() + 1;
		//	Value** argbuf = makealloca(Value*, argcount + 1);
		//	uint32_t argbufpos = 0;
		//	while (argbufpos < typeargcount)
		//	{
		//		argbuf[argbufpos] = typeargs[argbufpos]->GetLLVMElement(*env->Module);
		//		argbufpos++;
		//	}
		//	argbuf[argbufpos] = receiver;
		//	argbufpos++;
		//	while (argbufpos < argcount)
		//	{
		//		argbuf[argbufpos] = env->GetArgument(argbufpos - typeargcount - 1);
		//		argbufpos++;
		//	}
		//	return NomValue(CallDispatchBestMethod::GenerateBestInvoke(builder, receiver, typeargcount, env->GetArgCount(), llvm::ArrayRef<Value*>(argbuf, argbufpos)), &NomDynamicType::Instance());
		//}


		void CallDispatchBestMethod::Compile(NomBuilder& builder, CompileEnv* env, int lineno)
		{
			auto methodName = NomConstants::GetString(MethodName)->GetText()->ToStdString();
			NomSubstitutionContextMemberContext nscmc(env->Context);
			auto typeargs = NomConstants::GetTypeList(TypeArguments)->GetTypeList(&nscmc);

			int typeargcount = typeargs.size();
			int argcount = env->GetArgCount();

			auto dispatcherPair = env->PopDispatchPair();

			//size_t dispargcount = typeargcount + argcount + 1;
			//Value** argbuf = makealloca(Value*, dispargcount);
			//size_t argbufpos = 0;
			//while (argbufpos < typeargcount)
			//{
			//	argbuf[argbufpos] = typeargs[argbufpos]->GetLLVMElement(*(env->Module));
			//	argbufpos++;
			//}
			//argbuf[argbufpos] = builder->CreateExtractValue(dispatcherPair, { 1 });
			//argbufpos++;
			//while (argbufpos < dispargcount)
			//{
			//	argbuf[argbufpos] = EnsurePacked(builder, env->GetArgument(argbufpos - typeargcount - 1));
			//	argbufpos++;
			//}

			size_t dispargcount = typeargcount + argcount;
			Value** argbuf = makealloca(Value*, 3 + RTConfig_NumberOfVarargsArguments);
			argbuf[0] = MakeInt32(typeargcount);
			argbuf[1] = MakeInt32(argcount);
			argbuf[2] = builder->CreateExtractValue(dispatcherPair, { 1 });
			for (decltype(RTConfig_NumberOfVarargsArguments) i = 0; i < RTConfig_NumberOfVarargsArguments; i++)
			{
				argbuf[i+3] = ConstantPointerNull::get(POINTERTYPE);
			}
			size_t argbufpos = 0;
			Value* extArgArray = nullptr;
			if (dispargcount > 3)
			{
				extArgArray = builder->CreateAlloca(POINTERTYPE, dispargcount - 2);
				argbuf[RTConfig_NumberOfVarargsArguments + 2] = builder->CreatePointerCast(extArgArray, POINTERTYPE);
			}
			while (argbufpos < typeargcount)
			{
				Value* targ = builder->CreatePointerCast(typeargs[argbufpos]->GetLLVMElement(*(env->Module)), POINTERTYPE);
				if (argbufpos < RTConfig_NumberOfVarargsArguments - (dispargcount <= RTConfig_NumberOfVarargsArguments ? 0 : 1))
				{
					argbuf[argbufpos + 3] = targ;
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
					argbuf[argbufpos + 3] = arg;
				}
				else
				{
					MakeStore(builder, arg, builder->CreateGEP(extArgArray, MakeInt32(argbufpos - (RTConfig_NumberOfVarargsArguments - 1))));
				}
				argbufpos++;
			}

			auto dispatcherCallInst = builder->CreateCall(NomPartialApplication::GetDynamicDispatcherType(/*typeargcount, env->GetArgCount()*/), builder->CreateExtractValue(dispatcherPair, { 0 }), llvm::ArrayRef<Value*>(argbuf, 3 + RTConfig_NumberOfVarargsArguments), methodName + "()");
			dispatcherCallInst->setCallingConv(NOMCC);
			env->ClearArguments();
			RegisterValue(env, NomValue(dispatcherCallInst, &NomDynamicType::Instance(), true));
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
