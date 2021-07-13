#include "Context.h"
#include "llvm/IR/IRBuilder.h"
#include "Defs.h"
#include "RTCast.h"
#include "RTFilterInstantiations.h"
#include "RTClass.h"
#include "llvm/IR/Type.h"
#include "CompileHelpers.h"
#include "CallingConvConf.h"
#include "RTSubtyping.h"
#include "RTInterface.h"

using namespace llvm;
namespace Nom
{
	namespace Runtime
	{
		RTFilterInstantiations::RTFilterInstantiations()
		{
		}
		RTFilterInstantiations& RTFilterInstantiations::Instance()
		{
			static RTFilterInstantiations rtfi; return rtfi;
		}
		RTFilterInstantiations::~RTFilterInstantiations()
		{
		}
		llvm::FunctionType* RTFilterInstantiations::GetFunctionType()
		{
			return llvm::FunctionType::get(numtype(size_t), {RTInterface::GetLLVMType()->getPointerTo(), TYPETYPE, RTSubtyping::TypeArgumentListStackType()->getPointerTo(), TYPETYPE->getPointerTo(), numtype(size_t)->getPointerTo(), numtype(size_t)}, false);
		}
		llvm::Function* RTFilterInstantiations::createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const
		{
			llvm::Function* fun = Function::Create(GetFunctionType(), linkage, "RT_NOM_FilterInstantiations", &mod);
			fun->setCallingConv(NOMCC);

			BasicBlock* startBlock = BasicBlock::Create(LLVMCONTEXT, "", fun);
			NomBuilder builder;
			builder->SetInsertPoint(startBlock);
			
			builder->CreateRet(MakeInt<size_t>(0)); //TODO: implement FilterInstantiations

			return fun;
		}
		llvm::Function* RTFilterInstantiations::findLLVMElement(llvm::Module& mod) const
		{
			return mod.getFunction("RT_NOM_FilterInstantiations");
		}
	}
}