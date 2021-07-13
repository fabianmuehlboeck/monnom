#include "NomLambda.h"
#include "Defs.h"
#include "RTLambda.h"
#include "llvm/IR/GlobalVariable.h"
#include "NomType.h"
#include "CompileEnv.h"
#include "LambdaHeader.h"
#include "NomDynamicType.h"
#include "NomLambda.h"
#include "llvm/IR/Verifier.h"
#include <iostream>
#include "llvm/Support/raw_os_ostream.h"
#include "NomConstants.h"
#include "RTSignature.h"
#include "NomPartialApplication.h"
#include "NomCallableVersion.h"
#include "NomTypeDecls.h"
#include "CallingConvConf.h"
#include "NomStructType.h"
#include "NomTopType.h"

using namespace llvm;
using namespace std;
namespace Nom
{
	namespace Runtime
	{
		vector<NomLambda*>& NomLambda::preprocessQueue()
		{
			static vector<NomLambda*> ppq; return ppq;
		}
		NomLambda::NomLambda(ConstantID id, const NomMemberContext* parent, const RegIndex regcount, ConstantID closureTypeParams, ConstantID closureArguments, ConstantID typeParams, ConstantID argTypes, ConstantID returnType) : NomCallableLoaded("RT_NOM_Lambda_" + to_string(id), parent, "RT_NOM_Lambda_" + to_string(id), 0, closureTypeParams, closureArguments, false, false), ID(id), Body(this, regcount, typeParams, argTypes, returnType)
		{
			NomConstants::GetLambda(id)->SetLambda(this);
			preprocessQueue().push_back(this);
		}
		void NomLambda::ProcessPreprocessQueue()
		{
			for (auto lam : preprocessQueue())
			{
				lam->PreprocessInheritance();
			}
			preprocessQueue().clear();
		}
		llvm::Function* NomLambda::createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const
		{
			std::string name = *GetSymbolName(); //"RT_NOM_Lambda_" + to_string(ID);
			Function* fun = Function::Create(GetLLVMFunctionType(), linkage, name, &mod);
			fun->setCallingConv(NOMCC);

			NomBuilder builder;
			BasicBlock* startBlock = BasicBlock::Create(LLVMCONTEXT, "", fun);
			builder->SetInsertPoint(startBlock);

			auto targc = GetDirectTypeParametersCount();
			auto argc = GetArgumentCount();

			Value** typeArgBuf = makealloca(Value*, targc);
			Value** argBuf = makealloca(Value*, argc);

			auto carg = fun->arg_begin();
			for (decltype(targc) i = 0; i < targc; i++, carg++)
			{
				typeArgBuf[i] = carg;
			}
			for (decltype(argc) i = 0; i < argc; i++, carg++)
			{
				argBuf[i] = carg;
			}

			auto checkedFun = Body.GetLLVMElement(mod);
			Function* dispatcherFun; 

			bool specialDispatcherNeeded = false;
			for (auto argT : Body.GetArgumentTypes(nullptr))
			{
				if (!NomType::Anything->IsSubtype(argT))
				{
					specialDispatcherNeeded = true;
				}
			}
			if (specialDispatcherNeeded)
			{
				/*dispatcherFun = Function::Create(NomPartialApplication::GetDynamicDispatcherType(Body.GetDirectTypeParametersCount(), Body.GetArgumentCount()), linkage, "RT_NOM_LAMBDA_DD_" + name, &mod);
				NomBuilder builder;

				BasicBlock* entryBlock = BasicBlock::Create(LLVMCONTEXT, "", dispatcherFun);
				builder->SetInsertPoint(entryBlock);*/
				dispatcherFun = NomPartialApplication::GetDispatcherEntry(mod, linkage, Body.GetDirectTypeParametersCount(), Body.GetArgumentCount(), &Body, this, &NomStructType::Instance());
			}
			else
			{
				dispatcherFun = Body.GetVersion(NomPartialApplication::GetDynamicDispatcherType(Body.GetDirectTypeParametersCount(), Body.GetArgumentCount()))->GetLLVMElement(mod);
			}

			auto constant = RTLambda::CreateConstant(this, Body.GetDirectTypeParametersCount(), GetArgumentCount(), RTSignature::CreateGlobalConstant(mod, linkage, "RT_NOM_SIG_" + name, &this->Body), checkedFun, dispatcherFun);
			GlobalVariable* gv = new GlobalVariable(mod, constant->getType(), false, linkage, constant, "RT_NOM_LAMBDADESC_" + to_string(this->ID));

			LambdaHeader::GenerateConstructorCode(builder, ArrayRef<Value*>(typeArgBuf, targc), ArrayRef<Value*>(argBuf, argc), gv, this);

			return fun;
		}
		llvm::Function* NomLambda::findLLVMElement(llvm::Module& mod) const
		{
			std::string name = "RT_NOM_Lambda_" + to_string(ID);
			return mod.getFunction(name);
		}
		const NomField* NomLambda::GetField(NomStringRef name) const
		{
			for (auto field : Fields)
			{
				if (NomStringEquality()(field->GetName(), name))
				{
					return field;
				}
			}
			throw new std::exception();
		}
		NomTypeRef NomLambda::GetReturnType(const NomSubstitutionContext* context) const
		{
			return &NomDynamicType::Instance();
		}
		//TypeList NomLambda::GetArgumentTypes(const NomSubstitutionContext* context) const
		//{
		//	return argTypes;
		//}
		//int NomLambda::GetArgumentCount() const
		//{
		//	return argTypes.size();
		//}
		const std::string* NomLambda::GetSymbolName() const
		{
			if (symname.empty())
			{
				symname = "RT_NOM_Lambda_" + to_string(ID);
			}
			return &symname;
		}
		const std::string* NomLambdaBody::GetSymbolName() const
		{
			if (symname.empty())
			{
				symname = "RT_NOM_LambdaBody_" + to_string(Parent->ID);
			}
			return &symname;
		}
		llvm::FunctionType* NomLambda::GetLLVMFunctionType(const NomSubstitutionContext* context) const
		{
			auto args_ = GetArgumentTypes(context);

			auto targs_size = GetTypeParametersCount();
			auto args_size = args_.size();

			Type** args = makealloca(Type*, (targs_size + args_size));
			decltype(targs_size) j;
			for (j = 0; j < targs_size; j++)
			{
				args[j] = TYPETYPE;
			}
			//args[j] = REFTYPE;
			decltype(args_size) i;
			for (i = 0; i < args_size; i++)
			{
				args[i + j] = (args_)[i]->GetLLVMType();
			}
			return FunctionType::get(GetReturnType(context)->GetLLVMType(), ArrayRef<Type*>(args, targs_size + args_size), false);
		}
		llvm::ArrayRef<NomTypeParameterRef> NomLambda::GetArgumentTypeParameters() const
		{
			return GetAllTypeParameters();
		}
		llvm::FunctionType* NomLambdaBody::GetLLVMFunctionType(const NomSubstitutionContext* context) const
		{
			auto args_ = GetArgumentTypes(context);

			auto targs_size = GetDirectTypeParametersCount();
			auto args_size = args_.size();

			Type** args = makealloca(Type*, (targs_size + args_size + 1));
			decltype(targs_size) j;
			for (j = 0; j < targs_size; j++)
			{
				args[j] = TYPETYPE;
			}
			args[j] = REFTYPE;
			decltype(args_size) i;
			for (i = 0; i < args_size; i++)
			{
				args[i + j + 1] = (args_)[i]->GetLLVMType();
			}
			return FunctionType::get(GetReturnType(context)->GetLLVMType(), ArrayRef<Type*>(args, targs_size + args_size + 1), false);
		}
		const llvm::ArrayRef<NomTypeParameterRef> NomLambdaBody::GetDirectTypeParameters() const
		{
			return llvm::ArrayRef<NomTypeParameterRef>((NomTypeParameterRef*)this, (size_t)0);
		}
		size_t NomLambdaBody::GetDirectTypeParametersCount() const
		{
			return 0;
		}
		NomTypeParameterRef NomLambdaBody::GetLocalTypeParameter(int index) const
		{
			throw new std::exception();
		}
		const NomMemberContext* NomLambdaBody::GetParent() const
		{
			return Parent;
		}
		const std::string& NomLambdaBody::GetName() const
		{
			return *GetSymbolName();
		}
		llvm::ArrayRef<NomTypeParameterRef> NomLambdaBody::GetArgumentTypeParameters() const
		{
			return llvm::ArrayRef<NomTypeParameterRef>();
		}
		NomClosureField* NomLambda::AddField(const ConstantID name, const ConstantID type) {
			NomClosureField* field = new NomClosureField(this, name, type, Fields.size());
			Fields.push_back(field);
			return field;
		}
		NomLambdaBody::NomLambdaBody(NomLambda* parent, const RegIndex regcount, ConstantID typeParams, ConstantID argTypes, ConstantID returnType) :NomCallableLoaded("", parent, "", regcount, typeParams, argTypes, false, false), Parent(parent),/* ArgTypes(argTypes),*/ ReturnType(returnType)
		{
		}

		void NomLambda::PreprocessInheritance() const
		{
			if (!preprocessed)
			{
				preprocessed = true;
				//for (auto field : Fields)
				//{
				//	argTypes.push_back(field->GetType());
				//}
			}
		}

		// Inherited via NomCallable

		Function* NomLambdaBody::createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const
		{
			Function* fun = Function::Create(GetLLVMFunctionType(), linkage, *GetSymbolName(), &mod);
			fun->setCallingConv(NOMCC);

			NomBuilder builder;

			BasicBlock* startBlock = BasicBlock::Create(LLVMCONTEXT, "", fun);

			LambdaCompileEnv lenv = LambdaCompileEnv(regcount, fun->getName(), fun, &phiNodes, GetDirectTypeParameters(), GetArgumentTypes(nullptr), this);

			CompileEnv* env = &lenv;

			InitializePhis(builder, fun, env);

			builder->SetInsertPoint(startBlock);

			const std::vector<NomInstruction*>* instructions = GetInstructions();
#ifdef INSTRUCTIONMESSAGES
			auto dbgfun = GetDebugPrint(&mod);
#endif
			auto instrcount = instructions->size();
			for (decltype(instrcount) i = 0; i < instrcount; i++)
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
		NomTypeRef NomLambdaBody::GetReturnType(const NomSubstitutionContext* context) const
		{
			return NomConstants::GetType(context, ReturnType);
		}
		//TypeList NomLambdaBody::GetArgumentTypes(const NomSubstitutionContext* context) const
		//{
		//	//return NomConstants::GetTypeList(ArgTypes)->GetTypeList(this);
		//	if (context != nullptr && context->GetTypeArgumentCount() > 0)
		//	{
		//		return NomConstants::GetTypeList(ArgTypes)->GetTypeList(context);
		//	}
		//	else
		//	{
		//		if (argumentTypes.data() == nullptr)
		//		{
		//			NomSubstitutionContextMemberContext nscmc = NomSubstitutionContextMemberContext(this);
		//			argumentTypes = NomConstants::GetTypeList(ArgTypes)->GetTypeList(&nscmc);
		//		}
		//		return argumentTypes;
		//	}
		//}
		//int NomLambdaBody::GetArgumentCount() const
		//{
		//	return GetArgumentTypes().size();
		//}
	}
}