#include "LoadStringConstantInstruction.h"
#include "../StringClass.h"
#include "../NomClassType.h"

using namespace std;
namespace Nom
{
	namespace Runtime
	{
		void LoadStringConstantInstruction::Compile(NomBuilder &builder, CompileEnv* env, int lineno)
		{
			env->basicBlockTerminated = false;
			RegisterValue(env, NomValue(NomConstants::GetString(this->constantID)->getObject(*(env->Module)), NomStringClass::GetInstance()->GetType()));
			//GenerateLLVMDebugPrint(builder, env->Module, "Loading String Constant\n");
			//std::array<llvm::Value *, 1> args = { { llvm::ConstantInt::get(LLVMCONSTANTINDEXTYPE, constantID, false) } };
			//llvm::Function * fun = GetCppLoadStringConstant(env->Module);
			////Debug
			////llvm::FunctionType * ftype = fun->getFunctionType();
			////for (int i = 0; i < ftype->getNumParams(); i++) {
			////	llvm::Type * ptype = ftype->getFunctionParamType(i);
			////	llvm::Type * atype = args[i]->getType();
			////	ptype->dump();
			////	atype->dump();
			////	llvm::LLVMContext &pcontext = ptype->getContext();
			////	llvm::LLVMContext &acontext = atype->getContext();
			////	llvm::LLVMContext &globalcontext = LLVMCONTEXT;
			////	std::cout << (ptype == atype);
			////}
			////for (auto iter = fun->arg_begin(); iter != fun->arg_end(); iter++) {
			////	iter->getType()->dump();
			////}
			//RegisterValue(env, builder->CreateCall(fun, args));
		}
		void LoadStringConstantInstruction::Print(bool resolve)
		{
			cout << "LoadString ";
			NomConstants::PrintConstant(constantID, resolve);
			cout << " -> #" << std::dec << WriteRegister;
			cout << "\n";
		}
		void LoadStringConstantInstruction::FillConstantDependencies(NOM_CONSTANT_DEPENCENCY_CONTAINER& result)
		{
			result.push_back(constantID);
		}
	}
}