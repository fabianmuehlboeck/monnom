#include "NomMethod.h"
#include <iostream>
#include "llvm/IR/Verifier.h"
#include "IntClass.h"
#include "FloatClass.h"
#include "CompileEnv.h"
#include "NomInterface.h"
#include "NomType.h"
#include "NomTypeVar.h"
#include "llvm/Support/raw_os_ostream.h"
#include <iostream>
#include "StringClass.h"
#include "NomSubstitutionContext.h"
#include "CallingConvConf.h"
#include "RTConfig.h"
#include "NomBuilder.h"

using namespace llvm;

namespace Nom
{
	namespace Runtime
	{
		NomMethodLoaded::NomMethodLoaded(const NomInterface* container, const std::string& name, const std::string& qname, const ConstantID returnType, const ConstantID argTypes, RegIndex regcount, ConstantID typeParameters, bool isFinal) : NomCallableLoaded(name, container, qname, regcount, typeParameters, argTypes, false, false), returnType(returnType), /*NomMemberContextLoaded(container, typeParameters),*//* argTypes(argTypes),*/ isFinal(isFinal), Container(container)
		{

		}

		int NomMethod::GetIMTIndex() const
		{
			return (this->GetContainer()->GetName()->HashCode() + (GetOffset() * 4177)) % IMTsize;
		}

		bool NomMethod::IsOffsetSet()
		{
			return offsetSet;
		}
		bool NomMethod::SetOffset(int offset)
		{
			if (!offsetSet)
			{
				this->offset = offset;
				this->offsetSet = true;
				return true;
			}
			return false;
		}
		int NomMethod::GetOffset() const
		{
			return offset;
		}
		
		bool NomMethod::Overrides(const NomMethod* other) const
		{
			if (GetName() != other->GetName())
			{
				return false;
			}
			if (GetDirectTypeParametersCount() != other->GetDirectTypeParametersCount())
			{
				return false;
			}
			if (GetArgumentCount() != other->GetArgumentCount())
			{
				return false;
			}

			auto thisiface = this->GetContainer();
			auto otheriface = other->GetContainer();

			auto thisnscl = NomSubstitutionContextList(this->GetDirectTypeVariables());

			auto thiscmc = NomSubstitutionContextMemberContext(thisiface);

			auto thisinsts = thisiface->GetInstantiations();
			auto instargs = thisinsts[otheriface];

			auto othercnscl = NomSubstitutionContextList(instargs);

			auto thissc = NomSubstitutionContextCombination(&thiscmc, &thisnscl);
			auto othersc = NomSubstitutionContextCombination(&othercnscl, &thisnscl);

			if (!GetReturnType(&thissc)->IsSubtype(other->GetReturnType(&othersc), true))
			{
				return false;
			}
			auto myargs = GetArgumentTypes(&thissc);
			auto myarg = myargs.begin();
			auto oargs = other->GetArgumentTypes(&othersc);
			auto oarg = oargs.begin();
			while (myarg != myargs.end())
			{
				if (!((*oarg)->IsSubtype(*myarg, true)))
				{
					return false;
				}
				myarg++;
				oarg++;
			}
			return true;
		}
		NomMethodInternal::NomMethodInternal(const NomInterface* container, const std::string& name, const std::string& qname, bool isFinal) : NomCallableInternal(name, qname, container), isFinal(isFinal), Container(container)
		{
		}
		void NomMethodInternal::SetReturnType(NomTypeRef returnType)
		{
			if (returnType == nullptr || this->returnType != nullptr)
			{
				throw new std::exception();
			}
			this->returnType = returnType;
		}
		NomTypeRef NomMethodInternal::GetReturnType(const NomSubstitutionContext* context) const
		{
			if (context == nullptr || context->GetTypeArgumentCount()==0 || !returnType->ContainsVariables())
			{
				return returnType;
			}
			return returnType->SubstituteSubtyping(context);
		}
		llvm::Function* NomMethodInternal::createLLVMElement(Module& mod, llvm::GlobalValue::LinkageTypes linkage) const
		{
			NomBuilder builder;
			auto funtype = GetLLVMFunctionType();
			auto wfun = Function::Create(funtype, linkage, *GetSymbolName(), &mod);
			wfun->setCallingConv(NOMCC);
			BasicBlock* forwardblock = BasicBlock::Create(LLVMCONTEXT, GetName() + "start", wfun);
			builder->SetInsertPoint(forwardblock);
			auto ufun = Function::Create(funtype, linkage, GetQName(), &mod);
			ufun->setCallingConv(llvm::CallingConv::C);

			auto ftnumparams = funtype->getNumParams();
			llvm::Value** argbuf = makealloca(llvm::Value*, ftnumparams);
			for (decltype(ftnumparams) i = 0; i < ftnumparams; i++)
			{
				argbuf[i] = wfun->arg_begin() + i;
			}

			auto wcall = builder->CreateCall(ufun, llvm::ArrayRef<llvm::Value*>(argbuf, funtype->getNumParams()));
			wcall->setCallingConv(llvm::CallingConv::C);
			builder->CreateRet(wcall);
			return wfun;
		}
		llvm::ArrayRef<NomTypeParameterRef> NomMethodInternal::GetArgumentTypeParameters() const
		{
			return GetDirectTypeParameters();
		}
		const NomInterface* NomMethodInternal::GetContainer() const
		{
			return Container;
		}
		llvm::Function* NomMethodLoaded::createLLVMElement(Module& mod, llvm::GlobalValue::LinkageTypes linkage) const
		{
			NomBuilder builder;


			if (declOnly)
			{
				linkage = llvm::GlobalValue::LinkageTypes::ExternalLinkage;
			}
			auto fun = Function::Create(GetLLVMFunctionType(), linkage, *GetSymbolName(), &mod);
			fun->setCallingConv(NOMCC);
			if (declOnly)
			{
				return fun;
			}
			NomClassTypeRef selfType;
			if (Container->GetTypeParametersCount() > 0)
			{
				selfType = Container->GetType(Container->GetAllTypeVariables());
			}
			else
			{
				selfType = Container->GetType();
			}
			InstanceMethodCompileEnv menv = InstanceMethodCompileEnv(regcount, name, fun, &phiNodes, this->GetDirectTypeParameters(), this->GetArgumentTypes(nullptr), selfType, this);
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
				std::cout << "Could not verify method!";
				fun->print(out);
				out.flush();
				std::cout.flush();
				throw name;
			}
			return fun;
		}

		NomTypeRef NomMethodLoaded::GetReturnType(const NomSubstitutionContext* context) const
		{
			if (context != nullptr && context->GetTypeArgumentCount() > 0)
			{
				return NomConstants::GetType(context, returnType);
			}
			else
			{
				if (returnTypeBuf == nullptr)
				{
					NomSubstitutionContextMemberContext nscmc = NomSubstitutionContextMemberContext(this);
					returnTypeBuf = NomConstants::GetType(&nscmc, returnType);
				}
				return returnTypeBuf;
			}
		}

		llvm::ArrayRef<NomTypeParameterRef> NomMethodLoaded::GetArgumentTypeParameters() const
		{
			return GetDirectTypeParameters();
		}

		const NomInterface* NomMethodLoaded::GetContainer() const
		{
			return Container;
		}


		llvm::FunctionType* NomMethod::GetRawInvokeLLVMFunctionType(const NomSubstitutionContext* context) const
		{
			size_t argcount = GetDirectTypeParametersCount() + GetArgumentCount() + 1 + 1;
			llvm::Type** args = makealloca(llvm::Type *, (argcount));
			unsigned int j;
			args[0] = INTTYPE;
			args++;
			if (GetContainer() == NomIntClass::GetInstance())
			{
				args[0] = INTTYPE;
			}
			else if (GetContainer() == NomFloatClass::GetInstance())
			{
				args[0] = FLOATTYPE;
			}
			else
			{
				args[0] = REFTYPE;
			}
			for (j = 1; j <= GetDirectTypeParametersCount(); j++)
			{
				args[j] = TYPETYPE;
			}
			unsigned int i;
			auto argtypes = (GetArgumentTypes(context));
			for (i = 0; i < GetArgumentCount(); i++)
			{
				args[j + i] = argtypes[i]->GetLLVMType();
			}
			args--;
			return llvm::FunctionType::get(GetReturnType(context)->GetLLVMType(), ArrayRef<llvm::Type*>(args, argcount), false);
		}

		llvm::FunctionType* NomMethod::GetLLVMFunctionType(const NomSubstitutionContext *context) const
		{
			std::vector<llvm::Type* > args(GetDirectTypeParametersCount() + GetArgumentCount() + 1);
			unsigned int j;
			if (GetContainer() == NomIntClass::GetInstance())
			{
				args[0] = INTTYPE;
			}
			else if (GetContainer() == NomFloatClass::GetInstance())
			{
				args[0] = FLOATTYPE;
			}
			else
			{
				args[0] = REFTYPE;
			}
			for (j = 1; j <= GetDirectTypeParametersCount(); j++)
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

		NomMethodDeclLoaded::NomMethodDeclLoaded(const NomInterface* container, const std::string& name, const std::string& qname, const ConstantID returnType, const ConstantID argTypes, ConstantID typeParameters) : NomCallableLoaded(name, container, qname, 0, typeParameters, argTypes, true, false), returnTypeID(returnType)
		{
		}

		NomTypeRef NomMethodDeclLoaded::GetReturnType(const NomSubstitutionContext* context) const
		{
			if (context != nullptr && context->GetTypeArgumentCount() > 0)
			{
				return NomConstants::GetType(context, returnTypeID);
			}
			if (returnType == nullptr)
			{
				NomSubstitutionContextMemberContext nscmc(this);
				returnType = NomConstants::GetType(&nscmc, returnTypeID);
			}
			return returnType;
		}

		llvm::Function* NomMethodDeclLoaded::createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const
		{
			throw new std::exception(); //declaration should never actually be emitted
		}

		NomMethodDeclInternal::NomMethodDeclInternal(NomInterface* container, const std::string& name, const std::string& qname) : NomCallableInternal(name, qname, container), Container(container)
		{

		}

		void NomMethodDeclInternal::SetReturnType(NomTypeRef returnType)
		{
			if (returnType == nullptr || this->returnType != nullptr)
			{
				throw new std::exception();
			}
			this->returnType = returnType;
		}

		llvm::Function* NomMethodDeclInternal::createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const
		{
			throw new std::exception(); //declaration should never actually be emitted
		}

		NomTypeRef NomMethodDeclInternal::GetReturnType(const NomSubstitutionContext* context) const
		{
			if (context != nullptr && context->GetTypeArgumentCount() && returnType->ContainsVariables())
			{
				return returnType->SubstituteSubtyping(context);
			}
			return returnType;
		}

		llvm::ArrayRef<NomTypeParameterRef> NomMethodDeclInternal::GetArgumentTypeParameters() const
		{
			return GetDirectTypeParameters();
		}

		const NomInterface* NomMethodDeclInternal::GetContainer() const
		{
			return Container;
		}

	}
}