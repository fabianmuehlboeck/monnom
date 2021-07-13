#include "CastInstruction.h"
#include "../NomType.h"
#include "../IntClass.h"
#include "../FloatClass.h"
#include "../BoolClass.h"
#include "../RTOutput.h"
#include "../CompileHelpers.h"
#include "../RTCast.h"
#include <iostream>

using namespace llvm;
using namespace std;
namespace Nom
{
	namespace Runtime
	{
		CastInstruction::CastInstruction(const RegIndex reg, const RegIndex value, const ConstantID type) :NomValueInstruction(reg, OpCode::Cast), ValueRegister(value), TypeConstant(type)
		{

		}

		CastInstruction::~CastInstruction()
		{

		}

		NomValue CastInstruction::MakeCast(NomBuilder& builder, CompileEnv* env, NomValue val, NomTypeRef type)
		{
			auto valtype = val.GetNomType();
			if (valtype->IsSubtype(type))
			{
				return val;
			}
			Function* fun = builder->GetInsertBlock()->getParent();
			BasicBlock* success = BasicBlock::Create(LLVMCONTEXT, "castSuccess", fun);
			BasicBlock* fail = BasicBlock::Create(LLVMCONTEXT, "castFail", fun);
			auto castResult = RTCast::GenerateCast(builder, env, val, type);
			builder->CreateCondBr(castResult, success, fail);

			builder->SetInsertPoint(fail);
			static const char* cast_errorMessage = "Cast failed";
			builder->CreateCall(RTOutput_Fail::GetLLVMElement(*(env->Module)), GetLLVMPointer(cast_errorMessage))->setCallingConv(RTOutput_Fail::GetLLVMElement(*(env->Module))->getCallingConv());
			CreateDummyReturn(builder, env->Function);

			builder->SetInsertPoint(success);
			return NomValue(val, type, true);
			////if (val->getType()->isIntegerTy() || val->getType()->isFloatingPointTy())
			////{
			////	//TODO: make sure to handle unpacked ints/floats/bools?
			////	throw new std::exception();
			////}
			//if (!(valtype->IsDisjoint(NomIntClass::GetInstance()->GetType()) && valtype->IsDisjoint(NomIntClass::GetInstance()->GetType()) && valtype->IsDisjoint(NomIntClass::GetInstance()->GetType())))
			//{
			//	Function* fun = builder->GetInsertBlock()->getParent();
			//	BasicBlock* success = BasicBlock::Create(LLVMCONTEXT, "castSuccess", fun);
			//	BasicBlock* fail = BasicBlock::Create(LLVMCONTEXT, "castFail", fun);

			//	auto directcount = env->Context->GetDirectTypeArgumentCount();
			//	auto envcount = env->Context->GetTypeArgumentCount() - directcount;
			//	auto checkResult = RTValueSubtypingMergeEnvs::CreateValueSubtypingCheck(builder, *(env->Module), val, type->GetLLVMElement(*(env->Module)), env->GetEnvTypeArgumentArray(builder), MakeInt(env->GetEnvTypeArgumentCount()), env->GetLocalTypeArgumentArray(builder), MakeInt(env->GetLocalTypeArgumentCount()));
			//	builder->CreateCondBr(builder->CreateICmpEQ(checkResult, MakeInt(2,(uint64_t)0)), fail, success);

			//	builder->SetInsertPoint(fail);
			//	static const char * cast_errorMessage = "Cast failed";
			//	builder->CreateCall(RTOutput_Fail::GetLLVMElement(*(env->Module)), GetLLVMPointer(cast_errorMessage))->setCallingConv(RTOutput_Fail::GetLLVMElement(*(env->Module))->getCallingConv());
			//	CreateDummyReturn(builder, env->Function);

			//	builder->SetInsertPoint(success);

			//	return NomValue(val, type, true);

			//}
			//throw new std::exception();
			//MakeLoad(builder, builder->CreateGEP(val, {MakeInt32(0), MakeInt32((unsigned char)ObjectHeaderFields::ClassDescriptor})
			//return NomValue();
		}

		void CastInstruction::Compile(NomBuilder& builder, CompileEnv* env, int lineno)
		{
			NomSubstitutionContextMemberContext nscmc(env->Context);
			RegisterValue(env, MakeCast(builder, env, (*env)[ValueRegister], NomConstants::GetType(&nscmc, TypeConstant)));
		}

		void CastInstruction::Print(bool resolve)
		{
			cout << "Cast #" << std::dec << ValueRegister;
			cout << " to ";
			NomConstants::PrintConstant(TypeConstant, resolve);
			cout << " -> #" << std::dec << WriteRegister;
			cout << "\n";
		}

		void CastInstruction::FillConstantDependencies(NOM_CONSTANT_DEPENCENCY_CONTAINER& result)
		{
			result.push_back(TypeConstant);
		}

	}
}