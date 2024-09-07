#include "LibraryTest.h"
#include "llvm/Support/raw_os_ostream.h"
#include "llvm/IR/Verifier.h"
#include "CallingConvConf.h"
#include "NomCallable.h"
#include "NomTypeDecls.h"
#include "NomTypeParameter.h"
#include "instructions/ArgumentInstruction.h"
#include "instructions/CastInstruction.h"


namespace Nom {
	namespace Runtime {

		LibraryTest::LibraryTest() {

		}

		void LibraryTest::print_test() {

		}
		
		llvm::Function* LibraryTest::createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const {
			/*
			Creates an LLVM function and links it to an exisiting definition.
			Symbol name is the name of the function (create foo function) (based on c++ code, can link against and call this foo)
			Mod is the LLVM, module -> Drop everything into. (Code is meant so that you can make multiple modules)
			Environment is the book-keeping data to compile things.

			Calling convention -> the C calling convention.
			*/

			symname = "foo7348";
			linkage = llvm::GlobalValue::LinkageTypes::ExternalLinkage;
			auto fun = llvm::Function::Create(GetLLVMFunctionType(), linkage, *GetSymbolName(), &mod);
			fun->setCallingConv(llvm::CallingConv::C);

			NomBuilder builder;
			BasicBlock* startBlock = BasicBlock::Create(LLVMCONTEXT, *GetSymbolName() + "$start", fun);
			builder->SetInsertPoint(startBlock);

			CLibCompileEnv smenv = CLibCompileEnv(regcount, *GetSymbolName(), fun, &(this->phiNodes), typeArgs, argTypes, NULL, builder);
			CompileEnv* env = &smenv;

			InitializePhis(builder, fun, env);

			
#ifdef INSTRUCTIONMESSAGES
			auto dbgfun = GetDebugPrint(&mod);
#endif
		
			llvm::ArrayRef<NomTypeRef> functionTypes = ArrayRef<NomTypeRef>(argTypes);

			int nPTypes = 0;
			for (int i = 0; i < GetArgumentCount(); i++) {
				if (argTypes[i]->GetLLVMType() == REFTYPE) {
					llvm::Argument* arg = fun->getArg(i + typeArgs.size());
					llvm::Value* val = builder->CreateBitCast(arg, REFTYPE, "cA");
					(*env)[GetArgumentCount() + nPTypes] = NomValue(val, argTypes[i]);
					ArgumentInstruction argInst = ArgumentInstruction(GetArgumentCount() + nPTypes);
					argInst.Compile(builder, env, 0);
					nPTypes++;
				}
				else {
					ArgumentInstruction argInst = ArgumentInstruction(i);
					argInst.Compile(builder, env, 0);
				}
			}

			std::vector<NomTypeRef> tA1 = {};
			//const TypeList tA = ArrayRef<NomTypeRef>((const NomTypeRef*)NomIntClass::GetInstance()->GetType(), (size_t)1);
			const TypeList tA = ArrayRef<NomTypeRef>(tA1);
			CallCheckedStaticMethod callInst = CallCheckedStaticMethod(NULL, NULL, 0);
			callInst.CompileDirectly(className, methodName, tA, functionTypes, builder, env, 0);
#ifdef INSTRUCTIONMESSAGES
			if (!env->basicBlockTerminated)
			{
				builder->CreateCall(dbgfun, { { GetLLVMPointer(this->GetSymbolName()->data()), MakeInt<int64_t>(i)} });
			}
#endif
			llvm::Type* i8Type = llvm::Type::getInt8Ty(LLVMCONTEXT);
			llvm::Type* i8Ptr = i8Type->getPointerTo();
			builder->CreateRet(builder->CreateBitCast(*((*env)[0]), i8Ptr));
			//builder->CreateRetVoid();
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

		llvm::FunctionType* LibraryTest::GetLLVMFunctionType(const NomSubstitutionContext* context) const
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



		llvm::Function* LibraryTest::findLLVMElement(llvm::Module& mod) const
		{
			return mod.getFunction(*GetSymbolName());
		}

		NomTypeRef LibraryTest::GetReturnType(const NomSubstitutionContext* context) const
		{
			if (context != nullptr && context->GetTypeArgumentCount() && returnType->ContainsVariables())
			{
				return returnType->SubstituteSubtyping(context);
			}
			return returnType;
		}

		TypeList LibraryTest::GetArgumentTypes(const NomSubstitutionContext* context) const
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

		int LibraryTest::GetArgumentCount() const
		{
			
			if (argTypes.data() != nullptr)
			{
				return argTypes.size();
			}
			return 0;

		}
	}
}
