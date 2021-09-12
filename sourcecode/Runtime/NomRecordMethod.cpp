#include "NomRecordMethod.h"
#include "NomRecord.h"
#include "llvm/IR/Verifier.h"
#include <iostream>
#include "llvm/Support/raw_os_ostream.h"
#include "CompileEnv.h"
#include "CallingConvConf.h"
#include "NomBuilder.h"

using namespace llvm;
using namespace std;
namespace Nom
{
	namespace Runtime
	{
		NomRecordMethod::NomRecordMethod(const NomRecord* container, std::string& name, std::string& qname, ConstantID typeParameters, ConstantID returnType, ConstantID argTypes, RegIndex regcount) : NomCallableLoaded(name, container, qname, regcount, typeParameters, argTypes), Container(container), /*ArgumentTypes(argTypes),*/ ReturnType(returnType)
		{
		}
		llvm::Function* NomRecordMethod::createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const
		{
			NomBuilder builder;
			auto fun = Function::Create(GetLLVMFunctionType(), linkage, *GetSymbolName(), &mod);
			fun->setCallingConv(NOMCC);

			StructMethodCompileEnv menv = StructMethodCompileEnv(regcount, name, fun, &phiNodes, this->GetDirectTypeParameters(), this->GetArgumentTypes(nullptr), this);
			CompileEnv* env = &menv;

			BasicBlock* startBlock = BasicBlock::Create(LLVMCONTEXT, name + "start", fun);
			InitializePhis(builder, fun, env);

			builder->SetInsertPoint(startBlock);

			const std::vector<NomInstruction*>* instructions = GetInstructions();
#ifdef INSTRUCTIONMESSAGES
			auto dbgfun = GetDebugPrint(&mod);
#endif
			for (size_t i = 0; i < instructions->size(); i++)
			{
				(*instructions)[i]->Compile(builder, env, i);
#ifdef INSTRUCTIONMESSAGES
				if (!env->basicBlockTerminated)
				{
					builder->CreateCall(dbgfun, { { GetLLVMPointer(this->GetSymbolName()->data()), MakeInt<int64_t>(i)} });
				}
#endif
			}
			llvm::raw_os_ostream out(std::cout);
			//For some reason, verifyFunction is supposed to return false if there are no problems
			if (verifyFunction(*fun, &out))
			{
				std::cout << "Could not verify structure method!";
				fun->print(out);
				out.flush();
				std::cout.flush();
				throw name;
			}
			return fun;
		}
		NomTypeRef NomRecordMethod::GetReturnType(const NomSubstitutionContext* context) const
		{
			NomSubstitutionContextMemberContext nscmc(this);
			return NomConstants::GetType(&nscmc, ReturnType);
		}
		//TypeList NomRecordMethod::GetArgumentTypes(const NomSubstitutionContext* context) const
		//{
		//	return NomConstants::GetTypeList(ArgumentTypes)->GetTypeList(this);
		//}
		//int NomRecordMethod::GetArgumentCount() const
		//{
		//	return GetArgumentTypes().size();
		//}
		llvm::FunctionType* NomRecordMethod::GetLLVMFunctionType(const NomSubstitutionContext* context) const
		{
			auto args_ = GetArgumentTypes(context);
			auto targs_ = GetDirectTypeParameters();

			Type** args = makealloca(Type*, (targs_.size() + args_.size() + 1));
			unsigned int j;
			for (j = 0; j < targs_.size(); j++)
			{
				args[j] = TYPETYPE;
			}
			args[j] = REFTYPE;
			unsigned int i;
			for (i = 0; i < args_.size(); i++)
			{
				args[i + j + 1] = (args_)[i]->GetLLVMType();
			}
			return FunctionType::get(GetReturnType(context)->GetLLVMType(), ArrayRef<Type*>(args, targs_.size() + args_.size() + 1), false);
		}
		llvm::ArrayRef<NomTypeParameterRef> NomRecordMethod::GetArgumentTypeParameters() const
		{
			return GetDirectTypeParameters();
		}
	}
}