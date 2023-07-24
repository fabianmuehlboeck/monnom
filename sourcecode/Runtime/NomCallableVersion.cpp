#include "NomCallableVersion.h"
#include "NomCallable.h"
#include "CompileHelpers.h"
#include "TypeOperations.h"

using namespace llvm;
using namespace std;
namespace Nom
{
	namespace Runtime
	{
		size_t NomCallableVersion::cvcounter()
		{
			static size_t counter = 0; return ++counter;
		}
		NomCallableVersion::NomCallableVersion(const NomCallable* callable, llvm::FunctionType* functionType) : cvid(cvcounter()), Callable(callable), FunType(functionType)
		{
		}
		llvm::Function* NomCallableVersion::createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const
		{
			std::string refname = "RT_NOM_CV_" + to_string(cvid) + "_" + *Callable->GetSymbolName();

			Function* cfun = Callable->GetLLVMElement(mod);
			FunctionType* cfuntype = cfun->getFunctionType();

			Function* fun = Function::Create(FunType, linkage, refname, &mod);
			fun->setCallingConv(cfun->getCallingConv());

			NomBuilder builder;

			BasicBlock* bb = BasicBlock::Create(LLVMCONTEXT, "", fun);

			builder->SetInsertPoint(bb);

			unsigned int argcount = FunType->getNumParams();
			llvm::Value** argbuf = makealloca(llvm::Value*, argcount);

			llvm::Argument* arg = fun->arg_begin();
			for (unsigned int i = 0; i != argcount; i++, arg++)
			{
				argbuf[i] = arg;
				auto argtype = cfuntype->getFunctionParamType(i);
				if (arg->getType() != argtype)
				{
					if (argtype->isIntegerTy(1))
					{
						argbuf[i] = UnpackBool(builder, argbuf[i]);
					}
					else if (argtype->isIntegerTy(static_cast<unsigned int>(INTTYPE->getPrimitiveSizeInBits())))
					{
						argbuf[i] = UnpackInt(builder, argbuf[i]);
					}
					else if (argtype->isFloatingPointTy())
					{
						argbuf[i] = UnpackFloat(builder, argbuf[i]);
					}
					else
					{
						auto valtype = arg->getType();
						if (valtype->isIntegerTy(1))
						{
							argbuf[i] = PackBool(builder, argbuf[i]);
						}
						else if (valtype->isIntegerTy(static_cast<unsigned int>(INTTYPE->getPrimitiveSizeInBits())))
						{
							argbuf[i] = PackInt(builder, argbuf[i]);
						}
						else if (valtype->isFloatingPointTy())
						{
							argbuf[i] = PackFloat(builder, argbuf[i]);
						}
						else
						{
							throw new std::exception();
						}
					}
				}
			}
			auto callinst = builder->CreateCall(cfun, llvm::ArrayRef<llvm::Value*>(argbuf, argcount));
			callinst->setCallingConv(cfun->getCallingConv());

			if (callinst->getType() == fun->getReturnType())
			{
				builder->CreateRet(callinst);
			}
			else if (callinst->getType() == REFTYPE)
			{
				if (fun->getReturnType()->isIntegerTy(1))
				{
					builder->CreateRet(UnpackBool(builder, callinst));
				}
				else if (fun->getReturnType()->isIntegerTy(static_cast<unsigned int>(INTTYPE->getPrimitiveSizeInBits())))
				{
					builder->CreateRet(UnpackInt(builder, callinst));
				}
				else if (fun->getReturnType()->isFloatingPointTy())
				{
					builder->CreateRet(UnpackFloat(builder, callinst));
				}
				else
				{
					throw new std::exception();
				}
			}
			else if (callinst->getType()->isIntegerTy(1))
			{
				if (fun->getReturnType() == REFTYPE)
				{
					builder->CreateRet(PackBool(builder, callinst));
				}
				else
				{
					throw new std::exception();
				}
			}
			else if (callinst->getType()->isIntegerTy(static_cast<unsigned int>(INTTYPE->getPrimitiveSizeInBits())))
			{
				if (fun->getReturnType() == REFTYPE)
				{
					builder->CreateRet(PackInt(builder, callinst));
				}
				else
				{
					throw new std::exception();
				}
			}
			else if (callinst->getType()->isFloatingPointTy())
			{
				if (fun->getReturnType() == REFTYPE)
				{
					builder->CreateRet(PackFloat(builder, callinst));
				}
				else
				{
					throw new std::exception();
				}
			}

			return fun;
		}
		llvm::Function* NomCallableVersion::findLLVMElement(llvm::Module& mod) const
		{
			if (this->FunType == Callable->GetLLVMFunctionType())
			{
				return Callable->GetLLVMElement(mod);
			}
			std::string refname = "RT_NOM_CV_" + to_string(cvid) + "_" + *Callable->GetSymbolName();
			return mod.getFunction(refname);
		}
	}
}
