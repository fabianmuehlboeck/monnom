#include "RTVTable.h"
#include "RTInterfaceTableEntry.h"
#include "Defs.h"
#include "CompileHelpers.h"
#include "RTDescriptor.h"
#include "IMT.h"
#include "ObjectHeader.h"
#include "StructHeader.h"
#include "LambdaHeader.h"
#include "RTStructInterface.h"
#include "RTLambdaInterface.h"
#include "RTInstanceType.h"
#include "RTOutput.h"
#include "RTConfig.h"
#include "RTInterface.h"

using namespace llvm;
using namespace std;
namespace Nom
{
	namespace Runtime
	{
		llvm::StructType* RTVTable::GetLLVMType()
		{
			static StructType* rtitt = StructType::create(LLVMCONTEXT, "RT_NOM_InterfaceVTable");
			static bool once = true;
			if (once)
			{
				once = false;
				rtitt->setBody(
					arrtype(TYPETYPE,0),										//Type arguments (for those cases where they are saved in the vtable, going up)
					arrtype(GetIMTFunctionType()->getPointerTo(), IMTsize),		//Interface method table
					GetCheckReturnValueFunctionType()->getPointerTo(),			//Return value checks
					numtype(RTDescriptorKind),									//Kind
					POINTERTYPE													//Nom IR descriptor
				);
			}
			return rtitt;
		}
		llvm::Constant* RTVTable::CreateConstant(RTDescriptorKind kind, const void* irlink, llvm::Constant* interfaceMethodTable, llvm::Constant* checkReturnValueFunction)
		{
			return ConstantStruct::get(GetLLVMType(), ConstantArray::get(arrtype(TYPETYPE, 0), {}), interfaceMethodTable, checkReturnValueFunction, MakeInt<RTDescriptorKind>(kind), GetLLVMPointer(irlink));
		}
		llvm::Value* RTVTable::GenerateReadKind(NomBuilder& builder, llvm::Value* vtablePtr)
		{
			return MakeInvariantLoad(builder, builder->CreatePointerCast(vtablePtr, GetLLVMType()->getPointerTo()), MakeInt32(RTVTableFields::Kind), "Kind", AtomicOrdering::NotAtomic);
		}
		llvm::Instruction* RTVTable::GenerateWriteKind(NomBuilder& builder, llvm::Value* vtablePtr, llvm::Value* kind)
		{
			return MakeStore(builder, kind, builder->CreatePointerCast(vtablePtr, GetLLVMType()->getPointerTo()), MakeInt32(RTVTableFields::Kind));
		}
		llvm::Value* RTVTable::GenerateReadNomIRLink(NomBuilder& builder, llvm::Value* vtablePtr)
		{
			return MakeInvariantLoad(builder, builder->CreatePointerCast(vtablePtr, GetLLVMType()->getPointerTo()), MakeInt32(RTVTableFields::NomIRLink), "NomIR", AtomicOrdering::NotAtomic);
		}
		llvm::Value* RTVTable::GenerateReadReturnValueCheckFunction(NomBuilder& builder, llvm::Value* vtablePtr)
		{
			return MakeInvariantLoad(builder, builder->CreateGEP(builder->CreatePointerCast(vtablePtr, GetLLVMType()->getPointerTo()), { MakeInt32(0), MakeInt32(RTVTableFields::ReturnValueCheckFunction) }),"returnValueCheckFunction", AtomicOrdering::NotAtomic);
		}
		llvm::Value* RTVTable::GenerateReadInterfaceMethodTableEntry(NomBuilder& builder, llvm::Value* vtablePtr, llvm::Constant* index)
		{
			return MakeInvariantLoad(builder, builder->CreateGEP(builder->CreatePointerCast(vtablePtr, GetLLVMType()->getPointerTo()), { MakeInt32(0), MakeInt32(RTVTableFields::InterfaceMethodTable), index }), "IMTEntry", AtomicOrdering::NotAtomic);
		}

		llvm::Value* RTVTable::GenerateReadMethodTableEntry(NomBuilder& builder, llvm::Value* vtablePtr, llvm::Value* offset)
		{
			return MakeInvariantLoad(builder, builder->CreateGEP(builder->CreatePointerCast(vtablePtr, GetLLVMType()->getPointerTo()), { MakeInt32(0), MakeInt32(RTVTableFields::MethodTable), offset }), "methodPointer", AtomicOrdering::NotAtomic);
		}

		llvm::Value* RTVTable::GetInterfaceTableEntryPointer(NomBuilder& builder, llvm::Value* vtablePtr, llvm::Value* offset, llvm::Value* index)
		{
			auto iftablehead = builder->CreateIntToPtr(builder->CreateSub(offset, ConstantInt::get(INTTYPE, 1)), arrtype(RTInterfaceTableEntry::GetLLVMType(), 0)->getPointerTo());
			return builder->CreateGEP(iftablehead, { MakeInt32(0), index }, "ifaceTableEntryPtr");
			//return builder->CreateGEP(builder->CreatePointerCast(vtablePtr, GetLLVMType()->getPointerTo()), { MakeInt32(0), MakeInt32(RTVTableFields::InterfaceTable), builder->CreateSub(builder->CreateSub(builder->CreateAShr(builder->CreateTrunc(offset, inttype(32)), MakeInt32(1)), MakeInt32(1)), index) });
			/*auto interfaceTableStart = builder->CreatePointerCast(builder->CreateGEP(builder->CreatePointerCast(vtablePtr, inttype(8)->getPointerTo()), ), RTInterfaceTableEntry::GetLLVMType()->getPointerTo());
			return builder->CreateGEP(interfaceTableStart, index, "ifaceTableEntryPtr");*/
		}

		void RTVTable::GenerateInitialization(NomBuilder& builder, llvm::Value* vtptr, /*llvm::Value* ifcoffset,*/ llvm::Value* imtptr, llvm::Value* kind, llvm::Value* irdesc)
		{
			llvm::Value* selfptr = builder->CreatePointerCast(vtptr, GetLLVMType()->getPointerTo());
			//MakeStore(builder, ifcoffset, selfptr, MakeInt32(RTVTableFields::InterfaceTableOffset));
			MakeInvariantStore(builder, imtptr, selfptr, MakeInt32(RTVTableFields::InterfaceMethodTable));
			MakeInvariantStore(builder, kind, selfptr, MakeInt32(RTVTableFields::Kind));
			MakeInvariantStore(builder, irdesc, selfptr, MakeInt32(RTVTableFields::NomIRLink));
		}

		//int RTVTable::GenerateVTableKindSwitch(NomBuilder& builder, llvm::Value* vtptr, llvm::BasicBlock** classObjectBlock, llvm::BasicBlock** lambdaBlock, llvm::BasicBlock** structBlock, llvm::BasicBlock** partialAppBlock, llvm::BasicBlock** multiCastBlock)
		//{
		//	return GenerateVTableKindSwitch(builder, vtptr, classObjectBlock, lambdaBlock, lambdaBlock, structBlock, structBlock, partialAppBlock, multiCastBlock);
		//}

		//int RTVTable::GenerateVTableKindSwitch(NomBuilder& builder, llvm::Value* vtptr, llvm::BasicBlock** classObjectBlock, llvm::BasicBlock** lambda_targsInObject_Block, llvm::BasicBlock** lambda_targsInVTable_Block, llvm::BasicBlock** struct_targsInObject_Block, llvm::BasicBlock** struct_targsInVTable_Block, llvm::BasicBlock** partialAppBlock, llvm::BasicBlock** multiCastBlock)
		//{
		//	BasicBlock* origBlock = builder->GetInsertBlock();
		//	Function* fun = origBlock->getParent();
		//	
		//	BasicBlock* errorBlock = RTOutput_Fail::GenerateFailOutputBlock(builder, "Unhandled vtable type");

		//	BasicBlock* _classObjectBlock = errorBlock, * _lambda_targsInObject_Block = errorBlock, * _lambda_targsInVTable_Block = errorBlock, * _struct_targsInObject_Block = errorBlock, * _struct_targsInVTable_Block = errorBlock, * _partialAppBlock = errorBlock, * _multiCastBlock = errorBlock;
		//	auto countret = 0;
		//	if (classObjectBlock != nullptr)
		//	{
		//		_classObjectBlock = BasicBlock::Create(LLVMCONTEXT, "classObject", fun);
		//		*classObjectBlock = _classObjectBlock;
		//		countret++;
		//	}
		//	if (lambda_targsInObject_Block != nullptr)
		//	{
		//		_lambda_targsInObject_Block = BasicBlock::Create(LLVMCONTEXT, "lambda_targsInObject", fun);
		//		*lambda_targsInObject_Block = _lambda_targsInObject_Block;
		//		countret++;
		//	}
		//	if (lambda_targsInVTable_Block != nullptr)
		//	{
		//		if (lambda_targsInVTable_Block == lambda_targsInObject_Block)
		//		{
		//			_lambda_targsInVTable_Block = _lambda_targsInObject_Block;
		//		}
		//		else
		//		{
		//			_lambda_targsInVTable_Block = BasicBlock::Create(LLVMCONTEXT, "lambda_targsInVTable", fun);
		//			*lambda_targsInVTable_Block = _lambda_targsInVTable_Block;
		//			countret++;
		//		}
		//	}
		//	if (struct_targsInObject_Block != nullptr)
		//	{
		//		_struct_targsInObject_Block = BasicBlock::Create(LLVMCONTEXT, "struct_targsInObject", fun);
		//		*struct_targsInObject_Block = _struct_targsInObject_Block;
		//		countret++;
		//	}
		//	if (struct_targsInVTable_Block != nullptr)
		//	{
		//		if (struct_targsInVTable_Block == struct_targsInObject_Block)
		//		{
		//			_struct_targsInVTable_Block = _struct_targsInObject_Block;
		//		}
		//		else
		//		{
		//			_struct_targsInVTable_Block = BasicBlock::Create(LLVMCONTEXT, "struct_targsInVTable", fun);
		//			*struct_targsInVTable_Block = _struct_targsInVTable_Block;
		//			countret++;
		//		}
		//	}
		//	if (partialAppBlock != nullptr)
		//	{
		//		_partialAppBlock = BasicBlock::Create(LLVMCONTEXT, "partialApp", fun);
		//		*partialAppBlock = _partialAppBlock;
		//		countret++;
		//	}
		//	if (multiCastBlock != nullptr)
		//	{
		//		_multiCastBlock = BasicBlock::Create(LLVMCONTEXT, "multiCast", fun);
		//		*multiCastBlock = _multiCastBlock;
		//		countret++;
		//	}

		//	auto vtkind = RTVTable::GenerateReadKind(builder, vtptr);
		//	auto kindSwitch = builder->CreateSwitch(vtkind, errorBlock,7);
		//	kindSwitch->addCase(MakeInt<RTDescriptorKind>(RTDescriptorKind::Class), _classObjectBlock);
		//	kindSwitch->addCase(MakeInt<RTDescriptorKind>(RTDescriptorKind::OptimizedLambda), _lambda_targsInObject_Block);
		//	kindSwitch->addCase(MakeInt<RTDescriptorKind>(RTDescriptorKind::Lambda), _lambda_targsInVTable_Block);
		//	kindSwitch->addCase(MakeInt<RTDescriptorKind>(RTDescriptorKind::OptimizedStruct), _struct_targsInObject_Block);
		//	kindSwitch->addCase(MakeInt<RTDescriptorKind>(RTDescriptorKind::Struct), _struct_targsInVTable_Block);
		//	kindSwitch->addCase(MakeInt<RTDescriptorKind>(RTDescriptorKind::PartialApplication), _partialAppBlock);
		//	kindSwitch->addCase(MakeInt<RTDescriptorKind>(RTDescriptorKind::MultiCast), _multiCastBlock);

		//	return countret;
		//}

		llvm::FunctionType* GetMethodEnsureFunctionType()
		{
			return FunctionType::get(inttype(1), {REFTYPE, numtype(size_t)}, false);
		}

		//llvm::Value* RTVTable::GenerateReadFirstInterfaceTableEntryPointer(NomBuilder& builder, llvm::Value* vtablePtr)
		//{
		//	auto offset = MakeLoad(builder, builder->CreateGEP(builder->CreatePointerCast(vtablePtr, GetLLVMType()), )
		//	return MakeLoad(builder, builder builder->CreatePointerCast(vtablePtr, GetLLVMType()), MakeInt32(RTVTableFields::InterfaceTableFirstEntry), "FirstIfaceTableEntry");
		//}
	}
}