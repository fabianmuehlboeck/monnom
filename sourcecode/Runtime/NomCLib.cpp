#include "NomCLib.h"
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
		
		llvm::Function* NomCLibStatic::createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const {
		
			/*
			Creates an LLVM function with the same function type as the given C++ function.
			*/
			auto fun = llvm::Function::Create(GetLLVMFunctionType(), llvm::GlobalValue::LinkageTypes::ExternalLinkage, *GetSymbolName(), &mod);
			fun->setCallingConv(llvm::CallingConv::C);

			/*
			Instantiates the IR builder and starts the basic block
			*/
			NomBuilder builder;
			BasicBlock* startBlock = BasicBlock::Create(LLVMCONTEXT, *GetSymbolName() + "$start", fun);
			builder->SetInsertPoint(startBlock);

			/*
			Creates a compile environment unique to this type of function generation. The compile environment is responsible
			for storing the environment type arguments as well as reading and casting the LLVM argument values of the function call. 
			*/
			CLibStaticCompileEnv smenv = CLibStaticCompileEnv(regcount, *GetSymbolName(), fun, &(this->phiNodes), typeArgs, argTypes, builder);
			CompileEnv* env = &smenv;

			InitializePhis(builder, fun, env);
			
#ifdef INSTRUCTIONMESSAGES
			auto dbgfun = GetDebugPrint(&mod);
#endif	

			/*
			The argument instructions specify which arguments should be used for the next instruction.
			In this case, all environment arguments should be used. 
			*/
			for (int i = 0; i < GetArgumentCount(); i++) {
				ArgumentInstruction argInst = ArgumentInstruction(i);
				argInst.Compile(builder, env, 0);
			}

			/*
			Creates an instruction to call a static method (the method specified in the specification)
			and JIT compiles it to LLVM IR. "CompileActual" skips processing parser inputs, and directly
			compiles it with the instantiation given by the Specification File Parser. 
			*/
			CallCheckedStaticMethod callInst = CallCheckedStaticMethod(NULL, NULL, 0);
			callInst.CompileActual(method, method.TypeArgs, builder, env, 0);

#ifdef INSTRUCTIONMESSAGES
			if (!env->basicBlockTerminated)
			{
				builder->CreateCall(dbgfun, { { GetLLVMPointer(this->GetSymbolName()->data()), MakeInt<int64_t>(i)} });
			}
#endif
			/*
			Creates the return for the generated function
			*/
			createCastedReturn(builder, env);

			/*
			Verifies the function to make sure the structure and types are correct for this function.
			*/
			llvm::raw_os_ostream out(std::cout);
			//For some reason, verifyFunction is supposed to return false if there are no problems
			if (verifyFunction(*fun, &out))
			{
				out.flush();
				std::cout << "Could not verify static method!";
				fun->print(out);
				out.flush();
				std::cout.flush();
				throw GetSymbolName();
			}
			return fun;
		}

		llvm::Function* NomCLibConstructor::createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const {

			/*
			Creates an LLVM function with the same function type as the given C++ function.
			*/
			auto fun = llvm::Function::Create(GetLLVMFunctionType(), llvm::GlobalValue::LinkageTypes::ExternalLinkage, *GetSymbolName(), &mod);
			fun->setCallingConv(llvm::CallingConv::C);

			/*
			Instantiates the IR builder and starts the basic block
			*/
			NomBuilder builder;
			BasicBlock* startBlock = BasicBlock::Create(LLVMCONTEXT, *GetSymbolName() + "$start", fun);
			builder->SetInsertPoint(startBlock);

			/*
			Creates a compile environment unique to this type of function generation. The compile environment is responsible
			for storing the environment type arguments as well as reading and casting the LLVM argument values of the function call.
			*/
			CLibStaticCompileEnv smenv = CLibStaticCompileEnv(regcount, *GetSymbolName(), fun, &(this->phiNodes), typeArgs, argTypes, builder);
			CompileEnv* env = &smenv;

			InitializePhis(builder, fun, env);

#ifdef INSTRUCTIONMESSAGES
			auto dbgfun = GetDebugPrint(&mod);
#endif	
			/*
			The argument instructions specify which arguments should be used for the next instruction.
			In this case, all environment arguments should be used.
			*/
			for (int i = 0; i < GetArgumentCount(); i++) {
				ArgumentInstruction argInst = ArgumentInstruction(i);
				argInst.Compile(builder, env, 0);
			}

			/*
			Creates an instruction to call a constructor (the constructor in the specification)
			and JIT compiles it to LLVM IR. "CompileActual" skips processing parser inputs, and directly
			compiles it with the instantiation given by the Specification File Parser.
			*/
			CallConstructor callInst = CallConstructor(NULL, NULL, 0);
			callInst.CompileActual(classRef, builder, env, 0);

#ifdef INSTRUCTIONMESSAGES
			if (!env->basicBlockTerminated)
			{
				builder->CreateCall(dbgfun, { { GetLLVMPointer(this->GetSymbolName()->data()), MakeInt<int64_t>(i)} });
			}
#endif
			/*
			Creates the return for the generated function
			*/
			createCastedReturn(builder, env);

			/*
			Verifies the function to make sure the structure and types are correct for this function.
			*/
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
			Creates an LLVM function with the same function type as the given C++ function.
			*/
			auto fun = llvm::Function::Create(GetLLVMFunctionType(), llvm::GlobalValue::LinkageTypes::ExternalLinkage, *GetSymbolName(), &mod);
			fun->setCallingConv(llvm::CallingConv::C);

			/*
			Instantiates the IR builder and starts the basic block
			*/
			NomBuilder builder;
			BasicBlock* startBlock = BasicBlock::Create(LLVMCONTEXT, *GetSymbolName() + "$start", fun);
			builder->SetInsertPoint(startBlock);

			/*
			Creates a compile environment unique to this type of function generation. The compile environment is responsible
			for storing the environment type arguments as well as reading and casting the LLVM argument values of the function call.
			In this case, the compiler also includes a receiver argument, the type of the receiver (the *this*) of a function
			*/
			CLibInstanceCompileEnv smenv = CLibInstanceCompileEnv(regcount, *GetSymbolName(), fun, &(this->phiNodes), typeArgs, argTypes, receiver, builder);
			std::vector<NomTypeRef> recTypeArgValuesArray = {};
			CompileEnv* env = &smenv;

			InitializePhis(builder, fun, env);

#ifdef INSTRUCTIONMESSAGES
			auto dbgfun = GetDebugPrint(&mod);
#endif	
			/*
			The argument instructions specify which arguments should be used for the next instruction.
			In this case, all environment arguments should be used.
			*/
			for (int i = 0; i < GetArgumentCount(); i++) {
				ArgumentInstruction argInst = ArgumentInstruction(i+1);
				argInst.Compile(builder, env, 0);
			}

			/*
			Creates an instruction to call an instance method (the method in the specification)
			and JIT compiles it to LLVM IR. "CompileActual" skips processing parser inputs, and directly
			compiles it with the instantiation given by the Specification File Parser.
			*/
			CallCheckedInstanceMethod callInst = CallCheckedInstanceMethod(0, NULL, NULL, 0);
			callInst.CompileActual(method, builder, env, 0);
#ifdef INSTRUCTIONMESSAGES
			if (!env->basicBlockTerminated)
			{
				builder->CreateCall(dbgfun, { { GetLLVMPointer(this->GetSymbolName()->data()), MakeInt<int64_t>(i)} });
			}
#endif
			/*
			Creates the return for the generated function
			*/
			createCastedReturn(builder, env);

			/*
			Verifies the function to make sure the structure and types are correct for this function.
			*/
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
			/*
			Gets the llvm Function Type, used for the static method and constructor calls.
			Casts the arguments to what C++ uses for the function declaration, as this function
			declaration must match the C++ declaration perfectly, therefore pointer types are casted
			to i8* types. Adds all arguments and type arguments into the definition.
			*/
			std::vector<llvm::Type* > args(GetArgumentCount() + typeArgs.size());
			auto argtypes = (GetArgumentTypes(context));

			for (int i = 0; i < typeArgs.size(); i++) {
				args[i] = GetCPointerType();
			}
			
			for (int i = 0; i < GetArgumentCount(); i++)
			{  
				args[i+typeArgs.size()] = GetLLVMCastedType(argtypes[i]);
			}
			/*
			Specifies the return type of the function
			*/
			return llvm::FunctionType::get(GetLLVMCastedType(GetReturnType(context)), args, false);
			
		}

		llvm::FunctionType* NomCLibInstance::GetLLVMFunctionType(const NomSubstitutionContext* context) const
		{
			/*
			Gets the llvm Function Type, used for the static method and constructor calls.
			Casts the arguments to what C++ uses for the function declaration, as this function
			declaration must match the C++ declaration perfectly, therefore pointer types are casted
			to i8* types. Adds all arguments and type arguments into the definition. Additionally,
			the receiver of the call is added for instance methods and once again casted to a 
			C++ compatible type.
			*/
			std::vector<llvm::Type* > args(GetArgumentCount() + typeArgs.size() +1);
			auto argtypes = (GetArgumentTypes(context));
			args[0] = GetLLVMCastedType(receiver);

			for (int i = 0; i < typeArgs.size(); i++) {
				args[i+1] = GetCPointerType();
			}

			for (int i = 0; i < GetArgumentCount(); i++)
			{
				args[i + typeArgs.size()+1] = GetLLVMCastedType(argtypes[i]);
			}
			/*
			Specifies the return type of the function
			*/
			return llvm::FunctionType::get(GetLLVMCastedType(GetReturnType(context)), args, false);

		}

		llvm::Function* NomCLib::findLLVMElement(llvm::Module& mod) const
		{
			return mod.getFunction(*GetSymbolName());
		}

		NomTypeRef NomCLib::GetReturnType(const NomSubstitutionContext* context) const
		{
			/*
			Substitutes the type arguments with those known in the container. In this case, since this
			generated function is identical to a static method, there are no environment type arguments,
			and therefore within this context, it will always return the normal return type. 

			Potential TODO: Find a way to read C++ code environment type arguments and perform the substitution 
			*/
			if (context != nullptr && context->GetTypeArgumentCount() && returnType->ContainsVariables())
			{
				return returnType->SubstituteSubtyping(context);
			}
			return returnType;
		}

		TypeList NomCLib::GetArgumentTypes(const NomSubstitutionContext* context) const
		{
			/*
			Substitutes the type arguments with those known in the container. In this case, since this
			generated function is identical to a static method, there are no environment type arguments,
			and therefore within this context, it will always return the normal argument types.

			Potential TODO: Find a way to read C++ code environment type arguments and perform the substitution
			*/
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
			/*
			Gets the number of arguments
			*/
			if (argTypes.data() != nullptr)
			{
				return argTypes.size();
			}
			return 0;

		}
	}
}
