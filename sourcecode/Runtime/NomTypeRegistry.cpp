#include "NomTypeRegistry.h"
#include "NomClassType.h"
#include "NomJIT.h"
#include "RTCast.h"
#include "CompileHelpers.h"
#include "RTOutput.h"
#include "CallingConvConf.h"
#include "RTSubtyping.h"
#include "RTInterface.h"
#include "XRTInstanceType.h"
#include "XRTTypeVar.h"
#include "RTMaybeType.h"
#include "CastStats.h"
#include "PWSubstStack.h"
#include "PWTypeVar.h"
#include "PWTypeArr.h"
#include "PWClassType.h"
#include "PWInterface.h"
#include "PWInstanceType.h"
#include "PWPhi.h"

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
			[[clang::no_destroy]] static TypeRegistry tr; return tr;
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
			auto typePHI = PWPhi<PWType>::Create(builder, 2, "type");
			auto stackPHI = PWPhi<PWSubstStack>::Create(builder, 2, "substStack");
			typePHI->addIncoming(type, startBlock);
			stackPHI->addIncoming(substStack, startBlock);
			auto typeKind = RTTypeHead::GenerateReadTypeKind(builder, typePHI);
			auto typeSwitch = builder->CreateSwitch(typeKind, errorBlock, 8);
			typeSwitch->addCase(MakeInt<TypeKind>(TypeKind::TKBottom), idBlock);
			typeSwitch->addCase(MakeInt<TypeKind>(TypeKind::TKDynamic), idBlock);
			typeSwitch->addCase(MakeInt<TypeKind>(TypeKind::TKRecord), idBlock);
			typeSwitch->addCase(MakeInt<TypeKind>(TypeKind::TKPartialApp), idBlock);
			typeSwitch->addCase(MakeInt<TypeKind>(TypeKind::TKTop), idBlock);

			typeSwitch->addCase(MakeInt<TypeKind>(TypeKind::TKVariable), varBlock);
			typeSwitch->addCase(MakeInt<TypeKind>(TypeKind::TKClass), classBlock);
			typeSwitch->addCase(MakeInt<TypeKind>(TypeKind::TKInstance), instanceBlock);

			builder->SetInsertPoint(idBlock);
			builder->CreateRet(typePHI);
			
			{
				builder->SetInsertPoint(varBlock);
				PWSubstStack nextStack = nullptr;
				auto nextType = stackPHI.Pop(builder, PWTypeVar(typePHI), &nextStack);
				auto cblock = builder->GetInsertBlock();
				typePHI->addIncoming(nextType, cblock);
				stackPHI->addIncoming(nextStack, cblock);
				builder->CreateBr(switchBlock);
			}

			{
				BasicBlock* uniqueTypeArgsBlock = BasicBlock::Create(LLVMCONTEXT, "uniqueTypeArgs", fun);
				BasicBlock* uniqueTypeArgsLoopBlock = BasicBlock::Create(LLVMCONTEXT, "uniqueTypeArgsLoop", fun);
				BasicBlock* uniqueTypeArgsDoneBlock = BasicBlock::Create(LLVMCONTEXT, "uniqueTypeArgsDone", fun);
				BasicBlock* newInstanceBlock = BasicBlock::Create(LLVMCONTEXT, "needNewInstance", fun);
				builder->SetInsertPoint(classBlock);
				PWClassType curType = typePHI.wrapped;
				auto iface = curType.ReadClassDescriptorLink(builder);
				auto targcount = iface.ReadTypeArgCount(builder).Resize<32>(builder);
				auto noTargs = builder->CreateICmpEQ(targcount, MakeIntLike(targcount, 0));
				builder->CreateCondBr(noTargs, idBlock, uniqueTypeArgsBlock);

				builder->SetInsertPoint(uniqueTypeArgsBlock);
				auto newTypeArgsArr = PWTypeArr::Alloca(builder, targcount);
				auto hashArr = PWInverseArr<PWInt64>::Alloca(builder, targcount);
				auto typeArgsArr = curType.TypeArgumentsPointer(builder).WithSize(targcount);

				builder->CreateBr(uniqueTypeArgsLoopBlock);

				builder->SetInsertPoint(uniqueTypeArgsLoopBlock);
				auto typeArgIndexPHI = builder->CreatePHI(inttype(32), 2);
				typeArgIndexPHI->addIncoming(MakeIntLike(targcount, 0), uniqueTypeArgsBlock);
				auto foundDifference = builder->CreatePHI(inttype(1), 2);
				foundDifference->addIncoming(MakeUInt(1, 0), uniqueTypeArgsBlock);

				auto currentTypeArg = typeArgsArr.LoadElemAt(builder, typeArgIndexPHI, "currenTypeArg");
				auto uniquedTypeArg = builder->CreateCall(fun, { currentTypeArg, stackPHI });
				uniquedTypeArg->setCallingConv(fun->getCallingConv());

				newTypeArgsArr.Get(builder, typeArgIndexPHI).Store(builder, uniquedTypeArg);
				auto uniquedHash = PWType(uniquedTypeArg).ReadHash(builder);
				hashArr.Get(builder, typeArgIndexPHI).Store(builder, uniquedHash);

				auto newFoundDifference = builder->CreateOr(foundDifference, builder->CreateNot(CreatePointerEq(builder, uniquedTypeArg, currentTypeArg)));
				foundDifference->addIncoming(newFoundDifference, builder->GetInsertBlock());
				auto newTypeArgIndexPHI = builder->CreateAdd(typeArgIndexPHI, MakeInt32(1));
				typeArgIndexPHI->addIncoming(newTypeArgIndexPHI, builder->GetInsertBlock());
				auto typeArgsDone = builder->CreateICmpEQ(newTypeArgIndexPHI, targcount);
				builder->CreateCondBr(typeArgsDone, uniqueTypeArgsDoneBlock, uniqueTypeArgsLoopBlock);

				builder->SetInsertPoint(uniqueTypeArgsDoneBlock);
				builder->CreateCondBr(newFoundDifference, newInstanceBlock, idBlock);

				builder->SetInsertPoint(newInstanceBlock);
				static auto uniqueInstantiationFun = NomInterface::GetGetUniqueInstantiationFunction(mod);
				if (NomCastStats)
				{
					builder->CreateCall(GetIncTypeInstanceAlloactionsFunction(*fun->getParent()), {});
				}
				auto uniquedResult = builder->CreateCall(uniqueInstantiationFun, {iface.ReadIRPtr(builder), iface, newTypeArgsArr, hashArr, targcount});
				uniquedResult->setCallingConv(uniqueInstantiationFun->getCallingConv());
				builder->CreateRet(uniquedResult);
			}

			{
				BasicBlock* uniqueTypeArgsBlock = BasicBlock::Create(LLVMCONTEXT, "uniqueTypeArgs", fun);
				BasicBlock* newInstanceBlock = BasicBlock::Create(LLVMCONTEXT, "needNewInstance", fun);
				builder->SetInsertPoint(instanceBlock);
				PWInstanceType curType = typePHI.wrapped;
				auto iface = curType.ReadClassDescriptorLink(builder);
				auto targcount = iface.ReadTypeArgCount(builder).Resize<32>(builder);
				auto noTargs = builder->CreateICmpEQ(targcount, MakeIntLike(targcount, 0));
				builder->CreateCondBr(noTargs, idBlock, uniqueTypeArgsBlock);

				builder->SetInsertPoint(uniqueTypeArgsBlock);
				auto newTypeArgsArr = PWTypeArr::Alloca(builder, targcount);
				auto hashArr = PWInverseArr<PWInt64>::Alloca(builder, targcount);
				auto typeArgsArr = curType.TypeArgumentsPtr(builder).WithSize(targcount);
				typeArgsArr.IterateFromStart(builder, newInstanceBlock, [fun, stackPHI, newTypeArgsArr, hashArr](NomBuilder& b, PWType curArg, PWInt32 curIndex) {
					auto uniquedTypeArg = b->CreateCall(fun, { curArg, stackPHI });
					uniquedTypeArg->setCallingConv(fun->getCallingConv());
					newTypeArgsArr.Get(b, curIndex).Store(b, uniquedTypeArg);
					auto uniquedHash = PWType(uniquedTypeArg).ReadHash(b);
					hashArr.Get(b, curIndex).Store(b, uniquedHash);
					}, true, "uniqueTypeArgsLoop");

				builder->SetInsertPoint(newInstanceBlock);
				if (NomCastStats)
				{
					builder->CreateCall(GetIncTypeInstanceAlloactionsFunction(*fun->getParent()), {});
				}
				static auto uniqueInstantiationFun = NomInterface::GetGetUniqueInstantiationFunction(mod);
				auto uniquedResult = builder->CreateCall(uniqueInstantiationFun, { iface.ReadIRPtr(builder), iface, newTypeArgsArr, hashArr, targcount});
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
