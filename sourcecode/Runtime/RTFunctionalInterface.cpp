#include "RTFunctionalInterface.h"
#include "RTInterface.h"
#include "RTSignature.h"
#include "NomInterface.h"
#include "LambdaHeader.h"
#include "RTLambda.h"
#include "NomPartialApplication.h"
#include "RTLambdaInterface.h"
#include "CompileHelpers.h"
#include "RTOutput.h"
#include "RTVTable.h"
#include "RefValueHeader.h"
#include "RTCast.h"
#include "CompileEnv.h"
#include "NomDynamicType.h"
#include "NomTypeParameter.h"
#include "CallingConvConf.h"
#include "IMT.h"
#include "RTCompileConfig.h"

using namespace llvm;
using namespace std;
namespace Nom
{
	namespace Runtime
	{
		class LambdaCheckCompileEnv : public ACompileEnv
		{
		private:
			Value* castTypeArgs;
		public:
			LambdaCheckCompileEnv(llvm::Function* function, llvm::ArrayRef<NomTypeParameterRef> typeParams, const NomMemberContext* context, TypeList argTypes, Value* castTypeArgs) :ACompileEnv(argTypes.size() + 1, "", function, nullptr, typeParams, context, argTypes, &NomDynamicType::Instance()), castTypeArgs(castTypeArgs)
			{

			}
			// Inherited via CompileEnv
			virtual NomTypeVarValue GetTypeArgument(NomBuilder& builder, int i) override
			{
				if (((size_t)i) < Context->GetTypeParametersCount())
				{
					return NomTypeVarValue(MakeLoad(builder, builder->CreateGEP(castTypeArgs, MakeInt32(-(i + 1))), "typeArg"), Context->GetTypeParameter(i)->GetVariable());
				}
				return TypeArguments[i - Context->GetTypeParametersCount()];
			}
			virtual size_t GetEnvTypeArgumentCount() override
			{
				return Context->GetTypeParametersCount();
			}
			virtual llvm::Value* GetEnvTypeArgumentArray(NomBuilder& builder) override
			{
				return RTLambdaInterface::GetTypeArgumentsPointer(builder, RefValueHeader::GenerateReadVTablePointer(builder, registers[0]));
			}
		};

		//class OptLambdaCheckCompileEnv : public ACompileEnv
		//{
		//public:
		//	OptLambdaCheckCompileEnv(llvm::Function* function, llvm::ArrayRef<NomTypeParameterRef> typeParams, const NomMemberContext* context, TypeList argTypes) :ACompileEnv(argTypes.size() + 1, "", function, nullptr, typeParams, context, argTypes, &NomDynamicType::Instance())
		//	{

		//	}
		//	// Inherited via CompileEnv
		//	virtual NomTypeVarValue GetTypeArgument(NomBuilder& builder, int i) override
		//	{
		//		if (((size_t)i) < Context->GetTypeParametersCount())
		//		{
		//			return NomTypeVarValue(MakeLoad(builder, builder->CreateGEP(LambdaHeader::GeneratePointerToCastTypeArguments(builder, registers[0]), MakeInt32(-(i + 1))), "typeArg"), Context->GetTypeParameter(i)->GetVariable());
		//		}
		//		return TypeArguments[i - Context->GetTypeParametersCount()];
		//	}
		//	virtual size_t GetEnvTypeArgumentCount() override
		//	{
		//		return Context->GetTypeParametersCount();
		//	}
		//	virtual llvm::Value* GetEnvTypeArgumentArray(NomBuilder& builder) override
		//	{
		//		return LambdaHeader::GeneratePointerToCastTypeArguments(builder, registers[0]);
		//	}
		//};

		llvm::StructType* RTFunctionalInterface::GetLLVMType()
		{
			static llvm::StructType* rtfit = llvm::StructType::create(LLVMCONTEXT, "NOM_RT_FunctionalInterface");
			static bool once = true;
			if (once)
			{
				once = false;
				rtfit->setBody({ RTInterface::GetLLVMType(),					//regular interface representation
					POINTERTYPE,												//Raw invoke for perfect signature match
					POINTERTYPE,												//Raw invoke for prototype mismatch
					POINTERTYPE,												//Raw invoke for casting wrapper
					RTLambdaInterface::GetLLVMType()->getPointerTo(),			//optimized checked vtable
					RTLambdaInterface::GetLLVMType()->getPointerTo(),			//checked vtable
					RTLambdaInterface::GetLLVMType()->getPointerTo(),			//optimized prototype mismatch vtable
					RTLambdaInterface::GetLLVMType()->getPointerTo(),			//prototype mismatch vtable
					RTLambdaInterface::GetLLVMType()->getPointerTo(),			//optimized casting vtable
					RTLambdaInterface::GetLLVMType()->getPointerTo()			//casting vtable
					});
			}
			return rtfit;
		}
		llvm::Constant* RTFunctionalInterface::CreateGlobalConstant(llvm::Module& mod, GlobalValue::LinkageTypes linkage, const Twine& name, const NomInterface* irptr, llvm::Constant* typeArgCount, llvm::Constant* superTypesCount, llvm::Constant* superTypeEntries, llvm::Constant* instantiationDictionary)
		{
			llvm::StructType* gvartype = StructType::get(LLVMCONTEXT, { GetLLVMType() }, true);
			GlobalVariable* gvar = new GlobalVariable(mod, gvartype, true, linkage, nullptr, name);

			llvm::Constant* checkReturnValueFun = irptr->GetCheckReturnTypeFunction(mod, linkage);

			NomMethod* method = irptr->Methods[0];

			auto funtype = method->GetLLVMFunctionType();
			auto dispatcherType = NomPartialApplication::GetDynamicDispatcherType(/*method->GetDirectTypeParametersCount(), method->GetArgumentCount()*/);
			llvm::Function* checked = Function::Create(funtype, linkage, "NOM_RT_FICHECKED_" + *method->GetSymbolName(), &mod);
			llvm::Function* protoMismatch = Function::Create(funtype, linkage, "NOM_RT_FIPROTOMISMATCH_" + *method->GetSymbolName(), &mod);
			//llvm::Function* optCasting = Function::Create(funtype, linkage, "NOM_RT_FIOPTCASTING_" + *method->GetSymbolName(), &mod);
			llvm::Function* casting = Function::Create(funtype, linkage, "NOM_RT_FICASTING_" + *method->GetSymbolName(), &mod);


			checked->setCallingConv(NOMCC);
			protoMismatch->setCallingConv(NOMCC);
			//optCasting->setCallingConv(NOMCC);
			casting->setCallingConv(NOMCC);

			NomBuilder builder;

			BasicBlock* checkedStart = BasicBlock::Create(LLVMCONTEXT, "", checked);
			BasicBlock* protoMismatchStart = BasicBlock::Create(LLVMCONTEXT, "", protoMismatch);
			//BasicBlock* optCastingStart = BasicBlock::Create(LLVMCONTEXT, "", optCasting);
			BasicBlock* castingStart = BasicBlock::Create(LLVMCONTEXT, "", casting);
			//BasicBlock* optCastingSuccess = BasicBlock::Create(LLVMCONTEXT, "", optCasting);
			BasicBlock* castingSuccess = BasicBlock::Create(LLVMCONTEXT, "", casting);
			//BasicBlock* optCastingFail = BasicBlock::Create(LLVMCONTEXT, "", optCasting);
			BasicBlock* castingFail = BasicBlock::Create(LLVMCONTEXT, "", casting);

			static const char* cast_errorMessage = "Cast failed";

			{
				builder->SetInsertPoint(checkedStart);
				llvm::Argument* arg = checked->arg_begin();
				llvm::Value** argsBuf = makealloca(llvm::Value*, funtype->getNumParams());
				size_t pos = 0;
				argsBuf[pos] = arg;
				llvm::Value* lambdaDesc = LambdaHeader::GenerateReadLambdaMetadata(builder, arg);
				lambdaDesc->setName("lambdaDescriptor");
				llvm::Value* lambdaFun = builder->CreatePointerCast(RTLambda::GenerateReadFunctionPointer(builder, lambdaDesc), funtype->getPointerTo());
				lambdaDesc->setName("lambdaFunction");
				arg++;
				pos++;
				for (size_t s = method->GetDirectTypeParametersCount(); pos <= s; pos++, arg++)
				{
					argsBuf[pos] = arg;
				}
				for (; arg != checked->arg_end(); pos++, arg++)
				{
					argsBuf[pos] = arg;
				}
				CallInst* call = builder->CreateCall(funtype, lambdaFun, ArrayRef<Value*>(argsBuf, pos));
				call->setCallingConv(NOMCC);
				builder->CreateRet(call);
			}
			{
				builder->SetInsertPoint(protoMismatchStart);
				llvm::Argument* arg = protoMismatch->arg_begin();
				llvm::Value** argsBuf = makealloca(Value*, 3 + RTConfig_NumberOfVarargsArguments);
				auto targcount = method->GetDirectTypeParametersCount();
				auto argcount = method->GetArgumentCount();
				if (targcount + argcount > RTConfig_NumberOfVarargsArguments)
				{
					argsBuf[2 + RTConfig_NumberOfVarargsArguments] = builder->CreatePointerCast(builder->CreateAlloca(POINTERTYPE, MakeInt32(targcount + argcount - (RTConfig_NumberOfVarargsArguments-1))), POINTERTYPE);
				}
				else
				{
					argsBuf[2 + RTConfig_NumberOfVarargsArguments] = ConstantPointerNull::get(POINTERTYPE);
				}
				argsBuf[0] = MakeInt32(targcount);
				argsBuf[1] = MakeInt32(argcount);
				argsBuf[2] = arg;
				for (decltype(RTConfig_NumberOfVarargsArguments) i = 0; i < RTConfig_NumberOfVarargsArguments - 1; i++)
				{
					argsBuf[i+3] = ConstantPointerNull::get(POINTERTYPE);
				}
				size_t pos = 3;

				llvm::Value* lambdaDesc = LambdaHeader::GenerateReadLambdaMetadata(builder, arg);
				lambdaDesc->setName("lambdaDescriptor");
				llvm::Value* lambdaFun = builder->CreatePointerCast(RTLambda::GenerateReadDispatcherPointer(builder, lambdaDesc), dispatcherType->getPointerTo());
				lambdaDesc->setName("lambdaDispatcher");
				arg++;

				for (size_t s = targcount; pos <= s; pos++, arg++)
				{
					if (pos < 2 + RTConfig_NumberOfVarargsArguments || targcount + argcount == RTConfig_NumberOfVarargsArguments)
					{
						argsBuf[pos] = builder->CreatePointerCast(arg,POINTERTYPE);
					}
					else
					{
						MakeInvariantStore(builder, builder->CreatePointerCast(arg, POINTERTYPE), builder->CreateGEP(builder->CreatePointerCast(argsBuf[2 + RTConfig_NumberOfVarargsArguments], POINTERTYPE->getPointerTo()), MakeInt32(pos - (2 + RTConfig_NumberOfVarargsArguments))));
					}
				}
				for (; arg != protoMismatch->arg_end(); pos++, arg++)
				{
					if (pos < 2 + RTConfig_NumberOfVarargsArguments || targcount + argcount == RTConfig_NumberOfVarargsArguments)
					{
						argsBuf[pos] = builder->CreatePointerCast(EnsurePacked(builder, arg), POINTERTYPE);
					}
					else
					{
						MakeStore(builder, builder->CreatePointerCast(EnsurePacked(builder, arg), POINTERTYPE), builder->CreateGEP(builder->CreatePointerCast(argsBuf[2 + RTConfig_NumberOfVarargsArguments], POINTERTYPE->getPointerTo()), MakeInt32(pos - (2 + RTConfig_NumberOfVarargsArguments))));
					}
				}
				CallInst* call = builder->CreateCall(dispatcherType, lambdaFun, ArrayRef<Value*>(argsBuf, 3 + RTConfig_NumberOfVarargsArguments));
				call->setCallingConv(NOMCC);

				if (funtype->getReturnType() == INTTYPE)
				{
					builder->CreateRet(UnpackInt(builder, call));
				}
				else if (funtype->getReturnType() == FLOATTYPE)
				{
					builder->CreateRet(UnpackFloat(builder, call));
				}
				else if (funtype->getReturnType() == BOOLTYPE)
				{
					builder->CreateRet(UnpackBool(builder, call));
				}
				else
				{
					builder->CreateRet(call);
				}
			}

			//{
			//	builder->SetInsertPoint(optCastingStart);
			//	llvm::Argument* arg = optCasting->arg_begin();
			//	llvm::Value** argsBuf = makealloca(llvm::Value*, dispatcherType->getNumParams());
			//	size_t pos = 0;
			//	argsBuf[pos] = arg;
			//	llvm::Value* lambdaHeader = arg;
			//	llvm::Value* lambdaDesc = LambdaHeader::GenerateReadLambdaMetadata(builder, arg);
			//	lambdaDesc->setName("lambdaDescriptor");
			//	llvm::Value* lambdaFun = builder->CreatePointerCast(RTLambda::GenerateReadDispatcherPointer(builder, lambdaDesc), dispatcherType->getPointerTo());
			//	lambdaDesc->setName("lambdaDispatcher");
			//	arg++;
			//	pos++;
			//	for (size_t s = method->GetDirectTypeParametersCount(); pos <= s; pos++, arg++)
			//	{
			//		argsBuf[pos] = arg;
			//	}
			//	for (; arg != optCasting->arg_end(); pos++, arg++)
			//	{
			//		argsBuf[pos] = arg;
			//	}
			//	CallInst* call = builder->CreateCall(dispatcherType, lambdaFun, ArrayRef<Value*>(argsBuf, pos));
			//	call->setCallingConv(NOMCC);
			//	//auto newSubstStack = builder->CreateAlloca(RTSubtyping::TypeArgumentListStackType());
			//	//MakeStore(builder, ConstantPointerNull::get(RTSubtyping::TypeArgumentListStackType()->getPointerTo()), builder->CreateGEP(newSubstStack, { MakeInt32(0), MakeInt32(TypeArgumentListStackFields::Next) }));
			//	//MakeStore(builder, LambdaHeader::GeneratePointerToTypeArguments(builder, lambdaHeader), builder->CreateGEP(newSubstStack, { MakeInt32(0), MakeInt32(TypeArgumentListStackFields::Types) }));
			//	//RTValueSubtyping::CreateValueSubtypingCheck(builder, mod, call, method->GetReturnType()->GetLLVMElement(mod), newSubstStack);

			//	NomSubstitutionContextMemberContext nscmc(method);

			//	OptLambdaCheckCompileEnv env = OptLambdaCheckCompileEnv(optCasting, method->GetDirectTypeParameters(), irptr, method->GetArgumentTypes(&nscmc));
			//	auto subtCheckResult = RTCast::GenerateCast(builder, &env, call, method->GetReturnType(&nscmc));
			//	builder->CreateCondBr(builder->CreateICmpEQ(subtCheckResult, MakeInt(1, (uint64_t)1), "returnTypeOK"), optCastingSuccess, optCastingFail);

			//	builder->SetInsertPoint(optCastingSuccess);
			//	builder->CreateRet(call);
			//	builder->SetInsertPoint(optCastingFail);
			//	builder->CreateCall(RTOutput_Fail::GetLLVMElement(mod), GetLLVMPointer(cast_errorMessage))->setCallingConv(RTOutput_Fail::GetLLVMElement(mod)->getCallingConv());
			//	CreateDummyReturn(builder, optCasting);
			//}
			{
				builder->SetInsertPoint(castingStart);
				llvm::Argument* arg = casting->arg_begin();
				llvm::Value* argsBuf[6];
				auto targcount = method->GetDirectTypeParametersCount();
				auto argcount = method->GetArgumentCount();
				if (targcount + argcount > 3)
				{
					argsBuf[2 + RTConfig_NumberOfVarargsArguments] = builder->CreatePointerCast(builder->CreateAlloca(POINTERTYPE, MakeInt32(targcount + argcount - (RTConfig_NumberOfVarargsArguments - 1))), POINTERTYPE);
				}
				else
				{
					argsBuf[2 + RTConfig_NumberOfVarargsArguments] = ConstantPointerNull::get(POINTERTYPE);
				}
				argsBuf[0] = MakeInt32(targcount);
				argsBuf[1] = MakeInt32(argcount);
				argsBuf[2] = arg;
				for (decltype(RTConfig_NumberOfVarargsArguments) i = 0; i < RTConfig_NumberOfVarargsArguments - 1; i++)
				{
					argsBuf[i + 3] = ConstantPointerNull::get(POINTERTYPE);
				}
				size_t pos = 3;

				//llvm::Value* targspointer = RTLambdaInterface::GetTypeArgumentsPointer(builder, RefValueHeader::GenerateReadVTablePointer(builder, arg));
				//targspointer->setName("lambdaTypeArgs");
				llvm::Value* lambdaDesc = LambdaHeader::GenerateReadLambdaMetadata(builder, arg);
				lambdaDesc->setName("lambdaDescriptor");
				llvm::Value* lambdaFun = builder->CreatePointerCast(RTLambda::GenerateReadDispatcherPointer(builder, lambdaDesc), dispatcherType->getPointerTo());
				lambdaDesc->setName("lambdaDispatcher");
				arg++;

				for (size_t s = targcount; pos <= s; pos++, arg++)
				{
					if (pos < 2 + RTConfig_NumberOfVarargsArguments || targcount + argcount == RTConfig_NumberOfVarargsArguments)
					{
						argsBuf[pos] = builder->CreatePointerCast(arg, POINTERTYPE);
					}
					else
					{
						MakeInvariantStore(builder, builder->CreatePointerCast(arg, POINTERTYPE), builder->CreateGEP(builder->CreatePointerCast(argsBuf[2 + RTConfig_NumberOfVarargsArguments], POINTERTYPE->getPointerTo()), MakeInt32(pos - (2 + RTConfig_NumberOfVarargsArguments))));
					}
				}
				for (; arg != casting->arg_end(); pos++, arg++)
				{
					if (pos < 2 + RTConfig_NumberOfVarargsArguments || targcount + argcount == RTConfig_NumberOfVarargsArguments)
					{
						argsBuf[pos] = builder->CreatePointerCast(EnsurePacked(builder, arg), POINTERTYPE);
					}
					else
					{
						MakeStore(builder, builder->CreatePointerCast(EnsurePacked(builder, arg), POINTERTYPE), builder->CreateGEP(builder->CreatePointerCast(argsBuf[2 + RTConfig_NumberOfVarargsArguments], POINTERTYPE->getPointerTo()), MakeInt32(pos - (2 + RTConfig_NumberOfVarargsArguments))));
					}
				}
				CallInst* call = builder->CreateCall(dispatcherType, lambdaFun, ArrayRef<Value*>(argsBuf, 3 + RTConfig_NumberOfVarargsArguments));
				call->setCallingConv(NOMCC);
				//auto newSubstStack = builder->CreateAlloca(RTSubtyping::TypeArgumentListStackType());
				//MakeStore(builder, ConstantPointerNull::get(RTSubtyping::TypeArgumentListStackType()->getPointerTo()), builder->CreateGEP(newSubstStack, { MakeInt32(0), MakeInt32(TypeArgumentListStackFields::Next) }));
				//MakeStore(builder, targspointer, builder->CreateGEP(newSubstStack, { MakeInt32(0), MakeInt32(TypeArgumentListStackFields::Types) }));
				//auto subtCheckResult = RTValueSubtyping::CreateValueSubtypingCheck(builder, mod, call, method->GetReturnType()->GetLLVMElement(mod), newSubstStack);

				BasicBlock* checkReturnValueBlock = BasicBlock::Create(LLVMCONTEXT, "checkReturnValue", casting);

				auto castType = LambdaHeader::GenerateReadCastTypePointer(builder, argsBuf[2]);
				auto castTypeTag = builder->CreateTrunc(builder->CreatePtrToInt(castType, numtype(intptr_t)), inttype(1));
				builder->CreateCondBr(castTypeTag, castingFail, checkReturnValueBlock);

				builder->SetInsertPoint(checkReturnValueBlock);

				auto castTypeArgs = RTClassType::GetTypeArgumentsPtr(builder, castType);

				NomSubstitutionContextMemberContext nscmc(method);
				LambdaCheckCompileEnv env = LambdaCheckCompileEnv(casting, method->GetDirectTypeParameters(), irptr, method->GetArgumentTypes(&nscmc), castTypeArgs);
				auto subtCheckResult = RTCast::GenerateCast(builder, &env, call, method->GetReturnType(&nscmc));
				builder->CreateCondBr(builder->CreateICmpEQ(subtCheckResult, MakeInt(1, (uint64_t)1), "returnTypeOK"), castingSuccess, castingFail);

				builder->SetInsertPoint(castingSuccess);
				builder->CreateRet(call);

				builder->SetInsertPoint(castingFail);
				builder->CreateCall(RTOutput_Fail::GetLLVMElement(mod), GetLLVMPointer(cast_errorMessage))->setCallingConv(RTOutput_Fail::GetLLVMElement(mod)->getCallingConv());
				CreateDummyReturn(builder, casting);
			}


			llvm::Constant* optCheckedConstant = RTLambdaInterface::CreateGlobalConstant(mod, linkage, "NOM_RT_LIOPTCHECKED_" + name, RTDescriptorKind::OptimizedLambda, irptr, typeArgCount, superTypesCount, ConstantExpr::getGetElementPtr(((PointerType*)superTypeEntries->getType())->getElementType(), superTypeEntries, ArrayRef<Constant*>({ MakeInt32(0), MakeInt32(0) })), ConstantExpr::getGetElementPtr(gvartype, gvar, ArrayRef<Constant*>({ MakeInt32(0), MakeInt32(0) })), instantiationDictionary, checked, true);
			llvm::Constant* checkedConstant = RTLambdaInterface::CreateGlobalConstant(mod, linkage, "NOM_RT_LICHECKED_" + name, RTDescriptorKind::Lambda, irptr, typeArgCount, superTypesCount, ConstantExpr::getGetElementPtr(((PointerType*)superTypeEntries->getType())->getElementType(), superTypeEntries, ArrayRef<Constant*>({ MakeInt32(0), MakeInt32(0) })), ConstantExpr::getGetElementPtr(gvartype, gvar, ArrayRef<Constant*>({ MakeInt32(0), MakeInt32(0) })), instantiationDictionary, checked, false);
			llvm::Constant* optProtoMismatchConstant = RTLambdaInterface::CreateGlobalConstant(mod, linkage, "NOM_RT_LIOPTPROTOMISMATCH_" + name, RTDescriptorKind::OptimizedLambda, irptr, typeArgCount, superTypesCount, ConstantExpr::getGetElementPtr(((PointerType*)superTypeEntries->getType())->getElementType(), superTypeEntries, ArrayRef<Constant*>({ MakeInt32(0), MakeInt32(0) })), ConstantExpr::getGetElementPtr(gvartype, gvar, ArrayRef<Constant*>({ MakeInt32(0), MakeInt32(0) })), instantiationDictionary, protoMismatch, true);
			llvm::Constant* protoMismatchConstant = RTLambdaInterface::CreateGlobalConstant(mod, linkage, "NOM_RT_LIPROTOMISMATCH_" + name, RTDescriptorKind::Lambda, irptr, typeArgCount, superTypesCount, ConstantExpr::getGetElementPtr(((PointerType*)superTypeEntries->getType())->getElementType(), superTypeEntries, ArrayRef<Constant*>({ MakeInt32(0), MakeInt32(0) })), ConstantExpr::getGetElementPtr(gvartype, gvar, ArrayRef<Constant*>({ MakeInt32(0), MakeInt32(0) })), instantiationDictionary, protoMismatch, false);
			llvm::Constant* optCastingConstant = RTLambdaInterface::CreateGlobalConstant(mod, linkage, "NOM_RT_LIOPTCASTING_" + name, RTDescriptorKind::OptimizedLambda, irptr, typeArgCount, superTypesCount, ConstantExpr::getGetElementPtr(((PointerType*)superTypeEntries->getType())->getElementType(), superTypeEntries, ArrayRef<Constant*>({ MakeInt32(0), MakeInt32(0) })), ConstantExpr::getGetElementPtr(gvartype, gvar, ArrayRef<Constant*>({ MakeInt32(0), MakeInt32(0) })), instantiationDictionary, casting, true);
			llvm::Constant* castingConstant = RTLambdaInterface::CreateGlobalConstant(mod, linkage, "NOM_RT_LICASTING_" + name, RTDescriptorKind::Lambda, irptr, typeArgCount, superTypesCount, ConstantExpr::getGetElementPtr(((PointerType*)superTypeEntries->getType())->getElementType(), superTypeEntries, ArrayRef<Constant*>({ MakeInt32(0), MakeInt32(0) })), ConstantExpr::getGetElementPtr(gvartype, gvar, ArrayRef<Constant*>({ MakeInt32(0), MakeInt32(0) })), instantiationDictionary, casting, false);

			llvm::Constant** nullarr = makealloca(llvm::Constant*, IMTsize);
			for (decltype(IMTsize) i = 0; i < IMTsize; i++)
			{
				nullarr[i] = ConstantPointerNull::get(GetIMTFunctionType()->getPointerTo());
			}

			//TODO: actually generate check return value function
			gvar->setInitializer(llvm::ConstantStruct::get(gvartype, llvm::ConstantStruct::get(GetLLVMType(), RTInterface::CreateConstant(irptr, RTInterfaceFlags::IsInterface | RTInterfaceFlags::IsFunctional, typeArgCount, ConstantExpr::getSub(superTypesCount, MakeIntLike(superTypesCount, 1)), ConstantExpr::getGetElementPtr(((PointerType*)superTypeEntries->getType())->getElementType(), superTypeEntries, ArrayRef<Constant*>({ MakeInt32(0), MakeInt32(1) })), /*MakeInt<intptr_t>(1)*/ ConstantArray::get(arrtype(GetIMTFunctionType()->getPointerTo(), IMTsize), llvm::ArrayRef<llvm::Constant*>(nullarr, IMTsize)), checkReturnValueFun, ConstantPointerNull::get(GetMethodEnsureFunctionType()->getPointerTo()), instantiationDictionary, RTSignature::CreateGlobalConstant(mod, linkage, "RT_NOM_Signature_" + gvar->getName(), method)), ConstantExpr::getPointerCast(GenerateRawInvokeWrap(&mod,linkage,"",irptr,checked), POINTERTYPE), ConstantExpr::getPointerCast(GenerateRawInvokeWrap(&mod, linkage, "", irptr, protoMismatch), POINTERTYPE), ConstantExpr::getPointerCast(GenerateRawInvokeWrap(&mod, linkage, "", irptr, casting), POINTERTYPE), optCheckedConstant, checkedConstant, optProtoMismatchConstant, protoMismatchConstant, optCastingConstant, castingConstant)));
			return ConstantExpr::getGetElementPtr(gvartype, gvar, ArrayRef<llvm::Constant*>({ MakeInt32(0), MakeInt32(0) }));
		}

		llvm::Constant* RTFunctionalInterface::FindConstant(llvm::Module& mod, const StringRef name)
		{
			auto cnst = mod.getGlobalVariable(name);
			if (cnst == nullptr)
			{
				return nullptr;
			}
			return ConstantExpr::getGetElementPtr(cnst->getValueType(), cnst, ArrayRef<llvm::Constant*>({ MakeInt32(0), MakeInt32(0) }));
		}
		llvm::Value* RTFunctionalInterface::GenerateReadCheckedRawInvokeWrapper(NomBuilder& builder, llvm::Value* descriptorPtr)
		{
			return MakeInvariantLoad(builder, descriptorPtr, GetLLVMType()->getPointerTo(), MakeInt32(RTFunctionalInterfaceFields::CheckedWrapper), "checkedRawInvokeWrapper");
		}
		llvm::Value* RTFunctionalInterface::GenerateReadProtoMismatchRawInvokeWrapper(NomBuilder& builder, llvm::Value* descriptorPtr)
		{
			return MakeInvariantLoad(builder, descriptorPtr, GetLLVMType()->getPointerTo(), MakeInt32(RTFunctionalInterfaceFields::ProtoMismatchWrapper), "protoMismatchRawInvokeWrapper");
		}
		llvm::Value* RTFunctionalInterface::GenerateReadCastingRawInvokeWrapper(NomBuilder& builder, llvm::Value* descriptorPtr)
		{
			return MakeInvariantLoad(builder, descriptorPtr, GetLLVMType()->getPointerTo(), MakeInt32(RTFunctionalInterfaceFields::CastingWrapper), "castingRawInvokeWrapper");
		}
		llvm::Value* RTFunctionalInterface::GenerateReadRTSignature(NomBuilder& builder, llvm::Value* descriptorPtr)
		{
			return RTInterface::GenerateReadSignature(builder, descriptorPtr);
		}
		llvm::Value* RTFunctionalInterface::GenerateReadProtoMatchVTableOpt(NomBuilder& builder, llvm::Value* descriptorPtr)
		{
			return MakeLoad(builder, descriptorPtr, GetLLVMType()->getPointerTo(), MakeInt32(RTFunctionalInterfaceFields::CheckedVTableOpt), "CheckedVTableOpt");
		}
		llvm::Value* RTFunctionalInterface::GenerateReadProtoMatchVTable(NomBuilder& builder, llvm::Value* descriptorPtr)
		{
			return MakeLoad(builder, descriptorPtr, GetLLVMType()->getPointerTo(), MakeInt32(RTFunctionalInterfaceFields::CheckedVTable), "CheckedVTable");
		}
		llvm::Value* RTFunctionalInterface::GenerateReadProtoMismatchVTableOpt(NomBuilder& builder, llvm::Value* descriptorPtr)
		{
			return MakeLoad(builder, descriptorPtr, GetLLVMType()->getPointerTo(), MakeInt32(RTFunctionalInterfaceFields::ProtoMismatchVTableOpt), "ProtoMismatchVTableOpt");
		}
		llvm::Value* RTFunctionalInterface::GenerateReadProtoMismatchVTable(NomBuilder& builder, llvm::Value* descriptorPtr)
		{
			return MakeLoad(builder, descriptorPtr, GetLLVMType()->getPointerTo(), MakeInt32(RTFunctionalInterfaceFields::ProtoMismatchVTable), "ProtoMismatchVTable");
		}
		llvm::Value* RTFunctionalInterface::GenerateReadCastingVTableOpt(NomBuilder& builder, llvm::Value* descriptorPtr)
		{
			return MakeLoad(builder, descriptorPtr, GetLLVMType()->getPointerTo(), MakeInt32(RTFunctionalInterfaceFields::CastingVTableOpt), "CastingVTableOpt");
		}
		llvm::Value* RTFunctionalInterface::GenerateReadCastingVTable(NomBuilder& builder, llvm::Value* descriptorPtr)
		{
			return MakeLoad(builder, descriptorPtr, GetLLVMType()->getPointerTo(), MakeInt32(RTFunctionalInterfaceFields::CastingVTable), "CastingVTable");
		}
	}
}