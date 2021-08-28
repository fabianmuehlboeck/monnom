#include "RTOutput.h"
#include <iostream>
#include "NomNameRepository.h"
#include "CastStats.h"
#include "CompileHelpers.h"

using namespace llvm;
namespace Nom
{
	namespace Runtime
	{
		llvm::FunctionType* RTOutput_Fail::GetFunctionType()
		{
			static auto ft = FunctionType::get(REFTYPE, { POINTERTYPE }, false);
			return ft;
		}
		llvm::Function* RTOutput_Fail::GetLLVMElement(llvm::Module& mod)
		{
			Function* fun = mod.getFunction("NOM_RT_Fail");
			if (fun == nullptr)
			{
				fun = Function::Create(GetFunctionType(), GlobalValue::LinkageTypes::ExternalLinkage, "NOM_RT_Fail", &mod);
				fun->setDoesNotReturn();
			}
			return fun;
		}
		llvm::BasicBlock* RTOutput_Fail::GenerateFailOutputBlock(NomBuilder& builder, const char* errorMessage)
		{
			BasicBlock* origBlock = builder->GetInsertBlock();

			BasicBlock* errorBlock = BasicBlock::Create(LLVMCONTEXT, "errorBlock", origBlock->getParent());

			MakeBlockFailOutputBlock(builder, errorMessage, errorBlock);

			if (origBlock != nullptr)
			{
				builder->SetInsertPoint(origBlock);
			}

			return errorBlock;
		}
		void RTOutput_Fail::MakeBlockFailOutputBlock(NomBuilder& builder, const char* errorMessage, llvm::BasicBlock* block)
		{
			BasicBlock* origBlock = builder->GetInsertBlock();
			builder->SetInsertPoint(block);

			Function* errorFun = GetLLVMElement(*block->getParent()->getParent());
			builder->CreateCall(errorFun, GetLLVMPointer(errorMessage))->setCallingConv(errorFun->getCallingConv());
			builder->CreateUnreachable();

			if (origBlock != nullptr)
			{
				builder->SetInsertPoint(origBlock);
			}
		}
		llvm::FunctionType* RTOutput_Name::GetFunctionType()
		{
			static auto ft = FunctionType::get(Type::getVoidTy(LLVMCONTEXT), { POINTERTYPE, numtype(size_t) }, false);
			return ft;
		}
		llvm::Function* RTOutput_Name::GetLLVMElement(llvm::Module& mod)
		{
			Function* fun = mod.getFunction("NOM_RT_PrintReverseName");
			if (fun == nullptr)
			{
				fun = Function::Create(GetFunctionType(), GlobalValue::LinkageTypes::ExternalLinkage, "NOM_RT_PrintReverseName", &mod);
			}
			return fun;
		}
	}
}

extern "C" DLLEXPORT void* NOM_RT_Fail(const char* errstr)
{
	std::cout << "\n";
	std::cout << "\n";
	std::cout << "ERROR!\n";
	std::cout << errstr;
	std::cout << "Aborting...\n";
	std::cout << "\n";
	std::cout.flush();
	RT_debugout->flush();
	RT_NOM_STATS_Print(3);
	exit(1);
	return nullptr;
}

extern "C" DLLEXPORT void NOM_RT_PrintReverseName(const char* str, size_t id)
{
	std::cout << str;
	std::cout << *Nom::Runtime::NomNameRepository::Instance().GetNameFromID(id);
}