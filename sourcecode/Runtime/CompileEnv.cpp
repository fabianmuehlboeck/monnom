PUSHDIAGSUPPRESSION
#include "llvm/ADT/ArrayRef.h"
POPDIAGSUPPRESSION
#include "CompileEnv.h"
#include "ObjectHeader.h"
#include "NomMemberContext.h"
#include "NomType.h"
#include "NomClassType.h"
#include "NomMemberContext.h"
#include "ObjectHeader.h"
#include "CompileHelpers.h"
#include "TypeOperations.h"
#include "NomDynamicType.h"
#include "NomMethod.h"
#include "NomStaticMethod.h"
#include "NomLambda.h"
#include "TypeOperations.h"
#include "NomInterface.h"
#include "LambdaHeader.h"
#include "NomConstructor.h"
#include "NomRecord.h"
#include "NomRecordMethod.h"
#include "RecordHeader.h"
#include "NomTypeParameter.h"
#include "RTCompileConfig.h"
#include "PWObject.h"
#include "PWLambda.h"
#include "PWRecord.h"
#include "PWTypeArr.h"
#include "PWArr.h"
#include "PWType.h"
#include "PWDispatchPair.h"
#include "PWInt.h"

namespace Nom
{
	namespace Runtime
	{

		CompileEnv::CompileEnv(const llvm::Twine _contextName, llvm::Function* _function, const NomMemberContext* _context) : contextName(_contextName), Module(_function == nullptr ? nullptr : _function->getParent()), Function(_function), Context(_context) {}
#pragma region ACompileEnv
		ACompileEnv::ACompileEnv(const RegIndex _regcount, const llvm::Twine _contextName, llvm::Function* _function, const NomMemberContext* _context, const std::vector<PhiNode*>* _phiNodes, const llvm::ArrayRef<NomTypeParameterRef> _directTypeArgs, const llvm::ArrayRef<llvm::Value*> _typeArgValues) : CompileEnv(_contextName, _function, _context), regcount(_regcount), registers(new NomValue[static_cast<size_t>(_regcount)]), phiNodes(_phiNodes)
		{
			if (_directTypeArgs.size() != _typeArgValues.size())
			{
				throw new std::exception();
			}
			for (decltype(_directTypeArgs.size()) i = 0; i < _directTypeArgs.size(); i++)
			{
				TypeArguments.push_back(NomTypeVarValue(_typeArgValues[i], _directTypeArgs[i]->GetVariable()));
			}
		}

		ACompileEnv::ACompileEnv(const RegIndex _regcount, const llvm::Twine _contextName, llvm::Function* _function, [[maybe_unused]] int _argument_offset, const std::vector<PhiNode*>* _phiNodes, [[maybe_unused]] const llvm::ArrayRef<NomTypeParameterRef> _directTypeArgs, const NomMemberContext* _context, [[maybe_unused]] const TypeList _argtypes, [[maybe_unused]] NomTypeRef _thisType) : CompileEnv(_contextName, _function, _context), regcount(_regcount), registers(new NomValue[static_cast<size_t>(_regcount)]), phiNodes(_phiNodes)
		{
		}
		ACompileEnv::ACompileEnv(const RegIndex _regcount, const llvm::Twine _contextName, llvm::Function* _function, const std::vector<PhiNode*>* _phiNodes, const llvm::ArrayRef<NomTypeParameterRef> _directTypeArgs, const NomMemberContext* _context, const TypeList _argtypes, NomTypeRef _thisType)
			: ACompileEnv(_regcount, _contextName, _function, 0, _phiNodes, _directTypeArgs, _context, _argtypes, _thisType) {}

		ACompileEnv::~ACompileEnv()
		{
			delete[] registers;
		}



		NomValue ACompileEnv::GetArgument(size_t i)
		{
			return Arguments[i];
		}

		void ACompileEnv::PushArgument(NomValue arg)
		{
			Arguments.push_back(arg);
		}

		void ACompileEnv::PushDispatchPair(PWDispatchPair dpair)
		{
			dispatcherPairs.push(dpair);
		}

		PWDispatchPair ACompileEnv::PopDispatchPair()
		{
			auto ret = dispatcherPairs.top();
			dispatcherPairs.pop();
			return ret;
		}

		void ACompileEnv::ClearArguments()
		{
			Arguments.clear();
		}

		size_t ACompileEnv::GetArgCount()
		{
			return Arguments.size();
		}

		PhiNode* ACompileEnv::GetPhiNode(size_t index)
		{
			return (*phiNodes)[index];
		}

		size_t ACompileEnv::GetLocalTypeArgumentCount()
		{
			return TypeArguments.size();
		}

		PWTypeArr ACompileEnv::GetLocalTypeArgumentArray(NomBuilder& builder)
		{
			auto argc = GetLocalTypeArgumentCount();
			auto buf = PWTypeArr::Alloca(builder, PWInt32(argc, false), "localTypeArgs");
			localTypeArgArray = buf;
			for (decltype(argc) i = 0; i < argc; i++)
			{
				buf.ElemAt(builder, PWInt32(i,false)).Store(builder, TypeArguments[i]);
			}
			return buf;
		}

#pragma endregion
#pragma region AFullArityCompileEnv

		AFullArityCompileEnv::AFullArityCompileEnv(const RegIndex _regcount, const llvm::Twine _contextName, llvm::Function* _function, const NomMemberContext* _context, const std::vector<PhiNode*>* _phiNodes, const llvm::ArrayRef<NomTypeParameterRef> _directTypeArgs, const llvm::ArrayRef<llvm::Value*> _typeArgValues) : ACompileEnv(_regcount, _contextName, _function, _context, _phiNodes, _directTypeArgs, _typeArgValues)
		{
		}
		AFullArityCompileEnv::AFullArityCompileEnv(const RegIndex _regcount, const llvm::Twine _contextName, llvm::Function* _function, int _argument_offset, const std::vector<PhiNode*>* _phiNodes, const llvm::ArrayRef<NomTypeParameterRef> _directTypeArgs, const NomMemberContext* _context, const TypeList _argtypes, NomTypeRef _thisType) : ACompileEnv(_regcount, _contextName, _function, _argument_offset, _phiNodes, _directTypeArgs, _context, _argtypes, _thisType)
		{
			size_t argcount = 0;
			RegIndex argindex = 0;
			size_t typeArgCount = _directTypeArgs.size();
			for (auto& Arg : _function->args())
			{
				if (_argument_offset > 0)
				{
					_argument_offset--;
					continue;
				}
				if (argcount == 0 && _thisType != nullptr)
				{
					registers[argindex] = NomValue(&Arg, _thisType);
					argindex++;
				}
				else if (argcount + (_thisType != nullptr ? 0 : 1) <= typeArgCount)
				{
					TypeArguments.push_back(NomTypeVarValue(&Arg, _directTypeArgs[argcount - (_thisType != nullptr ? 1 : 0)]->GetVariable()));
				}
				else
				{
					registers[argindex] = NomValue(&Arg, _argtypes[static_cast<size_t>(argindex - (_thisType == nullptr ? 0 : 1))]);
					argindex++;
				}
				argcount++;
			}
			if (argcount != typeArgCount + _argtypes.size() + (_thisType == nullptr ? 0 : 1))
			{
				throw new std::exception();
			}
		}
		AFullArityCompileEnv::AFullArityCompileEnv(const RegIndex _regcount, const llvm::Twine _contextName, llvm::Function* _function, const std::vector<PhiNode*>* _phiNodes, const llvm::ArrayRef<NomTypeParameterRef> _directTypeArgs, const NomMemberContext* _context, const TypeList _argtypes, NomTypeRef _thisType) : AFullArityCompileEnv(_regcount, _contextName, _function, 0, _phiNodes, _directTypeArgs, _context, _argtypes, _thisType)
		{
		}
		AFullArityCompileEnv::~AFullArityCompileEnv()
		{
		}
#pragma endregion
#pragma region AVariableArityCompileEnv

		NomValue& AVariableArityCompileEnv::operator[](const RegIndex index)
		{
			return ACompileEnv::operator[](index);
		}

		AVariableArityCompileEnv::AVariableArityCompileEnv(const RegIndex _regcount, const llvm::Twine _contextName, llvm::Function* _function, const NomMemberContext* _context, const std::vector<PhiNode*>* _phiNodes, const llvm::ArrayRef<NomTypeParameterRef> _directTypeArgs, const llvm::ArrayRef<llvm::Value*> _typeArgValues) : ACompileEnv(_regcount, _contextName, _function, _context, _phiNodes, _directTypeArgs, _typeArgValues)
		{
		}
		AVariableArityCompileEnv::AVariableArityCompileEnv(const RegIndex _regcount, const llvm::Twine _contextName, llvm::Function* _function, int argument_offset, const std::vector<PhiNode*>* _phiNodes, const llvm::ArrayRef<NomTypeParameterRef> _directTypeArgs, const NomMemberContext* _context, const TypeList _argtypes, NomTypeRef _thisType) : ACompileEnv(_regcount, _contextName, _function, argument_offset, _phiNodes, _directTypeArgs, _context, _argtypes, _thisType)
		{
			auto argiter = _function->arg_begin();
			argiter++;
			registers[0] = NomValue(argiter, _thisType);
			argiter++;
			size_t argpos = 0;
			size_t valargpos = 0;
			for (decltype(RTConfig_NumberOfVarargsArguments) i = 0; i < RTConfig_NumberOfVarargsArguments; i++)
			{
				if (i < RTConfig_NumberOfVarargsArguments - 1 || _argtypes.size() + _directTypeArgs.size() <= RTConfig_NumberOfVarargsArguments)
				{
					if (argpos >= _directTypeArgs.size()&& valargpos<_argtypes.size())
					{
						registers[valargpos+1] = NomValue(argiter, _argtypes[valargpos]);
						valargpos++;
					}
					argiter++;
				}
				else
				{
					//TODO: implement
				}
				argpos++;
			}
		}
		AVariableArityCompileEnv::AVariableArityCompileEnv(const RegIndex _regcount, const llvm::Twine _contextName, llvm::Function* _function, const std::vector<PhiNode*>* _phiNodes, const llvm::ArrayRef<NomTypeParameterRef> _directTypeArgs, const NomMemberContext* _context, const TypeList _argtypes, NomTypeRef _thisType) : AVariableArityCompileEnv(_regcount, _contextName, _function, 0, _phiNodes, _directTypeArgs, _context, _argtypes, _thisType)
		{
		}
		AVariableArityCompileEnv::~AVariableArityCompileEnv()
		{
		}
#pragma endregion
#pragma region InstanceMethodCompileEnv
		InstanceMethodCompileEnv::InstanceMethodCompileEnv(RegIndex _regcount, const llvm::Twine _contextName, llvm::Function* _function, const std::vector<PhiNode*>* _phiNodes, const llvm::ArrayRef<NomTypeParameterRef> _directTypeArgs, const TypeList _argtypes, NomClassTypeRef _thisType, const NomMethod* _method) : AFullArityCompileEnv(_regcount, _contextName, _function, _phiNodes, _directTypeArgs, _method, _argtypes, _thisType), Method(_method)
		{
		}

		NomTypeVarValue InstanceMethodCompileEnv::GetTypeArgument(NomBuilder& builder, size_t i)
		{
			if (i < Method->GetTypeParametersStart())
			{
				return NomTypeVarValue(PWObject(registers[0]).ReadTypeArgument(builder, PWCInt32(i + Method->GetContainer()->GetTypeParametersStart(),false)), Context->GetTypeParameter(i)->GetVariable()); //needs to add container argument start because superclass variable instantiations may be different (e.g. B<T> extends A<Foo<T>>)
			}
			else
			{
				return TypeArguments[i - Method->GetTypeParametersStart()];
			}
		}
		size_t InstanceMethodCompileEnv::GetEnvTypeArgumentCount()
		{
			return Method->GetContainer()->GetTypeParametersCount();
		}
		PWTypeArr InstanceMethodCompileEnv::GetEnvTypeArgumentArray(NomBuilder& builder)
		{
			return PWObject(registers[0]).PointerToTypeArguments(builder).SubArr(builder, MakeInt32(Method->GetContainer()->GetTypeParametersStart())).WithSize(GetEnvTypeArgumentCount(), false);
		}
#pragma endregion

#pragma region StaticMethodCompileEnv
		StaticMethodCompileEnv::StaticMethodCompileEnv(RegIndex _regcount, const llvm::Twine _contextName, llvm::Function* _function, const std::vector<PhiNode*>* _phiNodes, const llvm::ArrayRef<NomTypeParameterRef> _directTypeArgs, const TypeList _argtypes, const NomStaticMethod* _method) : AFullArityCompileEnv(_regcount, _contextName, _function, _phiNodes, _directTypeArgs, _method, _argtypes, nullptr), Method(_method)
		{
		}

		NomTypeVarValue StaticMethodCompileEnv::GetTypeArgument([[maybe_unused]] NomBuilder& builder, size_t i)
		{
			return TypeArguments[static_cast<size_t>(i)];
		}
		size_t StaticMethodCompileEnv::GetEnvTypeArgumentCount()
		{
			return 0;
		}
		PWTypeArr StaticMethodCompileEnv::GetEnvTypeArgumentArray([[maybe_unused]] NomBuilder& builder)
		{
			return ConstantPointerNull::get(TYPETYPE->getPointerTo());
		}
#pragma endregion

#pragma region ConstructorCompileEnv
		ConstructorCompileEnv::ConstructorCompileEnv(RegIndex _regcount, const llvm::Twine _contextName, llvm::Function* _function, const std::vector<PhiNode*>* _phiNodes, const llvm::ArrayRef<NomTypeParameterRef> _directTypeArgs, const TypeList _argtypes, NomClassTypeRef _thisType, const NomConstructor* _method) : AFullArityCompileEnv(_regcount, _contextName, _function, _phiNodes, _directTypeArgs, _method, _argtypes, _thisType), Method(_method)
		{
		}

		NomTypeVarValue ConstructorCompileEnv::GetTypeArgument([[maybe_unused]] NomBuilder& builder, size_t i)
		{
			return TypeArguments[static_cast<size_t>(i)];
		}
		size_t ConstructorCompileEnv::GetEnvTypeArgumentCount()
		{
			return 0;
		}
		PWTypeArr ConstructorCompileEnv::GetEnvTypeArgumentArray([[maybe_unused]] NomBuilder& builder)
		{
			return ConstantPointerNull::get(TYPETYPE->getPointerTo());
		}
		bool ConstructorCompileEnv::GetInConstructor() { return inConstructor; }
		void ConstructorCompileEnv::SetPastInitialSetup() { inConstructor = false; }
#pragma endregion

#pragma region LambdaCompileEnv

		LambdaCompileEnv::LambdaCompileEnv(RegIndex _regcount, const llvm::Twine _contextName, llvm::Function* _function, const std::vector<PhiNode*>* _phiNodes, const llvm::ArrayRef<NomTypeParameterRef> _directTypeArgs, const TypeList _argtypes, const NomLambdaBody* _lambda) : AVariableArityCompileEnv(_regcount, _contextName, _function, _phiNodes, _directTypeArgs, _lambda, _argtypes, &NomDynamicType::LambdaInstance()), Lambda(_lambda)
		{
		}


		NomTypeVarValue LambdaCompileEnv::GetTypeArgument(NomBuilder& builder, size_t i)
		{
			if (i < Lambda->GetParent()->GetTypeParametersCount())
			{
				return NomTypeVarValue(PWLambdaPrecise(registers[0], this->Lambda->Parent).ReadTypeArgument(builder, i), Context->GetTypeParameter(i)->GetVariable());
			}
			else
			{
				return TypeArguments[i - Lambda->GetParent()->GetTypeParametersCount()];
			}
		}
		size_t LambdaCompileEnv::GetEnvTypeArgumentCount()
		{
			return Lambda->GetParent()->GetTypeParametersCount();
		}
		PWTypeArr LambdaCompileEnv::GetEnvTypeArgumentArray(NomBuilder& builder)
		{
			return PWLambda(registers[0]).PointerToTypeArguments(builder).WithSize(GetEnvTypeArgumentCount(), false);
		}

#pragma endregion
#pragma region NomRecord
		StructMethodCompileEnv::StructMethodCompileEnv(RegIndex _regcount, const llvm::Twine _contextName, llvm::Function* _function, const std::vector<PhiNode*>* _phiNodes, const llvm::ArrayRef<NomTypeParameterRef> _directTypeArgs, const TypeList _argtypes, const NomRecordMethod* _method) : AFullArityCompileEnv(_regcount, _contextName, _function, _phiNodes, _directTypeArgs, _method, _argtypes, &NomDynamicType::RecordInstance()), Method(_method)
		{
		}
		NomTypeVarValue StructMethodCompileEnv::GetTypeArgument(NomBuilder& builder, size_t i)
		{
			if (i < Method->Container->GetTypeParametersCount())
			{
				return NomTypeVarValue(PWRecord(registers[0]).ReadTypeArgument(builder, i), Context->GetTypeParameter(i)->GetVariable());
			}
			else
			{
				return TypeArguments[i - Method->Container->GetTypeParametersCount()];
			}
		}
		size_t StructMethodCompileEnv::GetEnvTypeArgumentCount()
		{
			return Method->Container->GetTypeParametersCount();
		}
		PWTypeArr StructMethodCompileEnv::GetEnvTypeArgumentArray(NomBuilder& builder)
		{
			return PWRecord(registers[0]).PointerToTypeArguments(builder).WithSize(GetEnvTypeArgumentCount(), false);
		}
#pragma endregion
#pragma region NomRecordInstantiation
		StructInstantiationCompileEnv::StructInstantiationCompileEnv(RegIndex _regcount, llvm::Function* _function, const llvm::ArrayRef<NomTypeParameterRef> _directTypeArgs, const TypeList _argtypes, const NomRecord* _structure, RegIndex _endargregcount) : AFullArityCompileEnv(_regcount, *_structure->GetSymbolName(), _function, nullptr, _directTypeArgs, _structure, _argtypes, nullptr), Record(_structure)
		{
			RegIndex endArgBarrier = static_cast<RegIndex>(_argtypes.size()) - _endargregcount;
			for (RegIndex i = static_cast<RegIndex>(_argtypes.size()); i > 0; i--)
			{
				if (i > endArgBarrier)
				{
					(*this)[_regcount - (i - static_cast<RegIndex>(_argtypes.size()) - 1)] = (*this)[i - 1];
				}
				else
				{
					(*this)[i] = (*this)[i - 1];
				}
			}
		}

		NomTypeVarValue StructInstantiationCompileEnv::GetTypeArgument(NomBuilder& builder, size_t i)
		{
			if (i < Context->GetTypeParametersCount())
			{
				return NomTypeVarValue(PWRecord(registers[0]).ReadTypeArgument(builder, i + Context->GetTypeParametersStart()), Context->GetTypeParameter(i)->GetVariable());
			}
			else
			{
				return TypeArguments[i - Context->GetTypeParametersStart()];
			}
		}
		size_t StructInstantiationCompileEnv::GetEnvTypeArgumentCount()
		{
			return Context->GetTypeParametersCount();
		}
		PWTypeArr StructInstantiationCompileEnv::GetEnvTypeArgumentArray(NomBuilder& builder)
		{
			return PWObject(registers[0]).PointerToTypeArguments(builder).SubArr(builder, MakeInt32(Context->GetTypeParametersStart()), "envTypeArr").WithSize(GetEnvTypeArgumentCount(), false);
		}
		bool StructInstantiationCompileEnv::GetInConstructor()
		{
			return inConstructor;
		}
		void StructInstantiationCompileEnv::SetPastInitialSetup()
		{
			inConstructor = false;
		}
#pragma endregion
#pragma region SimpleClassCompileEnv
		SimpleClassCompileEnv::SimpleClassCompileEnv(llvm::Function* _function, const NomMemberContext* _context, const llvm::ArrayRef<NomTypeParameterRef> _directTypeArgs, const TypeList _argtypes, NomTypeRef _thisType) : ACompileEnv(static_cast<RegIndex>(_function->getFunctionType()->getNumParams()) - static_cast<RegIndex>(_directTypeArgs.size()), *_context->GetSymbolName(), _function, nullptr, _directTypeArgs, _context, _argtypes, _thisType)
		{

		}

		NomTypeVarValue SimpleClassCompileEnv::GetTypeArgument(NomBuilder& builder, size_t i)
		{
			if (i < Context->GetTypeParametersCount())
			{
				return NomTypeVarValue(PWObject(registers[0]).ReadTypeArgument(builder, PWCInt32(i + (Context->GetTypeParametersStart()), false)), Context->GetTypeParameter(i)->GetVariable());
			}
			else
			{
				return TypeArguments[i - Context->GetTypeParametersStart()];
			}
		}
		size_t SimpleClassCompileEnv::GetEnvTypeArgumentCount()
		{
			return Context->GetTypeParametersCount();
		}
		PWTypeArr SimpleClassCompileEnv::GetEnvTypeArgumentArray(NomBuilder& builder)
		{
			return PWObject(registers[0]).PointerToTypeArguments(builder).SubArr(builder, MakeInt32(Context->GetTypeParametersStart())).WithSize(GetEnvTypeArgumentCount(), false);
		}
#pragma endregion
#pragma region CastedValueCompileEnv
		CastedValueCompileEnv::CastedValueCompileEnv(const llvm::ArrayRef<NomTypeParameterRef> _directTypeArgs, const llvm::ArrayRef<NomTypeParameterRef> _instanceTypeArgs, llvm::Function* _fun, size_t _regular_args_begin, size_t _funValueArgCount, llvm::Value* _instanceTypeArrPtr)
			: CompileEnv("", nullptr, nullptr), directTypeArgs(_directTypeArgs), instanceTypeArgs(_instanceTypeArgs), function(_fun), regularArgsBegin(_regular_args_begin), funValueArgCount(_funValueArgCount), instanceTypeArrPtr(_instanceTypeArrPtr)
		{
		}
		NomValue& CastedValueCompileEnv::operator[]([[maybe_unused]] const RegIndex index)
		{
			throw new std::exception();
		}
		NomTypeVarValue CastedValueCompileEnv::GetTypeArgument(NomBuilder& builder, size_t i)
		{
			if (i > instanceTypeArgs.size())
			{
				auto fargs = function->arg_begin();
				i -= (instanceTypeArgs.size());
				auto funargcount = (function->getFunctionType()->getNumParams()) - regularArgsBegin;
				if (i < funargcount - 1 || funargcount >= directTypeArgs.size() + funValueArgCount)
				{
					for (size_t j = 0; j < regularArgsBegin + i; j++)
					{
						fargs++;
					}
					return NomTypeVarValue(fargs, new NomTypeVar(directTypeArgs[i - instanceTypeArgs.size()]));
				}
				else
				{
					for (unsigned int j = 0; j < function->getFunctionType()->getNumParams() - 1; j++)
					{
						fargs++;
					}
					return NomTypeVarValue(MakeInvariantLoad(builder, TYPETYPE, fargs, MakeInt32(i - (funargcount - 1))), new NomTypeVar(directTypeArgs[i - instanceTypeArgs.size()]));
				}
			}
			else
			{
				return NomTypeVarValue(MakeInvariantLoad(builder, TYPETYPE, builder->CreateGEP(NLLVMPointer(TYPETYPE), instanceTypeArrPtr, llvm::ArrayRef<llvm::Value*>({ MakeInt32(-(i + 1))}))), new NomTypeVar(instanceTypeArgs[i]));
			}
		}
		NomValue CastedValueCompileEnv::GetArgument([[maybe_unused]] size_t i)
		{
			throw new std::exception();
		}
		void CastedValueCompileEnv::PushArgument([[maybe_unused]] NomValue arg)
		{
			throw new std::exception();
		}
		void CastedValueCompileEnv::ClearArguments()
		{
			throw new std::exception();
		}
		size_t CastedValueCompileEnv::GetArgCount()
		{
			throw new std::exception();
		}
		PhiNode* CastedValueCompileEnv::GetPhiNode([[maybe_unused]] size_t index)
		{
			throw new std::exception();
		}
		size_t CastedValueCompileEnv::GetLocalTypeArgumentCount()
		{
			return directTypeArgs.size();
		}
		size_t CastedValueCompileEnv::GetEnvTypeArgumentCount()
		{
			return instanceTypeArgs.size();
		}
		PWTypeArr CastedValueCompileEnv::GetLocalTypeArgumentArray([[maybe_unused]] NomBuilder& builder)
		{
			throw new std::exception();
		}
		PWTypeArr CastedValueCompileEnv::GetEnvTypeArgumentArray([[maybe_unused]] NomBuilder& builder)
		{
			return PWTypeArr(instanceTypeArrPtr, PWInt32(instanceTypeArgs.size(), false));
		}
		void CastedValueCompileEnv::PushDispatchPair([[maybe_unused]] PWDispatchPair dpair)
		{
			throw new std::exception();
		}
		PWDispatchPair CastedValueCompileEnv::PopDispatchPair()
		{
			throw new std::exception();
		}
#pragma endregion
		CastedValueCompileEnvIndirect::CastedValueCompileEnvIndirect(const llvm::ArrayRef<NomTypeParameterRef> _directTypeArgs, const llvm::ArrayRef<NomTypeParameterRef> _instanceTypeArgs, llvm::Function* _fun, size_t _regular_args_begin, size_t _funValueArgCount, llvm::Value* _instanceTypeArrPtr)
			: CastedValueCompileEnv(_directTypeArgs, _instanceTypeArgs, _fun, _regular_args_begin, _funValueArgCount, _instanceTypeArrPtr)
		{
		}
		NomTypeVarValue CastedValueCompileEnvIndirect::GetTypeArgument(NomBuilder& builder, size_t i)
		{
			if (i > instanceTypeArgs.size())
			{
				auto fargs = function->arg_begin();
				i -= instanceTypeArgs.size();
				auto funargcount = function->getFunctionType()->getNumParams() - regularArgsBegin;
				if (i < funargcount - 1 || funargcount >= directTypeArgs.size() + funValueArgCount)
				{
					for (size_t j = 0; j < regularArgsBegin + i; j++)
					{
						fargs++;
					}
					return NomTypeVarValue(fargs, new NomTypeVar(directTypeArgs[i - instanceTypeArgs.size()]));
				}
				else
				{
					for (unsigned int j = 0; j < function->getFunctionType()->getNumParams() - 1; j++)
					{
						fargs++;
					}
					return NomTypeVarValue(MakeInvariantLoad(builder, TYPETYPE, fargs, MakeInt32(i - (funargcount - 1))), new NomTypeVar(directTypeArgs[i - instanceTypeArgs.size()]));
				}
			}
			else
			{
				return NomTypeVarValue(MakeInvariantLoad(builder, TYPETYPE, builder->CreateGEP(arrtype(TYPETYPE, 0), MakeInvariantLoad(builder, NLLVMPointer(arrtype(TYPETYPE, 0)), instanceTypeArrPtr), { MakeInt32(0), MakeInt32(-(i + 1)) })), new NomTypeVar(instanceTypeArgs[i]));
			}
		}
		PWTypeArr CastedValueCompileEnvIndirect::GetEnvTypeArgumentArray(NomBuilder& builder)
		{
			return PWTypeArr(MakeInvariantLoad(builder, NLLVMPointer(TYPETYPE), instanceTypeArrPtr), PWInt32(GetEnvTypeArgumentCount(), false));
		}
	}
}
