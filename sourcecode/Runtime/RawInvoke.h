#pragma once
#include <llvm/IR/Module.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include "NomInterface.h"
#include "CompileHelpers.h"
#include "Defs.h"

namespace Nom
{
	namespace Runtime
	{
		template<typename FuncType>
		void GenerateRawInvoke(NomBuilder& builder, const NomInterface* ifc, const NomMethod* baseMethod, FuncType generateCallFun, llvm::ArrayRef<llvm::Value*> argarr)
		{
			std::unordered_map<llvm::FunctionType*, std::vector<InterfaceID>> callableVersions;
			std::vector<InterfaceID> ids;
			callableVersions[baseMethod->GetLLVMFunctionType()] = std::vector<InterfaceID>({ ifc->GetID() });
			ids.push_back(ifc->GetID());
			for (auto inst : ifc->GetInstantiations())
			{
				for (auto supermethod : inst.first->Methods)
				{
					if (baseMethod->Overrides(supermethod))
					{
						auto superFType = supermethod->GetLLVMFunctionType();
						if (callableVersions.find(superFType) == callableVersions.end())
						{
							callableVersions[superFType] = std::vector<InterfaceID>();
						}
						callableVersions[superFType].push_back(inst.first->GetID());
						ids.push_back(inst.first->GetID());
					}
				}
			}

			//std::string name = "RT_NOM_RAWINVOKE_" + this->GetName()->ToStdString();
			llvm::FunctionType* mainFunType = baseMethod->GetRawInvokeLLVMFunctionType(nullptr);
			//llvm::Function* fun = Function::Create(mainFunType, linkage, name, mod);
			//BasicBlock* startBlock = BasicBlock::Create(LLVMCONTEXT, "start", fun);
			llvm::BasicBlock* baseCase = nullptr;
			llvm::SwitchInst* caseSwitch = nullptr;

			//IRBuilder<> builder;
			//builder->SetInsertPoint(startBlock);

			auto iidArg = argarr[0];
			auto argcount = mainFunType->getNumParams() - 1;
			//Value** argarr = makealloca(Value*, argcount);
			//auto args = fun->arg_begin();
			//Argument* iidArg = args;

			if (callableVersions.size() != 1)
			{
				baseCase = llvm::BasicBlock::Create(LLVMCONTEXT, "baseCase", builder->GetInsertBlock()->getParent());
				caseSwitch = builder->CreateSwitch(iidArg, baseCase, ids.size());

				builder->SetInsertPoint(baseCase);
			}
			argarr = llvm::ArrayRef<llvm::Value*>(argarr.data() + 1, argarr.size() - 1);
			{ // this is the code for the baseCase block, emitted always
				//for (decltype(argcount) i = 0; i < argcount; i++)
				//{
				//	args++;
				//	argarr[i] = args;
				//}
				//auto call = builder->CreateCall(method->GetLLVMElement(mod), ArrayRef<Value*>(argarr, argcount));
				//call->setCallingConv(NOMCC);
				builder->CreateRet(generateCallFun(builder, baseMethod, argarr));
			}

			if (caseSwitch != nullptr)
			{
				for (auto cv : callableVersions)
				{
					llvm::BasicBlock* target;
					if (cv.first == mainFunType)
					{
						target = baseCase;
					}
					else
					{
						target = llvm::BasicBlock::Create(LLVMCONTEXT, "", builder->GetInsertBlock()->getParent());
						builder->SetInsertPoint(target);
						llvm::Value** cvargarr = makealloca(Value*, argcount);
						for (decltype(argcount) i = 0; i < argcount; i++)
						{
							cvargarr[i] = argarr[i];
							auto cvtype = cv.first->getParamType(i);
							auto mftype = mainFunType->getParamType(i);
							if (cvtype != mftype)
							{
								if (cvtype == INTTYPE && mftype == REFTYPE)
								{
									cvargarr[i] = PackInt(builder, argarr[i]);
								}
								else if (cvtype == FLOATTYPE && mftype == REFTYPE)
								{
									cvargarr[i] = PackFloat(builder, argarr[i]);
								}
								else if (cvtype == BOOLTYPE && mftype == REFTYPE)
								{
									cvargarr[i] = PackBool(builder, argarr[i]);
								}
								else if (cvtype == REFTYPE && mftype == INTTYPE)
								{
									cvargarr[i] = UnpackInt(builder, argarr[i]);
								}
								else if (cvtype == REFTYPE && mftype == FLOATTYPE)
								{
									cvargarr[i] = UnpackFloat(builder, argarr[i]);
								}
								else if (cvtype == REFTYPE && mftype == BOOLTYPE)
								{
									cvargarr[i] = UnpackBool(builder, argarr[i]);
								}
								else
								{
									throw std::exception();
								}
							}
						}

						llvm::Value* retval = generateCallFun(builder, baseMethod, argarr);
						//auto call = builder->CreateCall(method->GetLLVMElement(mod), ArrayRef<Value*>(cvargarr, argcount));
						//call->setCallingConv(NOMCC);
						//Value* retval = call;

						auto cvret = cv.first->getReturnType();
						auto mfret = mainFunType->getReturnType();
						if (cvret != mfret)
						{
							if (cvret == INTTYPE && mfret == REFTYPE)
							{
								retval = UnpackInt(builder, retval);
							}
							else if (cvret == FLOATTYPE && mfret == REFTYPE)
							{
								retval = UnpackFloat(builder, retval);
							}
							else if (cvret == BOOLTYPE && mfret == REFTYPE)
							{
								retval = UnpackBool(builder, retval);
							}
							else if (cvret == REFTYPE && mfret == INTTYPE)
							{
								retval = PackInt(builder, retval);
							}
							else if (cvret == REFTYPE && mfret == FLOATTYPE)
							{
								retval = PackFloat(builder, retval);
							}
							else if (cvret == REFTYPE && mfret == BOOLTYPE)
							{
								retval = PackBool(builder, retval);
							}
							else
							{
								throw std::exception();
							}
						}
						builder->CreateRet(retval);
					}
					for (auto iid : cv.second)
					{
						caseSwitch->addCase(MakeIntLike(iidArg, iid), target);
					}
				}
			}
		}
	}
}