#include "NomConstructor.h"
PUSHDIAGSUPPRESSION
#include "llvm/ADT/ArrayRef.h"
#include "llvm/Support/raw_os_ostream.h"
#include "llvm/IR/Verifier.h"
POPDIAGSUPPRESSION
#include "NomClass.h"
#include "NomType.h"
#include "NomClassType.h"
#include "NomMemberContext.h"
#include "NomTypeVar.h"
#include "IntClass.h"
#include "FloatClass.h"
#include <iostream>
#include "instructions/PhiNode.h"
#include "StringClass.h"
#include "ObjectClass.h"
#include "CallingConvConf.h"
#include "instructions/CallConstructor.h"
#include "PWObject.h"

namespace Nom
{
	namespace Runtime
	{
		[[clang::no_destroy]] static std::string constructorPrefix = "RT_NOM_Constructor_";
		NomConstructor::NomConstructor() : GloballyNamed(&constructorPrefix)
		{

		}
		NomConstructorLoaded::NomConstructorLoaded(const NomClass * _cls, const std::string &_name, const std::string &_qname, const ConstantID _arguments, const RegIndex _regcount, const ConstantID _typeArgs, bool _declOnly, bool _cppWrapper) : NomConstructor(), NomCallableLoaded(_name, _cls, _qname, _regcount, _typeArgs, _arguments, _declOnly, _cppWrapper), returnTypeBuf(nullptr), Class(_cls)
		{
		}


		NomConstructorLoaded::~NomConstructorLoaded()
		{
		}
		llvm::Function * NomConstructorLoaded::createLLVMElement(llvm::Module & mod, llvm::GlobalValue::LinkageTypes linkage) const
		{
			NomBuilder builder;
			if (cppWrapper)
			{
				auto funtype = GetLLVMFunctionType();
				auto wfun = Function::Create(funtype, linkage, *GetSymbolName(), &mod);
				wfun->setCallingConv(NOMCC);
				BasicBlock* forwardblock = BasicBlock::Create(LLVMCONTEXT, name + "start", wfun);
				builder->SetInsertPoint(forwardblock);
				auto ufun = Function::Create(funtype, (declOnly ? llvm::GlobalValue::LinkageTypes::ExternalLinkage : linkage), qname, &mod);
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

			if (declOnly)
			{
				linkage = llvm::GlobalValue::LinkageTypes::ExternalLinkage;
			}
			auto fun = llvm::Function::Create(GetLLVMFunctionType(), linkage, *GetSymbolName(), &mod);
			fun->setCallingConv(NOMCC);
			if (declOnly)
			{
				return fun;
			}
			NomClassTypeRef selfType;
			if (Class->GetTypeParametersCount() > 0)
			{
				selfType = Class->GetType(Class->GetAllTypeVariables());
			}
			else
			{
				selfType = Class->GetType();
			}
			ConstructorCompileEnv cenv = ConstructorCompileEnv(builder, regcount, *GetSymbolName(), fun, &(this->phiNodes), this->GetAllTypeParameters(), this->GetArgumentTypes(nullptr), selfType, this);
			CompileEnv* env = &cenv;


			BasicBlock* startBlock = BasicBlock::Create(LLVMCONTEXT, *GetSymbolName() + "$start", fun);
			InitializePhis(builder, fun, env);


			builder->SetInsertPoint(startBlock);

			size_t offset = Class->GetTypeArgOffset();
			for (size_t pos = 0; pos < this->Class->GetTypeParametersCount(); pos++)
			{
				PWObject((*env)[0]).WriteTypeArgument(builder, PWCInt32(offset + pos, false), env->GetTypeArgument(builder, pos));
			}

			auto preinstructions = this->preInstructions;
			for (auto instruction : preinstructions)
			{
				instruction->Compile(builder, env, 0);
			}

			auto superclass = Class->GetSuperClass();
			if (superclass.HasElem()&&superclass.Elem!=NomObjectClass::GetInstance())
			{
				RTValuePtr *superArgsBuf = makenmalloc(RTValuePtr,superConstructorArgs.size());
				auto scasize = superConstructorArgs.size();
				for (decltype(scasize) i = 0; i < scasize; i++)
				{
					superArgsBuf[i] = (*env)[superConstructorArgs[i]];
				}
				superclass.Elem->GenerateConstructorCall(builder, env, superclass.TypeArgs, (*env)[0], llvm::ArrayRef<RTValuePtr>(superArgsBuf, superConstructorArgs.size()));
			}
			cenv.SetPastInitialSetup();
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
			builder->CreateRet((*env)[0]);
			//For some reason, verifyFunction is supposed to return false if there are no problems
			llvm::raw_os_ostream out(std::cout);
			if (verifyFunction(*fun,&out))
			{
				std::cout << "Could not verify constructor!";
				fun->print(out);
				out.flush();
				std::cout.flush();
				throw name;
			}
			return fun;
		}
		NomTypeRef NomConstructorLoaded::GetReturnType(const NomSubstitutionContext* context) const
		{
			if (context != nullptr && context->GetTypeArgumentCount() > 0)
			{
				return Class->GetType(context->GetTypeParameters());
			}
			else
			{
				if (returnTypeBuf == nullptr)
				{
					NomSubstitutionContextMemberContext nscmc = NomSubstitutionContextMemberContext(this);
					returnTypeBuf = Class->GetType(nscmc.GetTypeParameters());
				}
				return returnTypeBuf;
			}
		}

		llvm::ArrayRef<NomTypeParameterRef> NomConstructorLoaded::GetArgumentTypeParameters() const
		{
			return GetAllTypeParameters();
		}

		llvm::FunctionType* NomConstructor::GetLLVMFunctionType(const NomSubstitutionContext* context) const
		{
			std::vector<llvm::Type* > args(GetTypeParametersCount() + GetArgumentCount() + 1);
			unsigned int j = 0;
			if (GetClass() == NomIntClass::GetInstance())
			{
				args[j] = INTTYPE;
			}
			else if (GetClass() == NomFloatClass::GetInstance())
			{
				args[j] = FLOATTYPE;
			}
			else
			{
				args[j] = REFTYPE;
			}
			for (j = 1; j <= GetTypeParametersCount(); j++)
			{
				args[j] = TYPETYPE;
			}
			unsigned int i;
			auto argtypes = GetArgumentTypes(context);
			for (i = 0; i < argtypes.size(); i++)
			{
				args[j + i] = argtypes[i]->GetLLVMType();
			}
			return llvm::FunctionType::get(GetReturnType(context)->GetLLVMType(), args, false);
		}
		NomConstructorInternal::NomConstructorInternal(const std::string &qname, const NomClass* cls) : NomCallableInternal("CONSTRUCT$", qname, cls), Container(cls)
		{
		}
		NomConstructorInternal::~NomConstructorInternal()
		{
		}
		llvm::Function* NomConstructorInternal::createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const
		{
			NomBuilder builder;
			auto funtype = GetLLVMFunctionType();
			auto wfun = Function::Create(funtype, linkage, *GetSymbolName(), &mod);
			wfun->setCallingConv(NOMCC);
			BasicBlock* forwardblock = BasicBlock::Create(LLVMCONTEXT, Container->GetName()->ToStdString() + "$CONSTRUCT$start", wfun);
			builder->SetInsertPoint(forwardblock);
			auto ufun = Function::Create(funtype,linkage, GetQName(), &mod);
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

			if (linkage == llvm::GlobalValue::LinkageTypes::ExternalLinkage)
			{
				GenerateCConstructor(mod, this);
			}
			return wfun;
		}
		NomTypeRef NomConstructorInternal::GetReturnType(const NomSubstitutionContext* context) const
		{
			if (context == nullptr)
			{
				return Container->GetType(GetAllTypeVariables());
			}
			return Container->GetType(context->GetTypeParameters());
		}
		llvm::ArrayRef<NomTypeParameterRef> NomConstructorInternal::GetArgumentTypeParameters() const
		{
			return GetAllTypeParameters();
		}
		const NomClass* NomConstructorInternal::GetClass() const
		{
			return Container;
		}
	}
}
