#include "CallDispatchBestMethod.h"
#include "../Intermediate_Representation/NomConstants.h"
#include "../Intermediate_Representation/NomNameRepository.h"
#include "../Runtime/RTDictionary.h"
#include "../Common/CompileHelpers.h"
#include "../Runtime/RTOutput.h"
#include "../Intermediate_Representation/Callables/NomPartialApplication.h"
#include "../Runtime_Data/VTables/RTVTable.h"
#include "../Runtime_Data/Headers/RefValueHeader.h"
#include "../Runtime_Data/Headers/ObjectHeader.h"
#include "../Runtime_Data/VTables/RTClass.h"
#include "../Runtime_Data/Headers/LambdaHeader.h"
#include "../Runtime_Data/VTables/RTLambda.h"
#include "../Intermediate_Representation/Types/NomDynamicType.h"
#include <iostream>
#include "../Library/IntClass.h"
#include "../Library/FloatClass.h"
#include "../Common/CallingConvConf.h"
#include "../Library/BoolClass.h"
#include "../Runtime/Statistics/CastStats.h"
#include "../Intermediate_Representation/CallTags/NomLambdaCallTag.h"
#include "../Runtime_Data/IMT.h"

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
