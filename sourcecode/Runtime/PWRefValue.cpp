#include "Defs.h"
#include "CompileHelpers.h"
#include "PWRefValue.h"
#include "RTConfig.h"
#include "IMT.h"
#include "IntClass.h"
#include "FloatClass.h"
#include "BoolClass.h"
#include "PWIMTFunction.h"

using namespace std;
using namespace llvm;
namespace Nom
{
	namespace Runtime
	{
		PWVTable PWRefValue::ReadVTable(NomBuilder& builder) const
		{
			return PWVTable(MakeInvariantLoad(builder, RefValueHeader::GetLLVMType(), wrapped, MakeInt32(RefValueHeaderFields::InterfaceTable)));
		}
		void PWRefValue::WriteVTable(NomBuilder& builder, PWVTable vtbl) const
		{
			MakeInvariantStore(builder, vtbl, RTVTable::GetLLVMType(), wrapped, MakeInt32(RefValueHeaderFields::InterfaceTable));
		}
		llvm::Value* PWRefValue::WriteVTableCMPXCHG(NomBuilder& builder, PWVTable vtbl, PWVTable orig) const
		{
			auto vtableAddress = builder->CreateGEP(RefValueHeader::GetLLVMType(), wrapped, { MakeInt32(0), MakeInt32(RefValueHeaderFields::InterfaceTable) });
			return builder->CreateAtomicCmpXchg(vtableAddress, orig, vtbl, MaybeAlign(8), AtomicOrdering::AcquireRelease, AtomicOrdering::Acquire);
		}
		llvm::Value* PWRefValue::ReadRawInvoke(NomBuilder& builder) const
		{
			if (NomLambdaOptimizationLevel == 0)
			{
				throw new std::exception();
			}
			return MakeInvariantLoad(builder, RefValueHeader::GetLLVMType(), wrapped, { MakeInt32(RefValueHeaderFields::RawInvoke),MakeInt32(0) });
		}
		void PWRefValue::WriteRawInvoke(NomBuilder& builder, llvm::Value* rawinvoke) const
		{
			if (NomLambdaOptimizationLevel == 0)
			{
				throw new std::exception();
			}
			MakeStore(builder, rawinvoke, RefValueHeader::GetLLVMType(), wrapped, { MakeInt32(RefValueHeaderFields::RawInvoke),MakeInt<int32_t>(0) });
		}
		llvm::AtomicCmpXchgInst* PWRefValue::WriteRawInvokeCMPXCHG(NomBuilder& builder, llvm::Value* rawinvoke, llvm::Value* orig) const
		{
			if (NomLambdaOptimizationLevel == 0)
			{
				throw new std::exception();
			}
			return builder->CreateAtomicCmpXchg(builder->CreateGEP(RefValueHeader::GetLLVMType(),wrapped, { MakeInt32(0), MakeInt32(RefValueHeaderFields::RawInvoke),MakeInt<int32_t>(0) }),
				orig,
				rawinvoke,
				MaybeAlign(8),
				llvm::AtomicOrdering::Monotonic,
				llvm::AtomicOrdering::Monotonic);
		}
		PWIMTFunction PWRefValue::GetIMTFunction(NomBuilder& builder, PWCInt32 idx, [[maybe_unused]] size_t lineno) const
		{
			Function* fun = builder->GetInsertBlock()->getParent();
			auto recreg = wrapped;

			BasicBlock* refValueBlock = nullptr, * packedIntBlock = nullptr, * packedFloatBlock = nullptr, * primitiveIntBlock = nullptr, * primitiveFloatBlock = nullptr, * primitiveBoolBlock = nullptr;

			BasicBlock* mergeBlock = nullptr;
			PHINode* tablePHI = nullptr;

			RefValueHeader::GenerateRefOrPrimitiveValueSwitch(builder, RTValue::GetValue(builder, recreg), &refValueBlock, &packedIntBlock, &packedFloatBlock, false, &primitiveIntBlock, nullptr, &primitiveFloatBlock, nullptr, &primitiveBoolBlock, nullptr);

			unsigned int count = (refValueBlock != nullptr ? 1 : 0) + (packedIntBlock != nullptr ? 1 : 0) + (packedFloatBlock != nullptr ? 1 : 0) + (primitiveIntBlock != nullptr ? 1 : 0) + (primitiveFloatBlock != nullptr ? 1 : 0) + (primitiveBoolBlock != nullptr ? 1 : 0);

			if (count == 0)
			{
				throw new std::exception();
			}
			if (count > 1)
			{
				mergeBlock = BasicBlock::Create(LLVMCONTEXT, "imtLookupMerge", fun);

				builder->SetInsertPoint(mergeBlock);
				tablePHI = builder->CreatePHI(NLLVMPointer(GetIMTFunctionType()), count, "imt");
			}

			if (refValueBlock != nullptr)
			{
				builder->SetInsertPoint(refValueBlock);
				Value* vtable = ReadVTable(builder);
				auto vte = RTVTable::GenerateReadInterfaceMethodTableEntry(builder, vtable, idx);
				if (count == 1)
				{
					return vte;
				}
				else
				{
					tablePHI->addIncoming(vte, builder->GetInsertBlock());
					builder->CreateBr(mergeBlock);
				}
			}
			if (packedIntBlock != nullptr)
			{
				builder->SetInsertPoint(packedIntBlock);
				auto clsConstant = NomIntClass::GetInstance()->GetLLVMElement(*fun->getParent());
				auto vte = RTVTable::GenerateReadInterfaceMethodTableEntry(builder, clsConstant, idx);
				if (count == 1)
				{
					return vte;
				}
				else
				{
					tablePHI->addIncoming(vte, builder->GetInsertBlock());
					builder->CreateBr(mergeBlock);
				}
			}
			if (primitiveIntBlock != nullptr)
			{
				builder->SetInsertPoint(primitiveIntBlock);
				auto clsConstant = NomIntClass::GetInstance()->GetLLVMElement(*fun->getParent());
				auto vte = RTVTable::GenerateReadInterfaceMethodTableEntry(builder, clsConstant, idx);
				if (count == 1)
				{
					return vte;
				}
				else
				{
					tablePHI->addIncoming(vte, builder->GetInsertBlock());
					builder->CreateBr(mergeBlock);
				}
			}
			if (packedFloatBlock != nullptr)
			{
				builder->SetInsertPoint(packedFloatBlock);
				auto clsConstant = NomFloatClass::GetInstance()->GetLLVMElement(*fun->getParent());
				auto vte = RTVTable::GenerateReadInterfaceMethodTableEntry(builder, clsConstant, idx);
				if (count == 1)
				{
					return vte;
				}
				else
				{
					tablePHI->addIncoming(vte, builder->GetInsertBlock());
					builder->CreateBr(mergeBlock);
				}
			}
			if (primitiveFloatBlock != nullptr)
			{
				builder->SetInsertPoint(primitiveFloatBlock);
				auto clsConstant = NomFloatClass::GetInstance()->GetLLVMElement(*fun->getParent());
				auto vte = RTVTable::GenerateReadInterfaceMethodTableEntry(builder, clsConstant, idx);
				if (count == 1)
				{
					return vte;
				}
				else
				{
					tablePHI->addIncoming(vte, builder->GetInsertBlock());
					builder->CreateBr(mergeBlock);
				}
			}
			if (primitiveBoolBlock != nullptr)
			{
				builder->SetInsertPoint(primitiveBoolBlock);
				auto clsConstant = NomBoolClass::GetInstance()->GetLLVMElement(*fun->getParent());
				auto vte = RTVTable::GenerateReadInterfaceMethodTableEntry(builder, clsConstant, idx);
				if (count == 1)
				{
					return vte;
				}
				else
				{
					tablePHI->addIncoming(vte, builder->GetInsertBlock());
					builder->CreateBr(mergeBlock);
				}
			}
			builder->SetInsertPoint(mergeBlock);
			return tablePHI;
		}
	}
}
