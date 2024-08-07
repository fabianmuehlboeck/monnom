#include "NomStaticMethod.h"
#include "llvm/IR/BasicBlock.h"
#include "TypeList.h"
#include "NomConstants.h"
#include "Context.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/raw_os_ostream.h"
#include <iostream>
#include "NomType.h"
#include "StringClass.h"
#include "NomSubstitutionContext.h"
#include "NomClass.h"
#include "CallingConvConf.h"
#include "NomBuilder.h"

using namespace Nom::Runtime;
using namespace llvm;

namespace Nom
{
	namespace Runtime
	{
		NomStaticMethodLoaded::NomStaticMethodLoaded(const std::string& name, const NomClass* parent, const std::string& qname, const ConstantID returnType, const ConstantID typeArgs, const ConstantID arguments, const int regcount, bool declOnly) : NomCallableLoaded(name, parent, qname, regcount, typeArgs, arguments, declOnly, false), returnType(returnType), returnTypeBuf(nullptr), Class(parent)
		{

		}
		llvm::Function * NomStaticMethodLoaded::createLLVMElement(llvm::Module &mod, llvm::GlobalValue::LinkageTypes linkage) const
		{
			if (declOnly)
			{
				linkage = llvm::GlobalValue::LinkageTypes::ExternalLinkage;
			}
			/*
			Creates an LLVM function and links it to an exisiting definition.
			Symbol name is the name of the function (create foo function) (based on c++ code, can link against and call this foo)
			Mod is the LLVM, module -> Drop everything into. (Code is meant so that you can make multiple modules)
			Environment is the book-keeping data to compile things. 

			Calling convention -> the C calling convention. 
			*/
			auto fun = llvm::Function::Create(GetLLVMFunctionType(), linkage, *GetSymbolName(), &mod);
			fun->setCallingConv(NOMCC);
			if (declOnly)
			{
				return fun;
			}
			StaticMethodCompileEnv smenv = StaticMethodCompileEnv(regcount, *GetSymbolName(), fun, &(this->phiNodes), this->GetAllTypeParameters(), this->GetArgumentTypes(nullptr), this);
			CompileEnv* env = &smenv;

			NomBuilder builder;
			BasicBlock* startBlock = BasicBlock::Create(LLVMCONTEXT, *GetSymbolName() + "$start", fun);

			InitializePhis(builder, fun, env);

			builder->SetInsertPoint(startBlock);

			auto instructions = GetInstructions();
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
			if (verifyFunction(*fun,&out))
			{
				out.flush();
				std::cout << "Could not verify static method!";
				fun->print(out);
				out.flush();
				std::cout.flush();
				throw name;
			}
			return fun;
		}

		NomTypeRef NomStaticMethodLoaded::GetReturnType(const NomSubstitutionContext* context) const
		{
			if (context != nullptr && context->GetTypeArgumentCount() > 0)
			{
				return NomConstants::GetType(context, returnType);
			}
			else
			{
				if (returnTypeBuf==nullptr)
				{
					NomSubstitutionContextMemberContext nscmc = NomSubstitutionContextMemberContext(this);
					returnTypeBuf = NomConstants::GetType(&nscmc, returnType);
				}
				return returnTypeBuf;
			}
		}

		llvm::ArrayRef<NomTypeParameterRef> NomStaticMethodLoaded::GetArgumentTypeParameters() const
		{
			return GetAllTypeParameters();
		}

		const NomClass* NomStaticMethodLoaded::GetContainer() const
		{
			return Class;
		}

		llvm::FunctionType* NomStaticMethod::GetLLVMFunctionType(const NomSubstitutionContext* context) const
		{
			std::vector<llvm::Type* > args(GetTypeParametersCount() + GetArgumentCount());
			unsigned int j;
			for (j = 0; j < GetTypeParametersCount(); j++)
			{
				args[j] = TYPETYPE;
			}
			unsigned int i;
			auto argtypes = (GetArgumentTypes(context));
			for (i = 0; i < GetArgumentCount(); i++)
			{
				args[j + i] = argtypes[i]->GetLLVMType();
			}
			return llvm::FunctionType::get(GetReturnType(context)->GetLLVMType(), args, false);
		}

		NomStaticMethodInternal::NomStaticMethodInternal(const std::string& name, const std::string& qname, const NomClass* cls) : NomCallableInternal(name, qname, cls), Container(cls)
		{
		}
		NomStaticMethodInternal::~NomStaticMethodInternal()
		{
		}
		llvm::Function* NomStaticMethodInternal::createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const
		{
			NomBuilder builder;
			auto funtype = GetLLVMFunctionType();
			auto wfun = Function::Create(funtype, linkage, *GetSymbolName(), &mod);
			wfun->setCallingConv(NOMCC);
			BasicBlock* forwardblock = BasicBlock::Create(LLVMCONTEXT, "start", wfun);
			builder->SetInsertPoint(forwardblock);
			auto ufun = Function::Create(funtype, linkage, GetQName(), &mod);
			ufun->setCallingConv(llvm::CallingConv::C);

			llvm::Value** argbuf = makealloca(llvm::Value*, funtype->getNumParams());
			auto fnumparams = funtype->getNumParams();
			for (decltype(fnumparams) i = 0; i < fnumparams; i++)
			{
				argbuf[i] = wfun->arg_begin() + i;
			}

			auto wcall = builder->CreateCall(ufun, llvm::ArrayRef<llvm::Value*>(argbuf, funtype->getNumParams()));
			wcall->setCallingConv(llvm::CallingConv::C);
			builder->CreateRet(wcall);

			return wfun;
		}

		llvm::ArrayRef<NomTypeParameterRef> NomStaticMethodInternal::GetArgumentTypeParameters() const
		{
			return GetAllTypeParameters();
		}
		const NomClass* NomStaticMethodInternal::GetContainer() const
		{
			return Container;
		}

		void NomStaticMethodInternal::SetReturnType(NomTypeRef returnType)
		{
			if (returnType == nullptr || this->returnType != nullptr)
			{
				throw new std::exception();
			}
			this->returnType = returnType;
		}

		NomTypeRef NomStaticMethodInternal::GetReturnType(const NomSubstitutionContext* context) const
		{
			if (context != nullptr && context->GetTypeArgumentCount() && returnType->ContainsVariables())
			{
				return returnType->SubstituteSubtyping(context);
			}
			return returnType;
		}
	}
}
