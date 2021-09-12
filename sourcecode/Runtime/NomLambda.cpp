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
#include "IMT.h"
#include "NomTopType.h"
#include "RTCompileConfig.h"
#include "RTOutput.h"
#include "CompileHelpers.h"
#include "NomLambdaCallTag.h"
#include "CallingConvConf.h"
#include "Metadata.h"
#include "RTCast.h"

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
			auto gvartype = StructType::get(LLVMCONTEXT, { inttype(64), RTLambda::GetLLVMType(), GetDynamicDispatchListEntryType() }, false);
			GlobalVariable* gv = new GlobalVariable(mod, gvartype, true, linkage, nullptr, "RT_NOM_LAMBDADESC_" + to_string(this->ID));
			gv->setAlignment(Align(256));
			NomBuilder builder;

			Function* invalidFieldReadFun = mod.getFunction("MONNOM_RT_LAMBDA_READ_FIELD");
			if (invalidFieldReadFun == nullptr)
			{
				invalidFieldReadFun = Function::Create(GetFieldReadFunctionType(), linkage, "MONNOM_RT_LAMBDA_READ_FIELD", mod);
				auto block = BasicBlock::Create(LLVMCONTEXT, "start", invalidFieldReadFun);
				RTOutput_Fail::MakeBlockFailOutputBlock(builder, "Tried to access field of lambda value!", block);
			}

			Function* invalidFieldWriteFun = mod.getFunction("MONNOM_RT_LAMBDA_WRITE_FIELD");
			if (invalidFieldWriteFun == nullptr)
			{
				invalidFieldWriteFun = Function::Create(GetFieldWriteFunctionType(), linkage, "MONNOM_RT_LAMBDA_WRITE_FIELD", mod);
				auto block = BasicBlock::Create(LLVMCONTEXT, "start", invalidFieldWriteFun);
				RTOutput_Fail::MakeBlockFailOutputBlock(builder, "Tried to write field of lambda value!", block);
			}

			Function* invalidIMTInvocation = mod.getFunction("MONNOM_RT_LAMBDA_IMT_ENTRY");
			if (invalidIMTInvocation == nullptr)
			{
				invalidIMTInvocation = Function::Create(GetIMTFunctionType(), linkage, "MONNOM_RT_LAMBDA_IMT_ENTRY", mod);
				auto block = BasicBlock::Create(LLVMCONTEXT, "start", invalidIMTInvocation);
				RTOutput_Fail::MakeBlockFailOutputBlock(builder, "Tried to call named method on lambda value!", block);
			}

			Function* fun = Function::Create(GetLLVMFunctionType(), linkage, name, &mod);
			fun->setCallingConv(NOMCC);

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

			auto imtarr = makealloca(Constant*, IMTsize);
			auto ddtarr = makealloca(Constant*, IMTsize);



			for (decltype(IMTsize) i = 0; i < IMTsize; i++)
			{
				imtarr[i] = invalidIMTInvocation;
				ddtarr[i] = ConstantExpr::getGetElementPtr(gvartype, gv, ArrayRef<Constant*>({ MakeInt32(0), MakeInt32(2) }));
			}

			auto emptyDDTE = GetDynamicDispatchListEntryConstant(MakeInt<size_t>(0), MakeInt<size_t>(0), ConstantPointerNull::get(GetIMTFunctionType()->getPointerTo()));

			auto constant = RTLambda::CreateConstant(this, ConstantArray::get(arrtype(GetIMTFunctionType()->getPointerTo(), IMTsize), ArrayRef<Constant*>(imtarr, IMTsize)), ConstantArray::get(arrtype(GetDynamicDispatchListEntryType()->getPointerTo(), IMTsize), ArrayRef<Constant*>(ddtarr, IMTsize)), invalidFieldReadFun, invalidFieldWriteFun);
			gv->setInitializer(ConstantStruct::get(gvartype, { MakeUInt(64,0), constant, emptyDDTE }));

			LambdaHeader::GenerateConstructorCode(builder, ArrayRef<Value*>(typeArgBuf, targc), ArrayRef<Value*>(argBuf, argc), ConstantExpr::getGetElementPtr(gvartype, gv, ArrayRef<Constant*>({ MakeInt32(0), MakeInt32(1) })), this);

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
			}
		}

		// Inherited via NomCallable

		Function* NomLambdaBody::createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const
		{
			Function* fun = Function::Create(GetIMTFunctionType(), linkage, *GetSymbolName(), &mod);
			fun->setCallingConv(NOMCC);

			NomBuilder builder;

			BasicBlock* startBlock = BasicBlock::Create(LLVMCONTEXT, "checkCallTag", fun);
			BasicBlock* matchBlock = BasicBlock::Create(LLVMCONTEXT, "callTagMatch", fun);
			BasicBlock* mismatchBlock = BasicBlock::Create(LLVMCONTEXT, "callTagMismatch", fun);

			auto funparamcount = GetIMTFunctionType()->getNumParams();

			auto argsarr = makealloca(Value*, funparamcount);

			auto args = fun->arg_begin();
			Value* callTag = args;
			for (decltype(funparamcount) i = 0; i < funparamcount; i++, args++)
			{
				argsarr[i] = args;
			}

			builder->SetInsertPoint(startBlock);
			auto targetCallTag = NomLambdaCallTag::GetCallTag(GetDirectTypeParametersCount(), GetArgumentCount())->GetLLVMElement(mod);
			auto callTagMatch = builder->CreateICmpEQ(builder->CreatePtrToInt(callTag, targetCallTag->getType()), targetCallTag);
			builder->CreateIntrinsic(Intrinsic::expect, { inttype(1) }, { callTagMatch, MakeUInt(1, 1) });
			builder->CreateCondBr(callTagMatch, matchBlock, mismatchBlock, GetLikelyFirstBranchMetadata());

			LambdaCompileEnv lenv = LambdaCompileEnv(regcount, fun->getName(), fun, &phiNodes, GetDirectTypeParameters(), GetArgumentTypes(nullptr), this);

			CompileEnv* env = &lenv;

			InitializePhis(builder, fun, env);

			builder->SetInsertPoint(matchBlock);
			NomSubstitutionContextMemberContext nscmc(this);
			auto argpos = 1;
			for (auto& arg : this->GetArgumentTypes(&nscmc))
			{
				auto val = (*env)[argpos];
				RTCast::GenerateCast(builder, env, val, arg);
				argpos++;
			}

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

			builder->SetInsertPoint(mismatchBlock);
			argsarr[0] = builder->CreatePointerCast(fun,POINTERTYPE);
			auto tailCall = builder->CreateCall(GetIMTFunctionType(), builder->CreatePointerCast(callTag, GetIMTFunctionType()->getPointerTo()), ArrayRef<Value*>(argsarr, funparamcount));
			tailCall->setCallingConv(NOMCC);
			tailCall->setTailCallKind(CallInst::TailCallKind::TCK_MustTail);
			builder->CreateRet(tailCall);

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
}
}