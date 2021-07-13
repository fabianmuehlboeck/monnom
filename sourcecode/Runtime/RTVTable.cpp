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
					numtype(RTDescriptorKind),									//Kind
					POINTERTYPE													//Nom IR descriptor
				);
			}
			return rtitt;
		}
		llvm::Constant* RTVTable::CreateConstant(RTDescriptorKind kind, const void* irlink, llvm::Constant* interfaceMethodTable)
		{
			return ConstantStruct::get(GetLLVMType(), ConstantArray::get(arrtype(TYPETYPE, 0), {}), interfaceMethodTable, MakeInt<RTDescriptorKind>(kind), GetLLVMPointer(irlink));
		}
		void RTVTable::GenerateGetInstanceType(NomBuilder& builder, llvm::Value* instanceTypeRef, llvm::Value* valPtr, llvm::Value* vtablePtr, llvm::BasicBlock* multiCastBlock)
		{
			llvm::Function* fun = builder->GetInsertBlock()->getParent();
			llvm::Module* mod = fun->getParent();
			BasicBlock* regularObjectBlock = BasicBlock::Create(LLVMCONTEXT, "regularObject", fun);
			BasicBlock* dynamicLambdaBlock = BasicBlock::Create(LLVMCONTEXT, "dynamicLambda", fun);
			BasicBlock* dynamicStructBlock = BasicBlock::Create(LLVMCONTEXT, "dynamicStruct", fun);
			BasicBlock* optimizedLambdaBlock = BasicBlock::Create(LLVMCONTEXT, "optimizedLambda", fun);
			BasicBlock* optimizedStructBlock = BasicBlock::Create(LLVMCONTEXT, "optimizedStruct", fun);
			BasicBlock* notImplementedBlock = BasicBlock::Create(LLVMCONTEXT, "notImplemented", fun);
			BasicBlock* mergeBlock = BasicBlock::Create(LLVMCONTEXT, "typeArgumentsPointerMerge", fun);

			auto kind = GenerateReadKind(builder, vtablePtr);
			auto kindswitch = builder->CreateSwitch(kind, notImplementedBlock, 5+(multiCastBlock==nullptr?0:1));
			kindswitch->addCase(MakeIntLike(kind, (char)RTDescriptorKind::Class), regularObjectBlock);
			kindswitch->addCase(MakeIntLike(kind, (char)RTDescriptorKind::Lambda), dynamicLambdaBlock);
			kindswitch->addCase(MakeIntLike(kind, (char)RTDescriptorKind::OptimizedLambda), optimizedLambdaBlock);
			kindswitch->addCase(MakeIntLike(kind, (char)RTDescriptorKind::Struct), dynamicStructBlock);
			kindswitch->addCase(MakeIntLike(kind, (char)RTDescriptorKind::OptimizedStruct), optimizedStructBlock);
			//TODO: partial app, expando
			if (multiCastBlock != nullptr)
			{
				kindswitch->addCase(MakeIntLike(kind, (char)RTDescriptorKind::MultiCast), multiCastBlock);
			}

			builder->SetInsertPoint(regularObjectBlock);
			auto objectTargPtr = ObjectHeader::GeneratePointerToTypeArguments(builder, valPtr);
			RTInstanceType::CreateInitialization(builder, *mod, instanceTypeRef, MakeInt<size_t>(0), ConstantPointerNull::get(POINTERTYPE), builder->CreatePointerCast(vtablePtr, RTInterface::GetLLVMType()->getPointerTo()), objectTargPtr);
			builder->CreateBr(mergeBlock);

			builder->SetInsertPoint(dynamicLambdaBlock);
			auto dynamicLambdaTargPtr = RTLambdaInterface::GetTypeArgumentsPointer(builder, vtablePtr);
			auto dynamicLambdaOrigInterface = RTLambdaInterface::GenerateReadOrigInterface(builder, vtablePtr);
			RTInstanceType::CreateInitialization(builder, *mod, instanceTypeRef, MakeInt<size_t>(0), ConstantPointerNull::get(POINTERTYPE), builder->CreatePointerCast(dynamicLambdaOrigInterface, RTInterface::GetLLVMType()->getPointerTo()), dynamicLambdaTargPtr);
			builder->CreateBr(mergeBlock);

			builder->SetInsertPoint(optimizedLambdaBlock);
			auto optimizedLambdaTargPtr = LambdaHeader::GeneratePointerToCastTypeArguments(builder, valPtr);
			auto optimizedLambdaOrigInterface = RTLambdaInterface::GenerateReadOrigInterface(builder, vtablePtr);
			RTInstanceType::CreateInitialization(builder, *mod, instanceTypeRef, MakeInt<size_t>(0), ConstantPointerNull::get(POINTERTYPE), builder->CreatePointerCast(optimizedLambdaOrigInterface, RTInterface::GetLLVMType()->getPointerTo()), optimizedLambdaTargPtr);
			builder->CreateBr(mergeBlock);

			builder->SetInsertPoint(dynamicStructBlock);
			auto dynamicStructTargPtr = RTStructInterface::GetTypeArgumentsPointer(builder, valPtr);
			auto dynamicStructOrigInterface = RTStructInterface::GetOriginalInterface(builder, vtablePtr);
			RTInstanceType::CreateInitialization(builder, *mod, instanceTypeRef, MakeInt<size_t>(0), ConstantPointerNull::get(POINTERTYPE), builder->CreatePointerCast(dynamicStructOrigInterface, RTInterface::GetLLVMType()->getPointerTo()), dynamicStructTargPtr);
			builder->CreateBr(mergeBlock);

			builder->SetInsertPoint(optimizedStructBlock);
			auto optimizedStructTargPtr = StructHeader::GeneratePointerToCastTypeArguments(builder, valPtr);
			auto optimizedStructOrigInterface = RTStructInterface::GetOriginalInterface(builder, vtablePtr);
			RTInstanceType::CreateInitialization(builder, *mod, instanceTypeRef, MakeInt<size_t>(0), ConstantPointerNull::get(POINTERTYPE), builder->CreatePointerCast(optimizedStructOrigInterface, RTInterface::GetLLVMType()->getPointerTo()), optimizedStructTargPtr);
			builder->CreateBr(mergeBlock);

			builder->SetInsertPoint(notImplementedBlock);
			static const char* generic_errorMessage = "Type instantiation not implemented for given instance type!";
			builder->CreateCall(RTOutput_Fail::GetLLVMElement(*fun->getParent()), GetLLVMPointer(generic_errorMessage))->setCallingConv(RTOutput_Fail::GetLLVMElement(*fun->getParent())->getCallingConv());
			CreateDummyReturn(builder, fun);

			builder->SetInsertPoint(mergeBlock);
		}
		llvm::Value* RTVTable::GenerateReadKind(NomBuilder& builder, llvm::Value* vtablePtr)
		{
			return MakeLoad(builder, builder->CreatePointerCast(vtablePtr, GetLLVMType()->getPointerTo()), MakeInt32(RTVTableFields::Kind), "Kind");
		}
		llvm::Instruction* RTVTable::GenerateWriteKind(NomBuilder& builder, llvm::Value* vtablePtr, llvm::Value* kind)
		{
			return MakeStore(builder, kind, builder->CreatePointerCast(vtablePtr, GetLLVMType()->getPointerTo()), MakeInt32(RTVTableFields::Kind));
		}
		llvm::Value* RTVTable::GenerateReadNomIRLink(NomBuilder& builder, llvm::Value* vtablePtr)
		{
			return MakeLoad(builder, builder->CreatePointerCast(vtablePtr, GetLLVMType()->getPointerTo()), MakeInt32(RTVTableFields::NomIRLink), "NomIR");
		}
		llvm::Value* RTVTable::GenerateReadInterfaceMethodTableEntry(NomBuilder& builder, llvm::Value* vtablePtr, llvm::Constant* index)
		{
			return MakeLoad(builder, builder->CreateGEP(builder->CreatePointerCast(vtablePtr, GetLLVMType()->getPointerTo()), { MakeInt32(0), MakeInt32(RTVTableFields::InterfaceMethodTable), index }));
		}

		llvm::Value* RTVTable::GenerateReadMethodTableEntry(NomBuilder& builder, llvm::Value* vtablePtr, llvm::Value* offset)
		{
			return MakeLoad(builder, builder->CreateGEP(builder->CreatePointerCast(vtablePtr, GetLLVMType()->getPointerTo()), { MakeInt32(0), MakeInt32(RTVTableFields::MethodTable), offset }), "methodPointer");
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
			MakeStore(builder, imtptr, selfptr, MakeInt32(RTVTableFields::InterfaceMethodTable));
			MakeStore(builder, kind, selfptr, MakeInt32(RTVTableFields::Kind));
			MakeStore(builder, irdesc, selfptr, MakeInt32(RTVTableFields::NomIRLink));
		}

		int RTVTable::GenerateVTableKindSwitch(NomBuilder& builder, llvm::Value* vtptr, llvm::BasicBlock** classObjectBlock, llvm::BasicBlock** lambdaBlock, llvm::BasicBlock** structBlock, llvm::BasicBlock** partialAppBlock, llvm::BasicBlock** multiCastBlock)
		{
			return GenerateVTableKindSwitch(builder, vtptr, classObjectBlock, lambdaBlock, lambdaBlock, structBlock, structBlock, partialAppBlock, multiCastBlock);
		}

		int RTVTable::GenerateVTableKindSwitch(NomBuilder& builder, llvm::Value* vtptr, llvm::BasicBlock** classObjectBlock, llvm::BasicBlock** lambda_targsInObject_Block, llvm::BasicBlock** lambda_targsInVTable_Block, llvm::BasicBlock** struct_targsInObject_Block, llvm::BasicBlock** struct_targsInVTable_Block, llvm::BasicBlock** partialAppBlock, llvm::BasicBlock** multiCastBlock)
		{
			BasicBlock* origBlock = builder->GetInsertBlock();
			Function* fun = origBlock->getParent();
			
			BasicBlock* errorBlock = RTOutput_Fail::GenerateFailOutputBlock(builder, "Unhandled vtable type");

			BasicBlock* _classObjectBlock = errorBlock, * _lambda_targsInObject_Block = errorBlock, * _lambda_targsInVTable_Block = errorBlock, * _struct_targsInObject_Block = errorBlock, * _struct_targsInVTable_Block = errorBlock, * _partialAppBlock = errorBlock, * _multiCastBlock = errorBlock;
			auto countret = 0;
			if (classObjectBlock != nullptr)
			{
				_classObjectBlock = BasicBlock::Create(LLVMCONTEXT, "classObject", fun);
				*classObjectBlock = _classObjectBlock;
				countret++;
			}
			if (lambda_targsInObject_Block != nullptr)
			{
				_lambda_targsInObject_Block = BasicBlock::Create(LLVMCONTEXT, "lambda_targsInObject", fun);
				*lambda_targsInObject_Block = _lambda_targsInObject_Block;
				countret++;
			}
			if (lambda_targsInVTable_Block != nullptr)
			{
				if (lambda_targsInVTable_Block == lambda_targsInObject_Block)
				{
					_lambda_targsInVTable_Block = _lambda_targsInObject_Block;
				}
				else
				{
					_lambda_targsInVTable_Block = BasicBlock::Create(LLVMCONTEXT, "lambda_targsInVTable", fun);
					*lambda_targsInVTable_Block = _lambda_targsInVTable_Block;
					countret++;
				}
			}
			if (struct_targsInObject_Block != nullptr)
			{
				_struct_targsInObject_Block = BasicBlock::Create(LLVMCONTEXT, "struct_targsInObject", fun);
				*struct_targsInObject_Block = _struct_targsInObject_Block;
				countret++;
			}
			if (struct_targsInVTable_Block != nullptr)
			{
				if (struct_targsInVTable_Block == struct_targsInObject_Block)
				{
					_struct_targsInVTable_Block = _struct_targsInObject_Block;
				}
				else
				{
					_struct_targsInVTable_Block = BasicBlock::Create(LLVMCONTEXT, "struct_targsInVTable", fun);
					*struct_targsInVTable_Block = _struct_targsInVTable_Block;
					countret++;
				}
			}
			if (partialAppBlock != nullptr)
			{
				_partialAppBlock = BasicBlock::Create(LLVMCONTEXT, "partialApp", fun);
				*partialAppBlock = _partialAppBlock;
				countret++;
			}
			if (multiCastBlock != nullptr)
			{
				_multiCastBlock = BasicBlock::Create(LLVMCONTEXT, "multiCast", fun);
				*multiCastBlock = _multiCastBlock;
				countret++;
			}

			auto vtkind = RTVTable::GenerateReadKind(builder, vtptr);
			auto kindSwitch = builder->CreateSwitch(vtkind, errorBlock,7);
			kindSwitch->addCase(MakeInt<RTDescriptorKind>(RTDescriptorKind::Class), _classObjectBlock);
			kindSwitch->addCase(MakeInt<RTDescriptorKind>(RTDescriptorKind::OptimizedLambda), _lambda_targsInObject_Block);
			kindSwitch->addCase(MakeInt<RTDescriptorKind>(RTDescriptorKind::Lambda), _lambda_targsInVTable_Block);
			kindSwitch->addCase(MakeInt<RTDescriptorKind>(RTDescriptorKind::OptimizedStruct), _struct_targsInObject_Block);
			kindSwitch->addCase(MakeInt<RTDescriptorKind>(RTDescriptorKind::Struct), _struct_targsInVTable_Block);
			kindSwitch->addCase(MakeInt<RTDescriptorKind>(RTDescriptorKind::PartialApplication), _partialAppBlock);
			kindSwitch->addCase(MakeInt<RTDescriptorKind>(RTDescriptorKind::MultiCast), _multiCastBlock);

			return countret;
		}

		//llvm::Value* RTVTable::GenerateReadFirstInterfaceTableEntryPointer(NomBuilder& builder, llvm::Value* vtablePtr)
		//{
		//	auto offset = MakeLoad(builder, builder->CreateGEP(builder->CreatePointerCast(vtablePtr, GetLLVMType()), )
		//	return MakeLoad(builder, builder builder->CreatePointerCast(vtablePtr, GetLLVMType()), MakeInt32(RTVTableFields::InterfaceTableFirstEntry), "FirstIfaceTableEntry");
		//}
	}
}