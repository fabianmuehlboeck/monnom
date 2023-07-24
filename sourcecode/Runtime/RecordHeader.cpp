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
#include "PWRecord.h"
#include "PWStructDict.h"

using namespace llvm;
using namespace std;
namespace Nom
{
	namespace Runtime
	{
		llvm::StructType* RecordHeader::GetFieldGroupType()
		{
			[[clang::no_destroy]] static llvm::StructType* fgst = StructType::create(LLVMCONTEXT, "RT_NOM_StructFieldGroup");
			static bool once = true;
			if (once)
			{
				once = false;
				fgst->setBody(
					arrtype(inttype(8),8),
					arrtype(REFTYPE,8)
				);
			}
			return fgst;
		}
		llvm::StructType* RecordHeader::GetLLVMType()
		{
			[[clang::no_destroy]] static llvm::StructType* shst = StructType::create(LLVMCONTEXT, "RT_NOM_StructHeader");
			static bool once = true;
			if (once)
			{
				once = false;
				shst->setBody(
					StructuralValueHeader::GetLLVMType(),													//common structural value stuff
					//arrtype(llvm::Type::getInt8Ty(LLVMCONTEXT), GetConcurrentDictionarySize()),			//instance dictionary
					POINTERTYPE.AsLLVMType(),																//instance dictionary
					arrtype(GetFieldGroupType(), 0)															//Field groups
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
			return PWRecord(objPointer).ReadStructDict(builder);
		}

		llvm::Value* RecordHeader::GenerateWriteField(NomBuilder& builder, llvm::Value* thisObj, PWCInt32 fieldindex, llvm::Value* value, [[maybe_unused]] size_t fieldCount)
		{
			PWRecord(thisObj).WriteField(builder, fieldindex, value);
			return nullptr;
		}
		llvm::Value* RecordHeader::GenerateWriteWrittenTag(NomBuilder& builder, llvm::Value* thisObj, PWCInt32 fieldindex, [[maybe_unused]] size_t fieldCount)
		{
			PWRecord(thisObj).WriteWrittenTag(builder, fieldindex);
			return nullptr;
		}

		llvm::Value* RecordHeader::GenerateReadField(NomBuilder& builder, llvm::Value* thisObj, PWCInt32 fieldindex, [[maybe_unused]] size_t fieldCount)
		{
			return PWRecord(thisObj).ReadField(builder, fieldindex);
		}
		llvm::Value* RecordHeader::GenerateReadWrittenTag(NomBuilder& builder, llvm::Value* thisObj, PWCInt32 fieldIndex)
		{
			return PWRecord(thisObj).ReadWrittenTag(builder, fieldIndex);
		}
		llvm::Value* RecordHeader::GenerateReadTypeArgument(NomBuilder& builder, llvm::Value* thisObj, PWCInt32 argindex, [[maybe_unused]] bool hasRawInvoke)
		{
			return PWRecord(thisObj).ReadTypeArgument(builder, argindex);
		}
		llvm::Value* RecordHeader::GeneratePointerToTypeArguments(NomBuilder& builder, llvm::Value* thisObj, [[maybe_unused]] bool hasRawInvoke)
		{
			return PWRecord(thisObj).PointerToTypeArguments(builder);
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

			MakeInvariantStore(builder, ConstantPointerNull::get(POINTERTYPE), GetLLVMType(), newmem, MakeInt32(StructHeaderFields::InstanceDictionary), AtomicOrdering::Unordered);

			for (size_t i = 0; i < (env->Record->Fields.size() + 7) / 8; i++)
			{
				builder->CreateMemSet(builder->CreateGEP(GetLLVMType(), newmem, { MakeInt32(0), MakeInt32(StructHeaderFields::Fields), MakeInt32(i), MakeInt32(0) }), MakeUInt(8, 0), 8, Align(8));
			}
			StructuralValueHeader::GenerateInitializationCode(builder, newmem, typeArguments, vTablePtr, rawInvokePtr);

			auto refValHeader = builder->CreateGEP(GetLLVMType(), newmem, { MakeInt32(0), MakeInt32(StructHeaderFields::StructValueHeader), MakeInt32(StructuralValueHeaderFields::RefValueHeader) }, "RefValHeader");

			builder->CreateRet(refValHeader);
		}
	}
}
