 #include "CallConstructor.h"
#include "../NomAlloc.h"
#include "../NomConstructor.h"
#include "../NomInstantiationRef.h"
#include "../NomClass.h"
#include "../NomVMInterface.h"
#include "../NomClassType.h"
#include "../CompileHelpers.h"
#include <iostream>
#include "llvm/Support/raw_os_ostream.h"
#include "../CallingConvConf.h"
#include "llvm/IR/Verifier.h"
#include "../RTCompileConfig.h"
#include "../RefValueHeader.h"

using namespace llvm;
using namespace std;
namespace Nom
{
	namespace Runtime
	{

		CallConstructor::CallConstructor(const RegIndex reg, ConstantID cls, ConstantID typeArgs) : NomValueInstruction(reg, OpCode::CallConstructor), ClassSuperClass(cls), TypeArgs(typeArgs)
		{
			
		}


		CallConstructor::~CallConstructor()
		{
		}

		llvm::Function* GenerateCConstructor(llvm::Module& mod, const NomConstructor *cnstrctr)
		{
			auto cls = cnstrctr->GetClass();

			FunctionType* ft = cnstrctr->GetLLVMFunctionType();
			auto targcount = cnstrctr->GetTypeParametersCount();
			auto argcount = cnstrctr->GetArgumentCount();
			int i;
			llvm::Type** argtarr = makealloca(llvm::Type*, targcount + argcount);
			for (i = 1; i <= targcount; i++)
			{
				argtarr[i-1] = ft->getParamType(i);
			}
			for (; i <= targcount+argcount; i++)
			{
				argtarr[i-1] = ft->getParamType(i);
			}
			FunctionType* nft = FunctionType::get(ft->getReturnType(), ArrayRef<llvm::Type*>(argtarr, targcount + argcount), false);

			Function* cfun = Function::Create(nft, llvm::GlobalValue::ExternalLinkage, std::string("RT_NOM_CCC_").append(*cnstrctr->GetSymbolName()), mod);

			BasicBlock* cblock = BasicBlock::Create(LLVMCONTEXT, "start", cfun);
			NomBuilder builder;

			builder->SetInsertPoint(cblock);

			Function* allocfun = GetNewAlloc(&mod);
			auto fieldCount = cls->GetFieldCount();
			llvm::Value* newmem = builder->CreateCall(allocfun, { MakeInt<size_t>(fieldCount+((cls->GetHasRawInvoke() && NomLambdaOptimizationLevel > 0)?1:0)), MakeInt<size_t>(cls->GetTypeArgOffset() + cls->GetTypeParametersCount()) });

			newmem = ObjectHeader::GenerateSetClassDescriptor(builder, newmem, fieldCount, cls->GetLLVMElement(mod));
			if (cls->GetHasRawInvoke() && NomLambdaOptimizationLevel > 0)
			{
				RefValueHeader::GenerateWriteRawInvoke(builder, newmem, cls->GetRawInvokeFunction(mod));
			}

			auto argiter = cfun->arg_begin();
			llvm::Value** argarr = makealloca(llvm::Value*, targcount + argcount + 1);
			argarr[0] = newmem;
			for (i = 1; i <= targcount; i++)
			{
				argarr[i] = argiter;
				argiter++;
			}
			for (; i <= targcount + argcount; i++)
			{
				argarr[i] = argiter;
				argiter++;
			}
			auto callinst = builder->CreateCall(cnstrctr->GetLLVMElement(mod), ArrayRef(argarr, targcount + argcount + 1));
			callinst->setCallingConv(NOMCC);
			cfun->setCallingConv(llvm::CallingConv::C);
			builder->CreateRet(callinst);

			llvm::raw_os_ostream out(std::cout);
			if (verifyFunction(*cfun, &out))
			{
				std::cout << "Could not verify C-accessible constructor!";
				cfun->print(out);
				out.flush();
				std::cout.flush();
				throw cnstrctr->GetSymbolName();
			}
			return cfun;
		}

		void CallConstructor::Compile(NomBuilder& builder, CompileEnv* env, int lineno)
		{
			NomSubstitutionContextMemberContext nscmc(env->Context);
			NomInstantiationRef<NomClass> cls = NomConstants::GetSuperClass(ClassSuperClass)->GetClassType(&nscmc);

			CompileActual(cls, builder, env, lineno);
		
		}

		void CallConstructor::CompileActual(NomInstantiationRef<NomClass> cls, NomBuilder& builder, CompileEnv* env, int lineno)
		{

			auto fieldCount = cls.Elem->GetFieldCount();

			Function* allocfun = GetNewAlloc(env->Module);
			llvm::Value* newmem = builder->CreateCall(allocfun, { MakeInt<size_t>(fieldCount + ((cls.Elem->GetHasRawInvoke() && NomLambdaOptimizationLevel > 0) ? 1 : 0)), MakeInt<size_t>(cls.Elem->GetTypeArgOffset() + cls.Elem->GetTypeParametersCount()) });

			newmem = ObjectHeader::GenerateSetClassDescriptor(builder, newmem, fieldCount, cls.Elem->GetLLVMElement(*(env->Module)));
			if (cls.Elem->GetHasRawInvoke() && NomLambdaOptimizationLevel > 0)
			{
				RefValueHeader::GenerateWriteRawInvoke(builder, newmem, cls.Elem->GetRawInvokeFunction(*(env->Module)));
			}

			NomValue* argbuf = (NomValue*)(nmalloc(sizeof(NomValue) * env->GetArgCount()));
			for (int i = 0; i < env->GetArgCount(); i++)
			{
				argbuf[i] = env->GetArgument(i);
			}
			RegisterValue(env, cls.Elem->GenerateConstructorCall(builder, env, cls.TypeArgs, newmem, llvm::ArrayRef<NomValue>(argbuf, env->GetArgCount())));
			env->ClearArguments();

		}


		void CallConstructor::Print(bool resolve)
		{
			cout << "CCall #";
			NomConstants::PrintConstant(ClassSuperClass, resolve);
			cout << " -> #" << std::dec << WriteRegister;
			cout << "\n";
		}
		void CallConstructor::FillConstantDependencies(NOM_CONSTANT_DEPENCENCY_CONTAINER& result)
		{
			result.push_back(ClassSuperClass);
			result.push_back(TypeArgs);
		}

	}
}