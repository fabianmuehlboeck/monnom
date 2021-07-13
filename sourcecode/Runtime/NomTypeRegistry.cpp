#include "NomTypeRegistry.h"
#include "NomClassType.h"
#include "NomJIT.h"
#include "RTCast.h"
#include "CompileHelpers.h"
#include "RTOutput.h"
#include "CallingConvConf.h"
#include "RTSubtyping.h"
#include "RTInterface.h"
#include "RTInstanceType.h"

using namespace Nom::Runtime;
using namespace llvm;
using namespace std;
namespace Nom
{
	namespace Runtime
	{

		TypeRegistry::TypeRegistry()
		{

		}


		TypeRegistry& TypeRegistry::Instance()
		{
			static TypeRegistry tr; return tr;
		}

		TypeRegistry::~TypeRegistry()
		{

		}

		llvm::FunctionType* TypeRegistry::GetLLVMFunctionType()
		{
			static FunctionType* ft = FunctionType::get(TYPETYPE, {TYPETYPE, RTSubtyping::TypeArgumentListStackType()->getPointerTo() }, false);
			return ft;
		}

		llvm::Function* TypeRegistry::createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const
		{
			FunctionType* funtype = GetLLVMFunctionType();
			Function* fun = Function::Create(funtype, linkage, "RT_NOM_EnsureUniquedType", &mod);
			fun->setCallingConv(NOMCC);

			NomBuilder builder;

			BasicBlock* startBlock = BasicBlock::Create(LLVMCONTEXT, "", fun);
			BasicBlock* switchBlock = BasicBlock::Create(LLVMCONTEXT, "typeSwitch", fun);

			BasicBlock* idBlock = BasicBlock::Create(LLVMCONTEXT, "identity", fun);
			BasicBlock* classBlock = BasicBlock::Create(LLVMCONTEXT, "class", fun);
			BasicBlock* instanceBlock = BasicBlock::Create(LLVMCONTEXT, "instance", fun);
			BasicBlock* varBlock = BasicBlock::Create(LLVMCONTEXT, "typeVar", fun);

			BasicBlock* errorBlock = BasicBlock::Create(LLVMCONTEXT, "ERROR", fun);

			Argument* arg = fun->arg_begin();
			Value* type = arg;
			arg++;
			Value* substStack = arg;

			builder->SetInsertPoint(startBlock);
			builder->CreateBr(switchBlock);

			builder->SetInsertPoint(switchBlock);
			auto typePHI = builder->CreatePHI(TYPETYPE, 2, "type");
			auto stackPHI = builder->CreatePHI(RTSubtyping::TypeArgumentListStackType()->getPointerTo(), 2, "substStack");
			typePHI->addIncoming(type, startBlock);
			stackPHI->addIncoming(substStack, startBlock);
			auto typeKind = RTTypeHead::GenerateReadTypeKind(builder, typePHI);
			auto typeSwitch = builder->CreateSwitch(typeKind, errorBlock, 8);
			typeSwitch->addCase(MakeInt<TypeKind>(TypeKind::TKBottom), idBlock);
			typeSwitch->addCase(MakeInt<TypeKind>(TypeKind::TKDynamic), idBlock);
			typeSwitch->addCase(MakeInt<TypeKind>(TypeKind::TKStruct), idBlock);
			typeSwitch->addCase(MakeInt<TypeKind>(TypeKind::TKPartialApp), idBlock);
			typeSwitch->addCase(MakeInt<TypeKind>(TypeKind::TKTop), idBlock);

			typeSwitch->addCase(MakeInt<TypeKind>(TypeKind::TKVariable), varBlock);
			typeSwitch->addCase(MakeInt<TypeKind>(TypeKind::TKClass), classBlock);
			typeSwitch->addCase(MakeInt<TypeKind>(TypeKind::TKInstance), instanceBlock);

			builder->SetInsertPoint(idBlock);
			builder->CreateRet(typePHI);
			
			{
				builder->SetInsertPoint(varBlock);
				auto targsArr = MakeLoad(builder, mod, builder->CreateGEP(stackPHI, { MakeInt32(0), MakeInt32(TypeArgumentListStackFields::Types) }));
				auto varIndex = RTTypeVar::GenerateLoadIndex(builder, typePHI);
				typePHI->addIncoming(MakeLoad(builder, mod, builder->CreateGEP(targsArr, builder->CreateSub(MakeInt32(-1), varIndex))), varBlock);
				stackPHI->addIncoming(MakeLoad(builder, mod, builder->CreateGEP(stackPHI, { MakeInt32(0), MakeInt32(TypeArgumentListStackFields::Next) })), varBlock);
				builder->CreateBr(switchBlock);
			}

			{
				BasicBlock* uniqueTypeArgsBlock = BasicBlock::Create(LLVMCONTEXT, "uniqueTypeArgs", fun);
				BasicBlock* uniqueTypeArgsLoopBlock = BasicBlock::Create(LLVMCONTEXT, "uniqueTypeArgsLoop", fun);
				BasicBlock* uniqueTypeArgsDoneBlock = BasicBlock::Create(LLVMCONTEXT, "uniqueTypeArgsDone", fun);
				BasicBlock* newInstanceBlock = BasicBlock::Create(LLVMCONTEXT, "needNewInstance", fun);
				builder->SetInsertPoint(classBlock);
				auto iface = RTClassType::GenerateReadClassDescriptorLink(builder, typePHI);
				auto targcount = RTInterface::GenerateReadTypeArgCount(builder, iface);
				auto noTargs = builder->CreateICmpEQ(targcount, MakeIntLike(targcount, 0));
				builder->CreateCondBr(noTargs, idBlock, uniqueTypeArgsBlock);

				builder->SetInsertPoint(uniqueTypeArgsBlock);
				auto newTypeArgsArr = builder->CreateGEP(builder->CreateAlloca(TYPETYPE, targcount), targcount);
				auto hashArr = builder->CreateGEP(builder->CreateAlloca(numtype(size_t), targcount), targcount);
				auto typeArgsArr = RTClassType::GetTypeArgumentsPtr(builder, typePHI);
				auto negTypeArgCount = builder->CreateNeg(targcount);
				builder->CreateBr(uniqueTypeArgsLoopBlock);

				builder->SetInsertPoint(uniqueTypeArgsLoopBlock);
				auto typeArgIndexPHI = builder->CreatePHI(inttype(32), 2);
				typeArgIndexPHI->addIncoming(negTypeArgCount, uniqueTypeArgsBlock);
				auto foundDifference = builder->CreatePHI(inttype(1), 2);
				foundDifference->addIncoming(MakeUInt(1, 0), uniqueTypeArgsBlock);

				auto currentTypeArg = MakeLoad(builder, builder->CreateGEP(typeArgsArr, typeArgIndexPHI));
				auto uniquedTypeArg = builder->CreateCall(fun, { currentTypeArg, stackPHI });
				uniquedTypeArg->setCallingConv(fun->getCallingConv());

				MakeStore(builder, uniquedTypeArg, builder->CreateGEP(newTypeArgsArr, typeArgIndexPHI));
				auto uniquedHash = RTTypeHead::GenerateReadTypeHash(builder, uniquedTypeArg);
				MakeStore(builder, uniquedHash, builder->CreateGEP(hashArr, typeArgIndexPHI));

				foundDifference->addIncoming(builder->CreateOr(foundDifference, CreatePointerEq(builder, uniquedTypeArg, currentTypeArg)), builder->GetInsertBlock());
				typeArgIndexPHI->addIncoming(builder->CreateAdd(typeArgIndexPHI, MakeInt32(1)), builder->GetInsertBlock());
				auto typeArgsDone = builder->CreateICmpEQ(typeArgIndexPHI, MakeInt32(0));
				builder->CreateCondBr(typeArgsDone, uniqueTypeArgsDoneBlock, uniqueTypeArgsLoopBlock);

				builder->SetInsertPoint(uniqueTypeArgsDoneBlock);
				builder->CreateCondBr(foundDifference, newInstanceBlock, idBlock);

				builder->SetInsertPoint(newInstanceBlock);
				static auto uniqueInstantiationFun = NomInterface::GetGetUniqueInstantiationFunction(mod);
				auto uniquedResult = builder->CreateCall(uniqueInstantiationFun, {RTInterface::GenerateReadNomIRLink(builder, iface), iface, newTypeArgsArr, hashArr, targcount});
				uniquedResult->setCallingConv(uniqueInstantiationFun->getCallingConv());
				builder->CreateRet(uniquedResult);
			}

			{
				BasicBlock* uniqueTypeArgsBlock = BasicBlock::Create(LLVMCONTEXT, "uniqueTypeArgs", fun);
				BasicBlock* uniqueTypeArgsLoopBlock = BasicBlock::Create(LLVMCONTEXT, "uniqueTypeArgsLoop", fun);
				BasicBlock* newInstanceBlock = BasicBlock::Create(LLVMCONTEXT, "needNewInstance", fun);
				builder->SetInsertPoint(instanceBlock);
				auto iface = RTInstanceType::GenerateReadClassDescriptorLink(builder, typePHI);
				auto targcount = RTInterface::GenerateReadTypeArgCount(builder, iface);
				auto noTargs = builder->CreateICmpEQ(targcount, MakeIntLike(targcount, 0));
				builder->CreateCondBr(noTargs, idBlock, uniqueTypeArgsBlock);

				builder->SetInsertPoint(uniqueTypeArgsBlock);
				auto newTypeArgsArr = builder->CreateGEP(builder->CreateAlloca(TYPETYPE, targcount), targcount);
				auto hashArr = builder->CreateGEP(builder->CreateAlloca(numtype(size_t), targcount), targcount);
				auto typeArgsArr = RTInstanceType::GetTypeArgumentsPtr(builder, typePHI);
				auto negTypeArgCount = builder->CreateNeg(targcount);
				builder->CreateBr(uniqueTypeArgsLoopBlock);

				builder->SetInsertPoint(uniqueTypeArgsLoopBlock);
				auto typeArgIndexPHI = builder->CreatePHI(inttype(32), 2);
				typeArgIndexPHI->addIncoming(negTypeArgCount, uniqueTypeArgsBlock);

				auto currentTypeArg = MakeLoad(builder, builder->CreateGEP(typeArgsArr, typeArgIndexPHI));
				auto uniquedTypeArg = builder->CreateCall(fun, { currentTypeArg, stackPHI });
				uniquedTypeArg->setCallingConv(fun->getCallingConv());

				MakeStore(builder, uniquedTypeArg, builder->CreateGEP(newTypeArgsArr, typeArgIndexPHI));
				auto uniquedHash = RTTypeHead::GenerateReadTypeHash(builder, uniquedTypeArg);
				MakeStore(builder, uniquedHash, builder->CreateGEP(hashArr, typeArgIndexPHI));

				typeArgIndexPHI->addIncoming(builder->CreateAdd(typeArgIndexPHI, MakeInt32(1)), builder->GetInsertBlock());
				auto typeArgsDone = builder->CreateICmpEQ(typeArgIndexPHI, MakeInt32(0));
				builder->CreateCondBr(typeArgsDone, newInstanceBlock, uniqueTypeArgsLoopBlock);

				builder->SetInsertPoint(newInstanceBlock);
				static auto uniqueInstantiationFun = NomInterface::GetGetUniqueInstantiationFunction(mod);
				auto uniquedResult = builder->CreateCall(uniqueInstantiationFun, { RTInterface::GenerateReadNomIRLink(builder, iface), iface, newTypeArgsArr, hashArr, targcount });
				uniquedResult->setCallingConv(uniqueInstantiationFun->getCallingConv());
				builder->CreateRet(uniquedResult);
			}

			builder->SetInsertPoint(errorBlock);
			static const char * invalidKind_errorMessage = "Error: unexpected type kind";
			builder->CreateCall(RTOutput_Fail::GetLLVMElement(mod), GetLLVMPointer(invalidKind_errorMessage))->setCallingConv(RTOutput_Fail::GetLLVMElement(mod)->getCallingConv());
			CreateDummyReturn(builder, fun);

			return fun;
		}

		llvm::Function* TypeRegistry::findLLVMElement(llvm::Module& mod) const
		{
			return mod.getFunction("RT_NOM_EnsureUniquedType");
		}

	}
}
