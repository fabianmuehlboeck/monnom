#include "NomPartialApplication.h"
#include "RTPartialApp.h"
#include <unordered_map>
#include "RTSubtyping.h"
#include "CompileHelpers.h"
PUSHDIAGSUPPRESSION
#include "llvm/Support/raw_os_ostream.h"
#include "llvm/IR/Verifier.h"
POPDIAGSUPPRESSION
#include <iostream>
#include "RTOutput.h"
#include "NomNameRepository.h"
#include "RTDescriptor.h"
#include "TypeOperations.h"
#include "RTCast.h"
#include "NomDynamicType.h"
#include "NomTypeVar.h"
#include "NomMemberContext.h"
#include "NomTypeParameter.h"
#include "IntClass.h"
#include "FloatClass.h"
#include "CallingConvConf.h"
#include "CompileEnv.h"
#include "RTCompileConfig.h"
#include "PWObject.h"
#include "PWPartialApp.h"
#include "PWTypeArr.h"
#include "PWType.h"

using namespace std;
using namespace llvm;
namespace Nom
{
	namespace Runtime
	{
		class NomPartialApplicationDispatcherEnv : public ACompileEnv
		{
		public:
			const NomCallable* const Callable;
			NomPartialApplicationDispatcherEnv(llvm::Value* _receiver, RegIndex _regcount, const llvm::Twine _contextName, llvm::Function* _function, llvm::ArrayRef<NomTypeParameterRef> _typeParams, llvm::ArrayRef<llvm::Value*> _typeArgValues, const NomCallable* _method) : ACompileEnv(_regcount, _contextName, _function, _method->GetParent(), nullptr, _typeParams, _typeArgValues), Callable(_method)
			{
				registers[0] = _receiver;
			}
			// Inherited via CompileEnv
			virtual NomTypeVarValue GetTypeArgument(NomBuilder& builder, size_t i) override
			{
				if (i < Callable->GetTypeParametersStart())
				{
					return NomTypeVarValue(ObjectHeader::GenerateReadTypeArgument(builder, registers[0], i + Callable->GetParent()->GetTypeParametersStart()), Context->GetTypeParameter(i)->GetVariable());
				}
				else
				{
					return TypeArguments[i - Callable->GetTypeParametersStart()];
				}
			}
			virtual size_t GetEnvTypeArgumentCount() override
			{
				return Callable->GetTypeParametersCount() - Callable->GetDirectTypeParametersCount();
			}
			virtual PWTypeArr GetEnvTypeArgumentArray(NomBuilder& builder) override
			{
				return PWObject(registers[0]).PointerToTypeArguments(builder).SubArr(builder, PWInt32(Callable->GetParent()->GetTypeParametersStart(), false));
			}
			virtual bool GetInConstructor() override
			{
				return false;
			}
		};
		class NomPartialApplicationDispatcherEntry : public NomCallable, public NomMemberContextInternal
		{
		private:
			const std::string name;
			const std::string qname;
			llvm::ArrayRef<const NomCallable*> overloadings;
		public:
			NomPartialApplicationDispatcherEntry(const NomMemberContext* _parent, llvm::ArrayRef<const NomCallable*> _overloadings) : NomCallable(), NomMemberContextInternal(_parent), name("NOM_OD_" + _overloadings[0]->GetQName()), qname("NOM_OD_" + _overloadings[0]->GetQName()), overloadings(_overloadings)
			{
			}
			// Inherited via NomCallable
			virtual Function* createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const override
			{
				NomBuilder builder;
				std::string funname = "NOM_OD_" + overloadings[0]->GetQName();
				Function* dispatcher = Function::Create(NomPartialApplication::GetDynamicDispatcherType(), linkage, funname, &mod);
				dispatcher->setCallingConv(NOMCC);

				BasicBlock* dispblock = BasicBlock::Create(LLVMCONTEXT, "", dispatcher);
				builder->SetInsertPoint(dispblock);

				auto dargs = dispatcher->arg_begin();
				Value* typeArgCountVal = dargs;
				dargs++;
				Value* argCountVal = dargs;
				dargs++;
				Value* receiver = dargs;
				dargs++;
				Value** restArgs = makealloca(Value*, RTConfig_NumberOfVarargsArguments);
				for (decltype(RTConfig_NumberOfVarargsArguments) i = 0; i < RTConfig_NumberOfVarargsArguments; i++)
				{
					restArgs[i] = dargs;
					dargs++;
				}

				for (auto& meth : overloadings)
				{
					BasicBlock* currentBlock = BasicBlock::Create(LLVMCONTEXT, "argCountMatch", dispatcher);
					BasicBlock* nextBlock = BasicBlock::Create(LLVMCONTEXT, "", dispatcher);
					auto typeParams = meth->GetDirectTypeParameters();
					decltype(typeParams.size()) typeArgCount = typeParams.size();
					auto argCount = meth->GetArgumentCount();
					auto parameterCountMatch = builder->CreateAnd(builder->CreateICmpEQ(typeArgCountVal, MakeIntLike(typeArgCountVal, typeArgCount)),
						builder->CreateICmpEQ(argCountVal, MakeIntLike(argCountVal, argCount)));
					builder->CreateCondBr(parameterCountMatch, currentBlock, nextBlock);

					builder->SetInsertPoint(currentBlock);
					Value** targValueArr = nullptr;

					llvm::Value** methodargs = makealloca(llvm::Value*, typeArgCount + 1 + argCount);
					llvm::Value** valargs = nullptr;
					methodargs[0] = receiver;

					if (typeArgCount > 0)
					{
						targValueArr = makealloca(Value*, typeArgCount);
						for (decltype(typeArgCount) i = 0; i < typeArgCount; i++)
						{
							if (i < RTConfig_NumberOfVarargsArguments - (typeArgCount + argCount > RTConfig_NumberOfVarargsArguments ? 1 : 0))
							{
								targValueArr[i] = builder->CreatePointerCast(restArgs[i], TYPETYPE);
							}
							else
							{
								targValueArr[i] = MakeLoad(builder, TYPETYPE, builder->CreateGEP(TYPETYPE, restArgs[RTConfig_NumberOfVarargsArguments - 1], MakeInt32(i - (RTConfig_NumberOfVarargsArguments - 1))));
							}
							methodargs[i + 1] = targValueArr[i];
						}
					}
					NomSubstitutionContextMemberContext nscmc(this);
					NomPartialApplicationDispatcherEnv padenv(receiver, argCount + 1, funname, dispatcher, typeParams, ArrayRef<llvm::Value*>(targValueArr, typeArgCount), this);
					//TODO: check type argument constraints, if any
					CompileEnv* env = &padenv;
					auto methargtypes = meth->GetArgumentTypes(&nscmc);
					if (argCount > 0)
					{
						valargs = makealloca(llvm::Value*, argCount);
						for (decltype(argCount) i = 0; i < argCount; i++)
						{
							auto expectedArgType = methargtypes[i];
							if (i + typeArgCount < RTConfig_NumberOfVarargsArguments - (typeArgCount + argCount > RTConfig_NumberOfVarargsArguments ? 1 : 0))
							{
								auto nv = NomValue(builder->CreatePointerCast(restArgs[i + typeArgCount], REFTYPE), false);
								valargs[i] = EnsureType(builder, env, nv, expectedArgType, meth->GetLLVMFunctionType()->getParamType(static_cast<unsigned int>(1 + i + typeArgCount)));
							}
							else
							{
								auto nv = NomValue(MakeLoad(builder, REFTYPE, builder->CreateGEP(REFTYPE, restArgs[RTConfig_NumberOfVarargsArguments - RTConfig_NumberOfVarargsArguments], MakeInt32(i + typeArgCount - (RTConfig_NumberOfVarargsArguments - 1)))), false);
								valargs[i] = EnsureType(builder, env, nv, expectedArgType, meth->GetLLVMFunctionType()->getParamType(static_cast<unsigned int>(1 + i + typeArgCount)));
							}
							methodargs[i + typeArgCount + 1] = valargs[i];
						}
					}
					auto fcargs = ArrayRef<llvm::Value*>(methodargs, typeArgCount + 1 + argCount);
					auto methcall = GenerateFunctionCall(builder, mod, meth->GetLLVMElement(mod), fcargs, true);
					if (methcall->getCallingConv() == llvm::CallingConv::Fast)
					{
						methcall->setTailCallKind(llvm::CallInst::TailCallKind::TCK_Tail);
					}
					auto mctype = methcall->getType();
					llvm::Value* retval = methcall;
					if (mctype->isIntegerTy(1))
					{
						retval = PackBool(builder, methcall);
					}
					else if (mctype->isIntegerTy(static_cast<unsigned int>(INTTYPE->getPrimitiveSizeInBits())))
					{
						retval = PackInt(builder, methcall);
					}
					else if (mctype->isFloatingPointTy())
					{
						retval = PackFloat(builder, methcall);
					}
					builder->CreateRet(retval);
					builder->SetInsertPoint(nextBlock);
				}
				static const char* failstr = "Could not find applicable method!";
				builder->CreateRet(builder->CreateCall(RTOutput_Fail::GetLLVMElement(mod), { GetLLVMPointer(failstr) }));

				llvm::raw_os_ostream out(std::cout);
				if (verifyFunction(*dispatcher, &out))
				{
					out.flush();
					std::cout << "Could not verify Dispatcher for method name ";
					std::cout << overloadings[0]->GetName();
					dispatcher->print(out);
					out.flush();
					std::cout.flush();
					throw new std::exception();
				}
				return dispatcher;
			}
			virtual size_t GetArgumentCount() const override
			{
				throw new std::exception();
				//return argCount;
			}

			// Inherited via NomCallable
			virtual NomTypeRef GetReturnType(const NomSubstitutionContext* context) const override
			{
				NomTypeRef ret = nullptr;
				for (auto ovl : overloadings)
				{
					if (ret == nullptr)
					{
						ret = ovl->GetReturnType(context);
					}
					else
					{
						ret = JoinTypes(ret, ovl->GetReturnType(context));
					}
				}
				return ret;
			}
			virtual TypeList GetArgumentTypes([[maybe_unused]] const NomSubstitutionContext* context) const override
			{
				throw new std::exception();
				//return argTypes;
			}
			virtual llvm::FunctionType* GetLLVMFunctionType(const NomSubstitutionContext* context = nullptr) const override
			{
				std::vector<llvm::Type* > args(GetDirectTypeParametersCount() + GetArgumentCount() + 1);
				size_t j = 0;
				if (GetParent() == NomIntClass::GetInstance())
				{
					args[j] = INTTYPE;
				}
				else if (GetParent() == NomFloatClass::GetInstance())
				{
					args[j] = FLOATTYPE;
				}
				else
				{
					args[j] = REFTYPE;
				}
				for (j = 1; j <= GetDirectTypeParametersCount(); j++)
				{
					args[j] = TYPETYPE;
				}
				size_t i;
				auto argtypes = (GetArgumentTypes(context));
				for (i = 0; i < GetArgumentCount(); i++)
				{
					args[j + i + 1] = argtypes[i]->GetLLVMType();
				}
				return llvm::FunctionType::get(GetReturnType(context)->GetLLVMType(), args, false);
			}
			virtual const std::string& GetName() const override
			{
				return name;
			}
			virtual const std::string& GetQName() const override
			{
				return qname;
			}
			virtual llvm::ArrayRef<NomTypeParameterRef> GetArgumentTypeParameters() const override
			{
				return GetDirectTypeParameters();
			}
		};
		NomPartialApplication::NomPartialApplication(const std::string _symbolName, llvm::ArrayRef<const NomCallable*> _methods, const NomMemberContext* _context, NomTypeRef _thisType) : context(_context), thisType(_thisType), SymbolName("RT_NOM_PA_" + _symbolName)
		{
			for (const NomCallable* meth : _methods)
			{
				this->methods.push_back(meth);
			}
		}
		llvm::FunctionType* NomPartialApplication::GetDynamicDispatcherType()
		{
			static FunctionType* ft = nullptr;
			static bool once = false;
			if (!once)
			{
				Type** argtypes = makealloca(Type*, 3 + RTConfig_NumberOfVarargsArguments);
				argtypes[0] = numtype(int32_t);
				argtypes[1] = numtype(int32_t);
				argtypes[2] = REFTYPE;
				for (decltype(RTConfig_NumberOfVarargsArguments) i = 0; i < RTConfig_NumberOfVarargsArguments; i++)
				{
					argtypes[i + 3] = POINTERTYPE;
				}
				ft = llvm::FunctionType::get(REFTYPE, ArrayRef<Type*>(argtypes, 3 + RTConfig_NumberOfVarargsArguments), false);
				once = true;
			}
			return ft;
		}

		llvm::Function* NomPartialApplication::GetDispatcherEntry(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage,/* int32_t typeArgCount, int32_t argCount,*/ llvm::ArrayRef<const NomCallable*> overloadings, const NomMemberContext* context/*, NomTypeRef thisType*/)
		{
			return ((new NomPartialApplicationDispatcherEntry(context, overloadings/*, typeArgCount, argCount, thisType*/))->createLLVMElement(mod, linkage));
		}

		llvm::Constant* NomPartialApplication::createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const
		{
			unordered_map<size_t, unordered_map<size_t, vector<const NomCallable*>>> overloadings;

			size_t dispcount = 0;

			for (const NomCallable* meth : methods)
			{
				size_t dtac = meth->GetDirectTypeParametersCount();
				auto tamatch = overloadings.find(dtac);
				if (tamatch == overloadings.end())
				{
					overloadings[dtac] = unordered_map<size_t, vector<const NomCallable* >>();
				}
				auto& ole = overloadings[dtac];
				size_t tpc = meth->GetArgumentCount();
				auto argmatch = ole.find(tpc);
				if (argmatch == ole.end())
				{
					ole[tpc] = vector<const NomCallable*>();
					dispcount++;
				}
				ole[tpc].push_back(meth);
			}

			using npa_arg = pair<pair<size_t, size_t>, Constant*>;
			auto argsbuf = makealloca(npa_arg, dispcount);
			size_t constantsBufPos = 0;

			for (auto& ole1 : overloadings)
			{
				for (auto& ole2 : ole1.second)
				{
					argsbuf[constantsBufPos] = make_pair(make_pair(ole1.first, ole2.first), ConstantExpr::getPointerCast(GetDispatcherEntry(mod, linkage, /*ole1.first, ole2.first,*/ ole2.second, context/*, thisType*/), POINTERTYPE));
					constantsBufPos++;
				}
			}
			auto cnst = RTPartialApp::CreateConstant(llvm::ArrayRef<npa_arg>(argsbuf, dispcount));
			llvm::GlobalVariable* gv = new llvm::GlobalVariable(mod, cnst->getType(), true, linkage, cnst, SymbolName);

			return ConstantExpr::getPointerCast(gv, RTPartialApp::GetLLVMType()->getPointerTo());
		}
		llvm::Constant* NomPartialApplication::findLLVMElement(llvm::Module& mod) const
		{
			auto gv = mod.getGlobalVariable(SymbolName);
			if (gv == nullptr)
			{
				return gv;
			}
			return ConstantExpr::getPointerCast(gv, RTPartialApp::GetLLVMType()->getPointerTo());
		}
	}
}
