#include "RTSignature.h"
#include "Context.h"
#include "Defs.h"
#include "CompileHelpers.h"
#include "NomCallable.h"
#include "NomTypeVar.h"
#include <iostream>
#include "llvm/Support/raw_os_ostream.h"
#include "llvm/IR/Verifier.h"
#include "NomTypeParameter.h"
#include "RTCast.h"
#include "RTSubtyping.h"
#include "CallingConvConf.h"

using namespace llvm;
using namespace std;
namespace Nom
{
	namespace Runtime
	{
		RTSignature::RTSignature()
		{
		}
		RTSignature& RTSignature::Instance()
		{
			static RTSignature rts; return rts;
		}
		llvm::StructType* RTSignature::GetLLVMType()
		{
			static StructType* sigt = StructType::create(LLVMCONTEXT, "RT_NOM_Signature");
			static bool once = true;
			if (once)
			{
				once = false;
				sigt->setBody(arrtype(TYPETYPE, 0), POINTERTYPE, TYPETYPE, inttype(32), inttype(32), arrtype(TYPETYPE, 0));
			}
			return sigt;
		}

		llvm::StructType* RTSignature::GetLLVMType(size_t argCount)
		{
			return StructType::get(arrtype(TYPETYPE, 0), POINTERTYPE, TYPETYPE, inttype(32), inttype(32), arrtype(TYPETYPE, argCount));
		}
		llvm::StructType* RTSignature::GetLLVMType(size_t typeArgCount, size_t argCount)
		{
			return StructType::get(LLVMCONTEXT, { arrtype(TYPETYPE, typeArgCount), GetLLVMType(argCount) }, true);
		}

		llvm::Value* RTSignature::GenerateReadReturnType(NomBuilder& builder, llvm::Value* signatureRef)
		{
			return MakeLoad(builder, signatureRef, GetLLVMType()->getPointerTo(), MakeInt32(RTSignatureFields::ReturnType), "returnType");
		}

		llvm::Value* RTSignature::GenerateReadTypeParamCount(NomBuilder& builder, llvm::Value* signatureRef)
		{
			return MakeLoad(builder, signatureRef, GetLLVMType()->getPointerTo(), MakeInt32(RTSignatureFields::TypeParamCount), "typeParamCount");
		}

		llvm::Value* RTSignature::GenerateReadParamCount(NomBuilder& builder, llvm::Value* signatureRef)
		{
			return MakeLoad(builder, signatureRef, GetLLVMType()->getPointerTo(), MakeInt32(RTSignatureFields::ParamCount), "paramCount");
		}

		llvm::Value* RTSignature::GenerateReadLLVMFunctionType(NomBuilder& builder, llvm::Value* signatureRef)
		{
			return MakeLoad(builder, signatureRef, GetLLVMType()->getPointerTo(), MakeInt32(RTSignatureFields::LLVMFunctionType), "llvmFunctionType");
		}

		llvm::Value* RTSignature::GenerateReadTypeParameter(NomBuilder& builder, llvm::Value* signatureRef, llvm::Value* index)
		{
			return MakeLoad(builder, signatureRef, GetLLVMType()->getPointerTo(), { MakeInt32(RTSignatureFields::TypeParameters), builder->CreateSub(MakeInt32(-1), index) }, "typeParam");
		}

		llvm::Value* RTSignature::GenerateReadParameter(NomBuilder& builder, llvm::Value* signatureRef, llvm::Value* index)
		{
			return MakeLoad(builder, signatureRef, GetLLVMType()->getPointerTo(), { MakeInt32(RTSignatureFields::ParameterTypes),index }, "paramType");
		}

		llvm::Constant* RTSignature::CreateGlobalConstant(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage, llvm::Twine name, const NomCallable* method)
		{
			auto typeArgCount = method->GetDirectTypeParametersCount();
			auto argCount = method->GetArgumentCount();
			Constant** typeArgBuf = makealloca(Constant*, typeArgCount);
			Constant** argBuf = makealloca(Constant*, argCount);

			auto tparams = method->GetAllTypeVariables();
			for (decltype(typeArgCount) i = 0; i != typeArgCount; i++)
			{
				typeArgBuf[i] = tparams[i]->GetLLVMElement(mod);
			}

			auto paramts = method->GetArgumentTypes(nullptr);
			for (decltype(argCount) i = 0; i != argCount; i++)
			{
				argBuf[i] = paramts[i]->GetLLVMElement(mod);
			}

			GlobalVariable* gvar = new GlobalVariable(mod, GetLLVMType(typeArgCount, argCount), true, linkage, nullptr, name);
			auto mainHead = ConstantStruct::get(GetLLVMType(argCount), ConstantArray::get(arrtype(TYPETYPE, 0), {}), GetLLVMPointer(method->GetLLVMFunctionType()), method->GetReturnType(nullptr)->GetLLVMElement(mod), MakeInt32((uint32_t)typeArgCount), MakeInt32((uint32_t)argCount), ConstantArray::get(arrtype(TYPETYPE, argCount), ArrayRef<Constant*>(argBuf, argCount)));
			gvar->setInitializer(ConstantStruct::get(GetLLVMType(typeArgCount, argCount), ConstantArray::get(arrtype(TYPETYPE, typeArgCount), ArrayRef<Constant*>(typeArgBuf, typeArgCount)), mainHead));
			return ConstantExpr::getPointerCast(ConstantExpr::getGetElementPtr(GetLLVMType(typeArgCount, argCount), gvar, ArrayRef<Constant*>({ MakeInt32(0), MakeInt32(1) })), GetLLVMType()->getPointerTo());
		}

		llvm::FunctionType* RTSignature::GetLLVMFunctionType()
		{
			static FunctionType* ft = FunctionType::get(inttype(2), { RTSignature::GetLLVMType()->getPointerTo(), RTSubtyping::TypeArgumentListStackType()->getPointerTo(), RTSignature::GetLLVMType()->getPointerTo(), RTSubtyping::TypeArgumentListStackType()->getPointerTo() }, false);
			return ft;
		}



		llvm::Function* RTSignature::createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const
		{
			FunctionType* funtype = GetLLVMFunctionType();
			Function* fun = Function::Create(funtype, linkage, "RT_NOM_SignatureSubtyping", &mod);
			fun->setCallingConv(NOMCC);

			NomBuilder builder;

			BasicBlock* startBlock = BasicBlock::Create(LLVMCONTEXT, "", fun);
			BasicBlock* checkTypeParametersBlock = BasicBlock::Create(LLVMCONTEXT, "checkTypeParameters", fun);
			BasicBlock* checkNextTypeParameterBlock = BasicBlock::Create(LLVMCONTEXT, "checkNextTypeParameter", fun);
			BasicBlock* checkParametersBlock = BasicBlock::Create(LLVMCONTEXT, "checkParameters", fun);
			BasicBlock* checkParametersOptBlock = BasicBlock::Create(LLVMCONTEXT, "checkParametersOptimistically", fun);
			BasicBlock* checkNextParameterBlock = BasicBlock::Create(LLVMCONTEXT, "checkNextParameter", fun);
			BasicBlock* checkReturnTypeBlock = BasicBlock::Create(LLVMCONTEXT, "checkReturnType", fun);
			BasicBlock* pessimisticReturnMatchBlock = BasicBlock::Create(LLVMCONTEXT, "pessimisticReturnMatch", fun);
			BasicBlock* optimisticReturnMatchBlock = BasicBlock::Create(LLVMCONTEXT, "optimisticReturnMatch", fun);

			BasicBlock* failBlock = BasicBlock::Create(LLVMCONTEXT, "fail", fun);

			builder->SetInsertPoint(startBlock);
			llvm::Argument* arg = fun->arg_begin();
			llvm::Value* leftSig = arg;
			arg++;
			llvm::Value* leftSubst = arg;
			arg++;
			llvm::Value* rightSig = arg;
			arg++;
			llvm::Value* rightSubst = arg;

			auto leftTPCount = GenerateReadTypeParamCount(builder, leftSig);
			auto rightTPCount = GenerateReadTypeParamCount(builder, rightSig);
			auto leftPCount = GenerateReadParamCount(builder, leftSig);
			auto rightPCount = GenerateReadParamCount(builder, rightSig);
			auto typeParamsCountMatch = builder->CreateICmpEQ(leftTPCount, rightTPCount, "typeParamsCountMatch");
			auto paramsCountMatch = builder->CreateICmpEQ(leftPCount, rightPCount, "paramsCountMatch");
			builder->CreateCondBr(builder->CreateAnd(typeParamsCountMatch, paramsCountMatch), checkTypeParametersBlock, failBlock);

			builder->SetInsertPoint(checkTypeParametersBlock);
			auto TPcountPHI = builder->CreatePHI(leftTPCount->getType(), 2, "tpcounter");
			auto TPpessimisticPHI = builder->CreatePHI(inttype(1), 2, "pessimisticTP");
			TPcountPHI->addIncoming(leftTPCount, startBlock);
			TPpessimisticPHI->addIncoming(MakeInt(1, (uint64_t)1), startBlock);
			builder->CreateCondBr(builder->CreateICmpEQ(TPcountPHI, ConstantInt::get(TPcountPHI->getType(), 0)), checkParametersBlock, checkNextTypeParameterBlock);

			builder->SetInsertPoint(checkNextTypeParameterBlock);
			auto tppos = builder->CreateSub(TPcountPHI, ConstantInt::get(TPcountPHI->getType(), 1));
			auto leftTP = GenerateReadTypeParameter(builder, leftSig, tppos);
			auto rightTP = GenerateReadTypeParameter(builder, rightSig, tppos);
			//TODO: implement checking type parameter matchup
			TPpessimisticPHI->addIncoming(TPpessimisticPHI, checkNextTypeParameterBlock);
			TPcountPHI->addIncoming(tppos, checkNextTypeParameterBlock);
			builder->CreateBr(checkTypeParametersBlock);

			builder->SetInsertPoint(checkParametersBlock);
			auto PcountPHI = builder->CreatePHI(leftPCount->getType(), 3, "pcounter");
			auto PpessimisticPHI = builder->CreatePHI(TPpessimisticPHI->getType(), 3, "pessimisticP");
			PcountPHI->addIncoming(leftPCount, checkTypeParametersBlock);
			PpessimisticPHI->addIncoming(TPpessimisticPHI, checkTypeParametersBlock);
			builder->CreateCondBr(builder->CreateICmpEQ(PcountPHI, ConstantInt::get(PcountPHI->getType(), 0)), checkReturnTypeBlock, checkNextParameterBlock);

			builder->SetInsertPoint(checkNextParameterBlock);
			auto ppos = builder->CreateSub(PcountPHI, ConstantInt::get(PcountPHI->getType(), 1));
			auto leftP = GenerateReadParameter(builder, leftSig, ppos);
			auto rightP = GenerateReadParameter(builder, rightSig, ppos);
			auto PsubtCheckResult = RTSubtyping::CreateTypeSubtypingCheck(builder, mod, rightP, leftP, rightSubst, leftSubst);
			PpessimisticPHI->addIncoming(PpessimisticPHI, checkNextParameterBlock);
			PcountPHI->addIncoming(ppos, checkNextParameterBlock);
			auto PsubtCheckSwitch = builder->CreateSwitch(PsubtCheckResult, failBlock, 2);
			PsubtCheckSwitch->addCase(ConstantInt::get(inttype(2), 3), checkParametersBlock);
			PsubtCheckSwitch->addCase(ConstantInt::get(inttype(2), 1), checkParametersOptBlock);

			builder->SetInsertPoint(checkParametersOptBlock);
			PpessimisticPHI->addIncoming(MakeInt(1, (uint64_t)0), checkParametersOptBlock);
			PcountPHI->addIncoming(ppos, checkParametersOptBlock);
			builder->CreateBr(checkParametersBlock);

			builder->SetInsertPoint(checkReturnTypeBlock);
			auto leftRet = GenerateReadReturnType(builder, leftSig);
			auto rightRet = GenerateReadReturnType(builder, rightSig);
			auto returnTypeCheckResult = RTSubtyping::CreateTypeSubtypingCheck(builder, mod, leftRet, rightRet, leftSubst, rightSubst);
			auto returnTypeCheckSwitch = builder->CreateSwitch(returnTypeCheckResult, failBlock, 2);
			returnTypeCheckSwitch->addCase(ConstantInt::get(inttype(2), 3), pessimisticReturnMatchBlock);
			returnTypeCheckSwitch->addCase(ConstantInt::get(inttype(2), 1), optimisticReturnMatchBlock);

			builder->SetInsertPoint(pessimisticReturnMatchBlock);
			builder->CreateRet(builder->CreateAdd(builder->CreateMul(builder->CreateZExt(PpessimisticPHI, inttype(2)), MakeInt(2, (uint64_t)2)), builder->CreateZExt(PpessimisticPHI, inttype(2))));

			builder->SetInsertPoint(optimisticReturnMatchBlock);
			builder->CreateRet(ConstantInt::get(inttype(2), 1));

			builder->SetInsertPoint(failBlock);
			builder->CreateRet(MakeInt(2, (uint64_t)0));

			llvm::raw_os_ostream out(std::cout);
			if (verifyFunction(*fun, &out))
			{
				out.flush();
				std::cout << "Could not verify RT_NOM_SignatureSubtyping!";
				fun->print(out);
				out.flush();
				std::cout.flush();
				throw new std::exception();
			}

			return fun;
		}

		llvm::Function* RTSignature::findLLVMElement(llvm::Module& mod) const
		{
			return mod.getFunction("RT_NOM_SignatureSubtyping");
		}

		llvm::Value* RTSignature::GenerateInlinedSignatureSubtyping(NomBuilder& builder, llvm::Value* signature, llvm::Value* leftSubst, NomInstantiationRef<NomCallable> rightMethod, llvm::Value* rightSubst, bool onlyPessimistic)
		{
			BasicBlock* incomingBlock = builder->GetInsertBlock();
			Function* fun = incomingBlock->getParent();
			auto subtypingFun = RTSubtyping::Instance().GetLLVMElement(*fun->getParent());
			BasicBlock* outBlock = BasicBlock::Create(LLVMCONTEXT, "inlineSignatureSubtypingOut", fun);
			builder->SetInsertPoint(outBlock);
			auto outPHI = builder->CreatePHI(inttype((onlyPessimistic ? 1 : 2)), (onlyPessimistic ? 2 : 3));

			
			BasicBlock* failBlock = BasicBlock::Create(LLVMCONTEXT, "inlineSignatureSubtypingFail", fun);
			builder->SetInsertPoint(failBlock);
			outPHI->addIncoming(MakeUInt(onlyPessimistic ? 1 : 2, 0), failBlock);
			builder->CreateBr(outBlock);


			BasicBlock* checkReturnTypeBlock = BasicBlock::Create(LLVMCONTEXT, "inlineSignatureSubtypingCheckReturnType", fun);
			builder->SetInsertPoint(incomingBlock);
			auto tpcount = RTSignature::GenerateReadTypeParamCount(builder, signature);
			auto paramcount = RTSignature::GenerateReadParamCount(builder, signature);
			auto pcounteq = builder->CreateAnd(builder->CreateICmpEQ(tpcount, MakeIntLike(tpcount, rightMethod.Elem->GetDirectTypeParametersCount())), builder->CreateICmpEQ(paramcount, MakeIntLike(paramcount, rightMethod.Elem->GetArgumentCount())));
			builder->CreateCondBr(pcounteq, checkReturnTypeBlock, failBlock);

			builder->SetInsertPoint(checkReturnTypeBlock);
			auto returnType = RTSignature::GenerateReadReturnType(builder, signature);

			BasicBlock* pessimisticNextBlock = BasicBlock::Create(LLVMCONTEXT, "inlineSignaturePessimisticNext", fun);
			BasicBlock* optimisticNextBlock = onlyPessimistic ? nullptr : BasicBlock::Create(LLVMCONTEXT, "inlineSignatureOptimisticNext", fun);

			NomSubstitutionContextList nscl(rightMethod.TypeArgs);
			auto rightReturnType = rightMethod.Elem->GetReturnType(&nscl);
			RTSubtyping::CreateInlineSubtypingCheck(builder, returnType, leftSubst, rightReturnType, rightSubst, pessimisticNextBlock, optimisticNextBlock, failBlock);

			auto argTypes = rightMethod.Elem->GetArgumentTypes(&nscl);
			int argIndex = 0;
			for (auto& methArgType : argTypes)
			{
				if (optimisticNextBlock != nullptr)
				{
					builder->SetInsertPoint(optimisticNextBlock);
					optimisticNextBlock = BasicBlock::Create(LLVMCONTEXT, "inlineSignatureOptimisticNext"+std::to_string(argIndex), fun);
					auto paramType = RTSignature::GenerateReadParameter(builder, signature, MakeInt32(argIndex));
					auto subtCall = builder->CreateCall(subtypingFun, { methArgType->GetLLVMElement(*fun->getParent()), paramType, rightSubst, leftSubst });
					subtCall->setCallingConv(subtypingFun->getCallingConv());
					builder->CreateCondBr(builder->CreateICmpEQ(subtCall, MakeIntLike(subtCall, 0)), failBlock, optimisticNextBlock);
				}
				builder->SetInsertPoint(pessimisticNextBlock);
				pessimisticNextBlock = BasicBlock::Create(LLVMCONTEXT, "inlineSignaturePessimisticNext" + std::to_string(argIndex), fun);
				auto paramType = RTSignature::GenerateReadParameter(builder, signature, MakeInt32(argIndex));
				auto subtCall = builder->CreateCall(subtypingFun, {methArgType->GetLLVMElement(*fun->getParent()), paramType, rightSubst, leftSubst});
				subtCall->setCallingConv(subtypingFun->getCallingConv());
				if (optimisticNextBlock != nullptr)
				{
					auto resultSwitch = builder->CreateSwitch(subtCall, failBlock, 2);
					resultSwitch->addCase(MakeIntLike(subtCall, 3), pessimisticNextBlock);
					resultSwitch->addCase(MakeIntLike(subtCall, 1), optimisticNextBlock);
				}
				else
				{
					builder->CreateCondBr(builder->CreateICmpEQ(subtCall, MakeIntLike(subtCall, 3)), pessimisticNextBlock, failBlock);
				}
			}
			builder->SetInsertPoint(pessimisticNextBlock);
			outPHI->addIncoming(MakeUInt(onlyPessimistic ? 1 : 2, onlyPessimistic ? 1 : 3), pessimisticNextBlock);
			builder->CreateBr(outBlock);

			if (optimisticNextBlock!=nullptr)
			{
				builder->SetInsertPoint(optimisticNextBlock);
				outPHI->addIncoming(MakeUInt(2, 1), optimisticNextBlock);
				builder->CreateBr(outBlock);
			}

			builder->SetInsertPoint(outBlock);
			return outPHI;
		}

	}
}