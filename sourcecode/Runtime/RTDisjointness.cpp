#include "RTDisjointness.h"
#include "Context.h"
#include "llvm/IR/IRBuilder.h"
#include "Defs.h"
#include "RTCast.h"
#include "CallingConvConf.h"
#include "RTSubtyping.h"

using namespace llvm;
namespace Nom
{
	namespace Runtime
	{
		RTDisjointness::RTDisjointness()
		{
		}
		RTDisjointness& RTDisjointness::Instance()
		{
			static RTDisjointness rtd; return rtd;
		}
		RTDisjointness::~RTDisjointness()
		{
		}
		llvm::FunctionType* RTDisjointness::GetDisjointnessFunctionType()
		{
			static llvm::FunctionType* ft = llvm::FunctionType::get(llvm::IntegerType::getInt1Ty(LLVMCONTEXT), { TYPETYPE, TYPETYPE, RTSubtyping::TypeArgumentListStackType()->getPointerTo(), RTSubtyping::TypeArgumentListStackType()->getPointerTo() }, false);
			return ft;
		}
		llvm::Function* RTDisjointness::createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const
		{
			llvm::Function* fun = Function::Create(GetDisjointnessFunctionType(), linkage, "NOM_RT_Disjointness", &mod);
			fun->setCallingConv(NOMCC);

			BasicBlock* bb = BasicBlock::Create(LLVMCONTEXT, "", fun);

			NomBuilder builder;
			
			builder->SetInsertPoint(bb);
			builder->CreateRet(llvm::ConstantInt::get(llvm::IntegerType::getInt1Ty(LLVMCONTEXT), 0, false)); //TODO: implement Disjointnesss
			return fun;
		}
		llvm::Function* RTDisjointness::findLLVMElement(llvm::Module& mod) const
		{
			return mod.getFunction("NOM_RT_Disjointness");
		}
	}
}