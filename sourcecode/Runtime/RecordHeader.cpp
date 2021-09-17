#include "RecordHeader.h"
#include "RefValueHeader.h"
#include "CompileHelpers.h"
#include "NomVMInterface.h"
#include "NomRecord.h"
#include "NomField.h"
#include "NomDynamicType.h"
#include "RTCompileConfig.h"
#include "RTVTable.h"
#include "RTOutput.h"
#include "Metadata.h"
#include "StructuralValueHeader.h"
#include "RTDictionary.h"

using namespace llvm;
using namespace std;
namespace Nom
{
	namespace Runtime
	{
		llvm::StructType* RecordHeader::GetLLVMType()
		{
			static llvm::StructType* shst = StructType::create(LLVMCONTEXT, "RT_NOM_StructHeader");
			static bool once = true;
			if (once)
			{
				once = false;
				shst->setBody(
					StructuralValueHeader::GetLLVMType(),													//common structural value stuff
					//arrtype(llvm::Type::getInt8Ty(LLVMCONTEXT), GetConcurrentDictionarySize()),			//instance dictionary
					POINTERTYPE,																			//instance dictionary
					arrtype(numtype(char), 0),																//Field written tags
					arrtype(REFTYPE, 0)																		//Fields
				);
			}
			return shst;
		}
		llvm::StructType* RecordHeader::GetLLVMType(size_t fieldCount)
		{
			return llvm::StructType::get(LLVMCONTEXT, { StructuralValueHeader::GetLLVMType(), POINTERTYPE, arrtype(numtype(char), fieldCount), arrtype(REFTYPE, fieldCount) }, false);
		}

		llvm::Value* RecordHeader::GenerateReadStructDictionary(NomBuilder& builder, llvm::Value* objPointer)
		{
			return builder->CreateGEP(builder->CreatePointerCast(objPointer, GetLLVMType()->getPointerTo()), { MakeInt32(0), MakeInt32(StructHeaderFields::InstanceDictionary), MakeInt32(0) });
		}

		llvm::Value* RecordHeader::GenerateWriteField(NomBuilder& builder, llvm::Value* thisObj, int32_t fieldindex, llvm::Value* value, size_t fieldCount)
		{
			return MakeStore(builder, value, builder->CreatePointerCast(thisObj, GetLLVMType(fieldCount)->getPointerTo()), { MakeInt32(StructHeaderFields::Fields), MakeInt32(fieldindex) });
		}
		llvm::Value* RecordHeader::GenerateWriteWrittenTag(NomBuilder& builder, llvm::Value* thisObj, int32_t fieldindex, size_t fieldCount)
		{
			return MakeStore(builder, MakeUInt(8, 1), builder->CreatePointerCast(thisObj, GetLLVMType(fieldCount)->getPointerTo()), { MakeInt32(StructHeaderFields::WrittenTags), MakeInt32(fieldindex) });
		}

		llvm::Value* RecordHeader::GenerateReadField(NomBuilder& builder, llvm::Value* thisObj, int32_t fieldindex, size_t fieldCount)
		{
			return MakeLoad(builder, builder->CreatePointerCast(thisObj, GetLLVMType(fieldCount)->getPointerTo()), { MakeInt32(StructHeaderFields::Fields), MakeInt32(fieldindex) });
		}

		llvm::Value* RecordHeader::GenerateReadField(NomBuilder& builder, llvm::Value* thisObj, llvm::Value* fieldindex)
		{
			return MakeLoad(builder, builder->CreatePointerCast(thisObj, GetLLVMType()->getPointerTo()), { MakeInt32(StructHeaderFields::Fields), fieldindex });
		}
		llvm::Value* RecordHeader::GenerateReadWrittenTag(NomBuilder& builder, llvm::Value* thisObj, llvm::Value* fieldIndex)
		{
			return builder->CreateTrunc(MakeLoad(builder, builder->CreatePointerCast(thisObj, GetLLVMType()->getPointerTo()), { MakeInt32(StructHeaderFields::WrittenTags), fieldIndex }), inttype(1));
		}
		llvm::Value* RecordHeader::GenerateReadTypeArgument(NomBuilder& builder, llvm::Value* thisObj, int32_t argindex, bool hasRawInvoke)
		{
			return StructuralValueHeader::GenerateReadTypeArgument(builder, thisObj, MakeInt32(argindex));
		}
		llvm::Value* RecordHeader::GeneratePointerToTypeArguments(NomBuilder& builder, llvm::Value* thisObj, bool hasRawInvoke)
		{
			return StructuralValueHeader::GenerateReadTypeArgsPtr(builder, thisObj);
		}


		void RecordHeader::GenerateConstructorCode(NomBuilder& builder, llvm::ArrayRef<llvm::Value*> typeArguments, StructInstantiationCompileEnv* env, llvm::Constant* descriptorRef, const std::vector<NomInstruction*>* instructions, llvm::Constant* rawInvokePtr)
		{
			Function* fun = builder->GetInsertBlock()->getParent();
			Function* allocfun = GetRecordAlloc(fun->getParent());

			llvm::Value* newmem = builder->CreatePointerCast(builder->CreateCall(allocfun, { MakeInt<size_t>(env->Record->Fields.size()), MakeInt<size_t>(typeArguments.size()) }), GetLLVMType()->getPointerTo());

			(*env)[0] = NomValue(newmem, &NomDynamicType::Instance());

			for (auto instr : *instructions)
			{
				instr->Compile(builder, env, 0);
			}

			for (auto field : env->Record->Fields)
			{
				GenerateWriteField(builder, newmem, field->Index, EnsurePacked(builder, (*env)[field->ValueRegister]),  NomLambdaOptimizationLevel > 0 && env->Record->GetHasRawInvoke());
			}

			auto vTablePtr = ConstantExpr::getPointerCast(descriptorRef, RTVTable::GetLLVMType()->getPointerTo());

			MakeInvariantStore(builder, ConstantPointerNull::get(POINTERTYPE), newmem, MakeInt32(StructHeaderFields::InstanceDictionary), AtomicOrdering::NotAtomic);

			builder->CreateMemSet(builder->CreateGEP(newmem, { MakeInt32(0), MakeInt32(StructHeaderFields::WrittenTags) }), MakeUInt(8, 0), env->Record->Fields.size(), Align(8));
			StructuralValueHeader::GenerateInitializationCode(builder, newmem, typeArguments, vTablePtr, rawInvokePtr);

			auto refValHeader = builder->CreateGEP(newmem, { MakeInt32(0), MakeInt32(StructHeaderFields::StructValueHeader), MakeInt32(StructuralValueHeaderFields::RefValueHeader) }, "RefValHeader");

			builder->CreateRet(refValHeader);
		}
	}
}