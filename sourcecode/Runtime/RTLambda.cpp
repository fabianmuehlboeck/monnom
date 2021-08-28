#include "RTLambda.h"
#include "RTDescriptor.h"
#include "NomLambda.h"
#include "CompileHelpers.h"
#include "RTSignature.h"
#include "RTVTable.h"
#include "NomPartialApplication.h"
#include "RTSTable.h"

using namespace llvm;
using namespace std;
namespace Nom
{
	namespace Runtime
	{
		llvm::StructType* RTLambda::GetLLVMType()
		{
			static StructType* ltype = StructType::create(LLVMCONTEXT, "RT_NOM_Lambda");
			static bool once = true;
			if (once)
			{
				once = false;
				ltype->setBody(
					RTSTable::GetLLVMType(),											//common parts of STables
					numtype(uint64_t),													// typeargcount in upper 32 bits, argcount in lower 32 bits
					numtype(size_t),													// number of preallocated type argument slots
					numtype(uint64_t),													// cast ID for specialized vtable
					RTVTable::GetLLVMType()->getPointerTo()								// specialized vtable
				);
			}
			return ltype;
		}
		llvm::Constant* RTLambda::CreateConstant(const NomLambda* lambda, int typeArgCount, int argCount, llvm::Constant* signature, llvm::Function* fun, llvm::Function* dispatcher)
		{
			auto targc = MakeInt32((uint32_t)lambda->Body.GetDirectTypeParametersCount());
			auto argc = MakeInt32((uint32_t)lambda->Body.GetArgumentCount());
			auto targcshl = ConstantExpr::getShl(ConstantExpr::getZExt(targc, numtype(uint64_t)), MakeInt<uint64_t>(32));
			auto combo = ConstantExpr::getAdd(targcshl, ConstantExpr::getZExt(argc, numtype(uint64_t)));
			return ConstantStruct::get(GetLLVMType(), RTSTable::GenerateConstant(signature, ConstantExpr::getPointerCast(fun, POINTERTYPE), dispatcher, MakeUInt(32, lambda->Body.GetDirectTypeParametersCount()), MakeUInt(32, lambda->Body.GetArgumentCount()), GetLLVMPointer(lambda)), combo, MakeInt<size_t>(0), MakeInt<uint64_t>(0), ConstantPointerNull::get(RTVTable::GetLLVMType()->getPointerTo()));
		}
		llvm::Value* RTLambda::GenerateReadIRPointer(NomBuilder& builder, llvm::Value* descriptor)
		{
			return RTSTable::GenerateReadNomIRLink(builder, descriptor);
			//return MakeLoad(builder, descriptor, RTLambda::GetLLVMType()->getPointerTo(), MakeInt32(RTLambdaFields::NomLink), "lambdaIR");
		}
		llvm::Value* RTLambda::GenerateReadPreallocatedSlots(NomBuilder& builder, llvm::Value* descriptor)
		{
			return MakeLoad(builder, descriptor, RTLambda::GetLLVMType()->getPointerTo(), MakeInt32(RTLambdaFields::PreallocatedSlots), "preAllocSlots");
		}
		llvm::Instruction* RTLambda::GenerateWritePreallocatedSlots(NomBuilder& builder, llvm::Value* descriptor, llvm::Value* newValue)
		{
			return builder->CreateAtomicRMW(AtomicRMWInst::BinOp::Max, builder->CreateGEP(builder->CreatePointerCast(descriptor, GetLLVMType()->getPointerTo()), { MakeInt32(0), MakeInt32(RTLambdaFields::PreallocatedSlots) }), EnsureIntegerSize(builder, newValue, bitsin(size_t)), AtomicOrdering::Monotonic);
			//return MakeStore(builder, newValue, descriptor, RTLambda::GetLLVMType()->getPointerTo(), MakeInt32(RTLambdaFields::PreallocatedSlots));
		}
		llvm::Value* RTLambda::GenerateReadSignature(NomBuilder& builder, llvm::Value* descriptor)
		{
			return RTSTable::GenerateReadSignature(builder, descriptor);
			//return MakeLoad(builder, descriptor, RTLambda::GetLLVMType()->getPointerTo(), MakeInt32(RTLambdaFields::Signature), "signature");
		}
		llvm::Value* RTLambda::GenerateReadDispatcherPointer(NomBuilder& builder, llvm::Value* descriptor)
		{
			return RTSTable::GenerateReadLambdaDispatcher(builder, descriptor);
			//return MakeLoad(builder, descriptor, RTLambda::GetLLVMType()->getPointerTo(), MakeInt32(RTLambdaFields::Dispatcher), "dispatcher");
		}
		llvm::Value* RTLambda::GenerateReadFunctionPointer(NomBuilder& builder, llvm::Value* descriptor)
		{
			return RTSTable::GenerateReadLambdaMethod(builder, descriptor);
			//return MakeLoad(builder, descriptor, RTLambda::GetLLVMType()->getPointerTo(), MakeInt32(RTLambdaFields::Function), "function");
		}
		llvm::Value* RTLambda::GenerateCheckArgCountsMatch(NomBuilder& builder, llvm::Value* descriptor, llvm::Value* typeArgCount, llvm::Value* argCount)
		{
			auto argcounts = MakeLoad(builder, descriptor, RTLambda::GetLLVMType()->getPointerTo(), MakeInt32(RTLambdaFields::ArgCounts), "argcounts");
			auto matchKey = builder->CreateAdd(builder->CreateShl(builder->CreateZExt(builder->CreateTrunc(typeArgCount, inttype(32)), inttype(64)), MakeInt(64, (uint64_t)32)), builder->CreateZExt(builder->CreateTrunc(argCount, inttype(32)), inttype(64)));
			return builder->CreateICmpEQ(argcounts, matchKey);
		}
		llvm::Value* RTLambda::GenerateReadSpecializedVTableCastID(NomBuilder& builder, llvm::Value* descriptor)
		{
			return MakeLoad(builder, descriptor, RTLambda::GetLLVMType()->getPointerTo(), MakeInt32(RTLambdaFields::SpecializedVTableCastID), "SpecializedVTableCastID");
		}
		llvm::Value* RTLambda::GenerateReadSpecializedVTable(NomBuilder& builder, llvm::Value* descriptor)
		{
			return MakeLoad(builder, descriptor, RTLambda::GetLLVMType()->getPointerTo(), MakeInt32(RTLambdaFields::SpecializedVTable), "specializedVtable");
		}
		void RTLambda::GenerateWriteSpecializedVTable(NomBuilder& builder, llvm::Value* descriptor, llvm::Value* castID, llvm::Value* vtable)
		{
			auto address = builder->CreateGEP(builder->CreatePointerCast(descriptor, RTLambda::GetLLVMType()->getPointerTo()), { MakeInt32(0), MakeInt32(RTLambdaFields::SpecializedVTable) });
			auto cmpx = builder->CreateAtomicCmpXchg(address, ConstantPointerNull::get(RTVTable::GetLLVMType()->getPointerTo()), builder->CreatePointerCast(vtable, RTVTable::GetLLVMType()->getPointerTo()), AtomicOrdering::Monotonic, AtomicOrdering::Monotonic);

			if (castID != nullptr)
			{
				BasicBlock* successBlock = BasicBlock::Create(LLVMCONTEXT, "cmpxSuccess", builder->GetInsertBlock()->getParent());
				BasicBlock* outBlock = BasicBlock::Create(LLVMCONTEXT, "cmpxFail_continue", builder->GetInsertBlock()->getParent());
				builder->CreateCondBr(builder->CreateExtractValue(cmpx, { 1 }, "cmpxIsSuccess"), successBlock, outBlock);

				builder->SetInsertPoint(successBlock);
				MakeStore(builder, castID, descriptor, RTLambda::GetLLVMType()->getPointerTo(), MakeInt32(RTLambdaFields::SpecializedVTableCastID));
				builder->CreateBr(outBlock);

				builder->SetInsertPoint(outBlock);
			}
		}
	}
}