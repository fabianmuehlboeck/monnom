#include "NomCLib.h"
#include "llvm/Support/raw_os_ostream.h"
#include "llvm/IR/Verifier.h"
#include "CallingConvConf.h"
#include "NomCallable.h"
#include "NomTypeDecls.h"
#include "NomTypeParameter.h"
#include "instructions/ArgumentInstruction.h"
#include "instructions/CastInstruction.h"
#include "TypeOperations.h"


namespace Nom {
	namespace Runtime {
		
		llvm::Function* NomCLibStatic::createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const {
			/*
			Creates an LLVM function and links it to an exisiting definition.
			Symbol name is the name of the function (create foo function) (based on c++ code, can link against and call this foo)
			Mod is the LLVM, module -> Drop everything into. (Code is meant so that you can make multiple modules)
			Environment is the book-keeping data to compile things.

			Calling convention -> the C calling convention.
			*/

			linkage = llvm::GlobalValue::LinkageTypes::ExternalLinkage;
			auto fun = llvm::Function::Create(GetLLVMFunctionType(), linkage, *GetSymbolName(), &mod);
			fun->setCallingConv(llvm::CallingConv::C);

			NomBuilder builder;
			BasicBlock* startBlock = BasicBlock::Create(LLVMCONTEXT, *GetSymbolName() + "$start", fun);
			builder->SetInsertPoint(startBlock);

			CLibStaticCompileEnv smenv = CLibStaticCompileEnv(regcount, *GetSymbolName(), fun, &(this->phiNodes), typeArgs, argTypes, nullptr, builder);
			CompileEnv* env = &smenv;

			InitializePhis(builder, fun, env);
			
#ifdef INSTRUCTIONMESSAGES
			auto dbgfun = GetDebugPrint(&mod);
#endif	
			llvm::ArrayRef<NomTypeRef> functionTypes = ArrayRef<NomTypeRef>(argTypes);

			for (int i = 0; i < GetArgumentCount(); i++) {
				ArgumentInstruction argInst = ArgumentInstruction(i);
				argInst.Compile(builder, env, 0);
			}


			std::vector<NomTypeRef> typeArgValuesArray = {};
			for (int i = 0; i < env->GetLocalTypeArgumentCount(); i++) {
				typeArgValuesArray.push_back((NomTypeRef)(env->GetTypeArgument(builder, i).GetVariable()));
			}
			const TypeList typeArgValues = ArrayRef<NomTypeRef>(typeArgValuesArray);

			
			CallCheckedStaticMethod callInst = CallCheckedStaticMethod(NULL, NULL, 0);
			callInst.CompileActual(method, method.TypeArgs, builder, env, 0);
#ifdef INSTRUCTIONMESSAGES
			if (!env->basicBlockTerminated)
			{
				builder->CreateCall(dbgfun, { { GetLLVMPointer(this->GetSymbolName()->data()), MakeInt<int64_t>(i)} });
			}
#endif
			if (GetReturnType(nullptr)->GetLLVMType() == REFTYPE) {
				llvm::Type* i8Type = llvm::Type::getInt8Ty(LLVMCONTEXT);
				llvm::Type* i8Ptr = i8Type->getPointerTo();
				builder->CreateRet(builder->CreateBitCast(*((*env)[0]), i8Ptr));
			}
			else if (GetReturnType(nullptr)->GetLLVMType() == FLOATTYPE) {
				builder->CreateRet(EnsurePackedUnpacked(builder, *((*env)[0]), FLOATTYPE));
			}
			else {
				builder->CreateRet(EnsurePackedUnpacked(builder, *((*env)[0]), INTTYPE));
			}
			llvm::raw_os_ostream out(std::cout);
			//For some reason, verifyFunction is supposed to return false if there are no problems
			if (verifyFunction(*fun, &out))
			{
				out.flush();
				std::cout << "Could not verify static method!";
				fun->print(out);
				out.flush();
				std::cout.flush();
				//throw name;
			}
			return fun;
		}
		llvm::Function* NomCLibConstructor::createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const {
			/*
			Creates an LLVM function and links it to an exisiting definition.
			Symbol name is the name of the function (create foo function) (based on c++ code, can link against and call this foo)
			Mod is the LLVM, module -> Drop everything into. (Code is meant so that you can make multiple modules)
			Environment is the book-keeping data to compile things.

			Calling convention -> the C calling convention.
			*/

			linkage = llvm::GlobalValue::LinkageTypes::ExternalLinkage;
			auto fun = llvm::Function::Create(GetLLVMFunctionType(), linkage, *GetSymbolName(), &mod);
			fun->setCallingConv(llvm::CallingConv::C);

			NomBuilder builder;
			BasicBlock* startBlock = BasicBlock::Create(LLVMCONTEXT, *GetSymbolName() + "$start", fun);
			builder->SetInsertPoint(startBlock);

			CLibStaticCompileEnv smenv = CLibStaticCompileEnv(regcount, *GetSymbolName(), fun, &(this->phiNodes), typeArgs, argTypes, nullptr, builder);
			CompileEnv* env = &smenv;

			InitializePhis(builder, fun, env);

#ifdef INSTRUCTIONMESSAGES
			auto dbgfun = GetDebugPrint(&mod);
#endif	
			llvm::ArrayRef<NomTypeRef> functionTypes = ArrayRef<NomTypeRef>(argTypes);

			for (int i = 0; i < GetArgumentCount(); i++) {
				ArgumentInstruction argInst = ArgumentInstruction(i);
				argInst.Compile(builder, env, 0);
			}

			/*
			std::vector<NomTypeRef> typeArgValuesArray = {};
			for (int i = 0; i < env->GetLocalTypeArgumentCount(); i++) {
				typeArgValuesArray.push_back((NomTypeRef)(env->GetTypeArgument(builder, i).GetVariable()));
			}
			const TypeList typeArgValues = ArrayRef<NomTypeRef>(typeArgValuesArray);
			*/

			CallConstructor callInst = CallConstructor(NULL, NULL, 0);
			callInst.CompileDirectly(classRef, argTypes, builder, env, 0);
#ifdef INSTRUCTIONMESSAGES
			if (!env->basicBlockTerminated)
			{
				builder->CreateCall(dbgfun, { { GetLLVMPointer(this->GetSymbolName()->data()), MakeInt<int64_t>(i)} });
			}
#endif
			llvm::Type* i8Type = llvm::Type::getInt8Ty(LLVMCONTEXT);
			llvm::Type* i8Ptr = i8Type->getPointerTo();
			builder->CreateRet(builder->CreateBitCast(*((*env)[0]), i8Ptr));
			llvm::raw_os_ostream out(std::cout);
			//For some reason, verifyFunction is supposed to return false if there are no problems
			if (verifyFunction(*fun, &out))
			{
				out.flush();
				std::cout << "Could not verify static method!";
				fun->print(out);
				out.flush();
				std::cout.flush();
				//throw name;
			}
			return fun;
		}

		llvm::Function* NomCLibInstance::createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const {
			/*
			Creates an LLVM function and links it to an exisiting definition.
			Symbol name is the name of the function (create foo function) (based on c++ code, can link against and call this foo)
			Mod is the LLVM, module -> Drop everything into. (Code is meant so that you can make multiple modules)
			Environment is the book-keeping data to compile things.

			Calling convention -> the C calling convention.
			*/

			linkage = llvm::GlobalValue::LinkageTypes::ExternalLinkage;
			auto fun = llvm::Function::Create(GetLLVMFunctionType(), linkage, *GetSymbolName(), &mod);
			fun->setCallingConv(llvm::CallingConv::C);

			NomBuilder builder;
			BasicBlock* startBlock = BasicBlock::Create(LLVMCONTEXT, *GetSymbolName() + "$start", fun);
			builder->SetInsertPoint(startBlock);



			CLibInstanceCompileEnv smenv = CLibInstanceCompileEnv(regcount, *GetSymbolName(), fun, &(this->phiNodes), typeArgs, argTypes, receiver, builder);
			std::vector<NomTypeRef> recTypeArgValuesArray = {};
			
			const TypeList recTypeArgValues = ArrayRef<NomTypeRef>(recTypeArgValuesArray);

			CompileEnv* env = &smenv;

			InitializePhis(builder, fun, env);

#ifdef INSTRUCTIONMESSAGES
			auto dbgfun = GetDebugPrint(&mod);
#endif	
			llvm::ArrayRef<NomTypeRef> functionTypes = ArrayRef<NomTypeRef>(argTypes);

			for (int i = 0; i < GetArgumentCount(); i++) {
				ArgumentInstruction argInst = ArgumentInstruction(i+1);
				argInst.Compile(builder, env, 0);
			}

			std::vector<NomTypeRef> typeArgValuesArray = {};
			for (int i = 0; i < env->GetLocalTypeArgumentCount(); i++) {
				typeArgValuesArray.push_back((NomTypeRef)(env->GetTypeArgument(builder, i).GetVariable()));
			}
			const TypeList typeArgValues = ArrayRef<NomTypeRef>(typeArgValuesArray);

			CallCheckedInstanceMethod callInst = CallCheckedInstanceMethod(0, NULL, NULL, 0);
			callInst.CompileDirectly(method, functionTypes, builder, env, 0);
#ifdef INSTRUCTIONMESSAGES
			if (!env->basicBlockTerminated)
			{
				builder->CreateCall(dbgfun, { { GetLLVMPointer(this->GetSymbolName()->data()), MakeInt<int64_t>(i)} });
			}
#endif
			if (GetReturnType(nullptr)->GetLLVMType() == REFTYPE) {
				llvm::Type* i8Type = llvm::Type::getInt8Ty(LLVMCONTEXT);
				llvm::Type* i8Ptr = i8Type->getPointerTo();
				builder->CreateRet(builder->CreateBitCast(*((*env)[0]), i8Ptr));
			}
			else if (GetReturnType(nullptr)->GetLLVMType() == FLOATTYPE){
				builder->CreateRet(EnsurePackedUnpacked(builder, *((*env)[0]), FLOATTYPE));
			}
			else {
				builder->CreateRet(EnsurePackedUnpacked(builder, *((*env)[0]), INTTYPE));
			}
			llvm::raw_os_ostream out(std::cout);
			//For some reason, verifyFunction is supposed to return false if there are no problems
			if (verifyFunction(*fun, &out))
			{
				out.flush();
				std::cout << "Could not verify static method!";
				fun->print(out);
				out.flush();
				std::cout.flush();
				//throw name;
			}
			return fun;
		}

		llvm::FunctionType* NomCLib::GetLLVMFunctionType(const NomSubstitutionContext* context) const
		{
			
			std::vector<llvm::Type* > args(GetArgumentCount() + typeArgs.size());
			auto argtypes = (GetArgumentTypes(context));

			for (int i = 0; i < typeArgs.size(); i++) {
				llvm::Type* i8Type = llvm::Type::getInt8Ty(LLVMCONTEXT);
				llvm::Type* i8Ptr = i8Type->getPointerTo();
				args[i] = i8Ptr;
			}
			
			for (int i = 0; i < GetArgumentCount(); i++)
			{  
				if (argtypes[i]->GetLLVMType() == REFTYPE) {
					llvm::Type* i8Type = llvm::Type::getInt8Ty(LLVMCONTEXT);
					llvm::Type* i8Ptr = i8Type->getPointerTo();
					args[i+typeArgs.size()] = i8Ptr;
				}
				else {
					args[i + typeArgs.size()] = argtypes[i]->GetLLVMType();
				}
			}

			if (GetReturnType(context)->GetLLVMType() == REFTYPE) {
				llvm::Type* i8Type = llvm::Type::getInt8Ty(LLVMCONTEXT);
				llvm::Type* i8Ptr = i8Type->getPointerTo();
				return llvm::FunctionType::get(i8Ptr, args, false);
			}
			return llvm::FunctionType::get(GetReturnType(context)->GetLLVMType(), args, false);
			
		}

		llvm::FunctionType* NomCLibInstance::GetLLVMFunctionType(const NomSubstitutionContext* context) const
		{

			std::vector<llvm::Type* > args(GetArgumentCount() + typeArgs.size() +1);
			auto argtypes = (GetArgumentTypes(context));

			if (receiver->GetLLVMType() == REFTYPE) {
				llvm::Type* i8Type = llvm::Type::getInt8Ty(LLVMCONTEXT);
				llvm::Type* i8Ptr = i8Type->getPointerTo();
				args[0] = i8Ptr;
			}
			else {
				args[0] = receiver->GetLLVMType();
			}

			for (int i = 0; i < typeArgs.size(); i++) {
				llvm::Type* i8Type = llvm::Type::getInt8Ty(LLVMCONTEXT);
				llvm::Type* i8Ptr = i8Type->getPointerTo();
				args[i+1] = i8Ptr;
			}

			for (int i = 0; i < GetArgumentCount(); i++)
			{
				if (argtypes[i]->GetLLVMType() == REFTYPE) {
					llvm::Type* i8Type = llvm::Type::getInt8Ty(LLVMCONTEXT);
					llvm::Type* i8Ptr = i8Type->getPointerTo();
					args[i + typeArgs.size()+1] = i8Ptr;
				}
				else {
					args[i + typeArgs.size() +1] = argtypes[i]->GetLLVMType();
				}
			}

			if (GetReturnType(context)->GetLLVMType() == REFTYPE) {
				llvm::Type* i8Type = llvm::Type::getInt8Ty(LLVMCONTEXT);
				llvm::Type* i8Ptr = i8Type->getPointerTo();
				return llvm::FunctionType::get(i8Ptr, args, false);
			}
			return llvm::FunctionType::get(GetReturnType(context)->GetLLVMType(), args, false);

		}

		llvm::Function* NomCLib::findLLVMElement(llvm::Module& mod) const
		{
			return mod.getFunction(*GetSymbolName());
		}

		NomTypeRef NomCLib::GetReturnType(const NomSubstitutionContext* context) const
		{
			if (context != nullptr && context->GetTypeArgumentCount() && returnType->ContainsVariables())
			{
				return returnType->SubstituteSubtyping(context);
			}
			return returnType;
		}

		TypeList NomCLib::GetArgumentTypes(const NomSubstitutionContext* context) const
		{
			if (context == nullptr || context->GetTypeArgumentCount() == 0 || argTypes.size() == 0)
			{
				return argTypes;
			}
			bool hasVariables = false;
			for (auto at : argTypes)
			{
				if (at->ContainsVariables())
				{
					hasVariables = true;
					break;
				}
			}
			if (!hasVariables)
			{
				return argTypes;
			}
			NomTypeRef* tarr = (NomTypeRef*)(gcalloc(sizeof(NomTypeRef) * argTypes.size()));
			for (size_t i = 0; i < argTypes.size(); i++)
			{
				tarr[i] = argTypes[i]->SubstituteSubtyping(context);
			}
			return TypeList(tarr, argTypes.size());
		}

		int NomCLib::GetArgumentCount() const
		{
			
			if (argTypes.data() != nullptr)
			{
				return argTypes.size();
			}
			return 0;

		}
	}
}
