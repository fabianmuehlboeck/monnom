#include "../Common/CompileEnv.h"
#include "../Runtime_Data/Headers/ObjectHeader.h"
#include "../Intermediate_Representation/NomMemberContext.h"
#include "../Intermediate_Representation/Types/NomType.h"
#include "llvm/ADT/ArrayRef.h"
#include "../Intermediate_Representation/Types/NomClassType.h"
#include "../Intermediate_Representation/NomMemberContext.h"
#include "../Runtime_Data/Headers/ObjectHeader.h"
#include "../Runtime_Data/Headers/RefValueHeader.h"
#include "../Common/CompileHelpers.h"
#include "../Common/TypeOperations.h"
#include "../Intermediate_Representation/Types/NomDynamicType.h"
#include "../Intermediate_Representation/Callables/NomMethod.h"
#include "../Intermediate_Representation/Callables/NomStaticMethod.h"
#include "../Intermediate_Representation/Data/NomLambda.h"
#include "../Common/TypeOperations.h"
#include "../Intermediate_Representation/Data/NomInterface.h"
#include "../Runtime_Data/Headers/LambdaHeader.h"
#include "../Intermediate_Representation/Callables/NomConstructor.h"
#include "../Intermediate_Representation/Data/NomRecord.h"
#include "../Intermediate_Representation/Callables/NomRecordMethod.h"
#include "../Runtime_Data/Headers/RecordHeader.h"
#include "../Intermediate_Representation/NomTypeParameter.h"
#include "../Runtime/RTCompileConfig.h"
#include "../Runtime/Config/RTConfig.h"
#include "../Runtime/Statistics/CastStats.h"

namespace Nom
{
	namespace Runtime
	{

		CompileEnv::CompileEnv(const llvm::Twine contextName, llvm::Function* function, const NomMemberContext* context) : contextName(contextName), Module(function == nullptr ? nullptr : function->getParent()), Function(function), Context(context) {}
#pragma region ACompileEnv
		ACompileEnv::ACompileEnv(const RegIndex regcount, const llvm::Twine contextName, llvm::Function* function, const NomMemberContext* context, const std::vector<PhiNode*>* phiNodes, const llvm::ArrayRef<NomTypeParameterRef> directTypeArgs, const llvm::ArrayRef<llvm::Value*> typeArgValues) : CompileEnv(contextName, function, context), regcount(regcount), registers(new NomValue[regcount]), phiNodes(phiNodes)
		{
			if (directTypeArgs.size() != typeArgValues.size())
			{
				throw new std::exception();
			}
			for (decltype(directTypeArgs.size()) i = 0; i < directTypeArgs.size(); i++)
			{
				TypeArguments.push_back(NomTypeVarValue(typeArgValues[i], directTypeArgs[i]->GetVariable()));
			}
		}

		ACompileEnv::ACompileEnv(const RegIndex regcount, const llvm::Twine contextName, llvm::Function* function, int argument_offset, const std::vector<PhiNode*>* phiNodes, const llvm::ArrayRef<NomTypeParameterRef> directTypeArgs, const NomMemberContext* context, const TypeList argtypes, NomTypeRef thisType) : CompileEnv(contextName, function, context), regcount(regcount), registers(new NomValue[regcount]), phiNodes(phiNodes)
		{
		}
		ACompileEnv::ACompileEnv(const RegIndex regcount, const llvm::Twine contextName, llvm::Function* function, const std::vector<PhiNode*>* phiNodes, const llvm::ArrayRef<NomTypeParameterRef> directTypeArgs, const NomMemberContext* context, const TypeList argtypes, NomTypeRef thisType)
			: ACompileEnv(regcount, contextName, function, 0, phiNodes, directTypeArgs, context, argtypes, thisType) {}

		ACompileEnv::~ACompileEnv()
		{
			delete[] registers;
		}


		NomValue& ACompileEnv::operator[] (const RegIndex index)
		{
			if (index < 0 || index >= regcount)
			{
				throw "Invalid Register index!";
			}

			auto value = registers[index];
			llvm::Value *tag, *tagVal;
			value.TagAndVal(tag, tagVal);
			// Need to handle both a value being defined, and also the register being undefined.
			if (*value != nullptr || (tag == nullptr && *value == nullptr)) return registers[index];
			if (builder == nullptr) throw "Builder needed to lookup NomValue";
			auto newValue = Runtime::PackFromTag(*builder, tagVal, value.GetNomType(), tag, value.GetPackBlock());
			registers[index].MakePacked(newValue);
			return registers[index];
		}
		llvm::Value* ACompileEnv::LookupUnwrapped(const RegIndex index, llvm::Value** outTag, NomTypeRef* type)
		{
			if (index < 0 || index >= regcount)
			{
				throw "Invalid Register index!";
			}
			auto value = registers[index];
			llvm::Value *tag, *tagVal;
			value.TagAndVal(tag, tagVal);
			*type = value.GetNomType();
			if (tag != nullptr) {
				*outTag = tag;
				return tagVal;
			}
			#define BUILDER ((*builder))

			auto oldInsertPoint = BUILDER->GetInsertBlock();
			llvm::Function* fun = BUILDER->GetInsertBlock()->getParent();
			

			llvm::BasicBlock::iterator splitPoint;
			BasicBlock* splitBlock;

			const char * splitBlockName;
			if (auto *inst = dyn_cast<llvm::Instruction>(*value) ) {
				splitBlock = inst->getParent();
				do {inst = inst->getNextNode();} while (inst && isa<llvm::PHINode>(inst));
				if (inst) {
					splitPoint = inst->getIterator();
					splitBlockName = "unwrapOutBlockAfterInst";
				} else {
					splitPoint = splitBlock->end();
					splitBlockName = "unwrapOutBlockEndofBlock";
				}
			} else if (isa<llvm::Argument>(*value) || isa<llvm::Constant>(*value)) {
				splitBlock = &fun->getEntryBlock();
				splitPoint = splitBlock->getFirstInsertionPt();
				splitBlockName = "unwrapOutBlockArgumentOrConstant";
			} else {
				throw "Invalid value type!";
			}


			// We need to split the basic block by splicing everything after the current insert point into the new block
			BasicBlock* outBlock;
			if (splitBlock->getTerminator()) {
				outBlock = splitBlock->splitBasicBlock(splitPoint, splitBlockName);
				auto addedBr = splitBlock->getTerminator();
				addedBr->dropAllReferences();
				addedBr->removeFromParent();
			} else {
				outBlock = BasicBlock::Create(LLVMCONTEXT, splitBlockName, fun);
				outBlock->getInstList().splice(outBlock->begin(), splitBlock->getInstList(), splitPoint, splitBlock->end());
			}

			BUILDER->SetInsertPoint(splitBlock);


			BasicBlock* refValueBlock = nullptr, * primitiveIntBlock = nullptr, * 
				primitiveFloatBlock = nullptr, * primitiveBoolBlock = nullptr;
			Value* intValue = nullptr, * floatValue = nullptr, * boolValue = nullptr;

			auto cases = RefValueHeader::GenerateRefOrPrimitiveValueSwitch(BUILDER, value, 
				&refValueBlock, nullptr, nullptr, true, &primitiveIntBlock, &intValue, 
				&primitiveFloatBlock, &floatValue, &primitiveBoolBlock, &boolValue);


			// BasicBlock* outBlock = BasicBlock::Create(LLVMCONTEXT, "unpackOut", fun);
			PHINode* outValuePHI = nullptr;
			llvm::Value* outValue = nullptr;
			PHINode* outTagPHI = nullptr;
			llvm::Type* unpackedTy = llvm::Type::getInt64Ty(LLVMCONTEXT);

			if (cases > 1)
			{
				BUILDER->SetInsertPoint(outBlock, outBlock->begin());

				outValuePHI = BUILDER->CreatePHI(unpackedTy, cases, "unpackedValue");
				outTagPHI = BUILDER->CreatePHI(llvm::Type::getIntNTy(LLVMCONTEXT, 2), cases, "unpackedTag");
				outValue = outValuePHI;
			}

			if (primitiveIntBlock != nullptr)
			{
				BUILDER->SetInsertPoint(primitiveIntBlock);
				if (outValuePHI == nullptr) {
					outValue = BUILDER->CreateBitCast(intValue, unpackedTy, "unpackedInt");
					*outTag = MakeUInt(2, 3);
				} else {
					// TODO: Check if this is actually sufficient! I think it is though since in RefValueHeader if the type is known statically
					// Then the correct call is generated
					if (NomCastStats) {
                        BUILDER->CreateCall(GetIncIntUnpacksFunction(*BUILDER->GetInsertBlock()->getParent()->getParent()), {});
					}
					auto casted = BUILDER->CreateBitCast(intValue, unpackedTy, "unpackedInt");
					outValuePHI->addIncoming(casted, primitiveIntBlock);
					outTagPHI->addIncoming(MakeUInt(2,3), primitiveIntBlock);
				}
				BUILDER->CreateBr(outBlock);
			}

			if (primitiveFloatBlock != nullptr)
			{
				BUILDER->SetInsertPoint(primitiveFloatBlock);
				if (outValuePHI == nullptr) {
					outValue = BUILDER->CreateBitCast(floatValue, unpackedTy, "unpackedFloat");
					*outTag = MakeUInt(2, 1);
				} else {
					if (NomCastStats) {
                        BUILDER->CreateCall(GetIncFloatUnpacksFunction(*BUILDER->GetInsertBlock()->getParent()->getParent()), {});
					}
					auto casted = BUILDER->CreateBitCast(floatValue, unpackedTy, "unpackedFloat");
					outValuePHI->addIncoming(casted, primitiveFloatBlock);
					outTagPHI->addIncoming(MakeUInt(2,1), primitiveFloatBlock);
				}
				BUILDER->CreateBr(outBlock);
			}

			if (refValueBlock != nullptr)
			{
				BUILDER->SetInsertPoint(refValueBlock);
				if (outValuePHI == nullptr) {
					outValue = BUILDER->CreatePtrToInt(value, unpackedTy, "unpackedRef");
					*outTag = MakeUInt(2, 0);
				} else {
					auto casted = BUILDER->CreatePtrToInt(value, unpackedTy, "unpackedRef");
					outValuePHI->addIncoming(casted, refValueBlock);
					outTagPHI->addIncoming(MakeUInt(2,1), refValueBlock);
				}
				BUILDER->CreateBr(outBlock);
			}

			if (primitiveBoolBlock != nullptr)
			{
				BUILDER->SetInsertPoint(primitiveBoolBlock);
				if (outValuePHI == nullptr) {
					outValue = BUILDER->CreateZExt(boolValue, unpackedTy, "unpackedBool");
					*outTag = MakeUInt(2, 0);
				} else {
					auto casted = BUILDER->CreateZExt(boolValue, unpackedTy, "unpackedBool");
					outValuePHI->addIncoming(casted, primitiveBoolBlock);
					outTagPHI->addIncoming(MakeUInt(2,1), primitiveBoolBlock);
				}
				BUILDER->CreateBr(outBlock);
			}

			if (outBlock->getTerminator()) {
				BUILDER->SetInsertPoint(oldInsertPoint);
			} else {
				BUILDER->SetInsertPoint(outBlock);
			}
			if (outValuePHI != nullptr) {
				*outTag = outTagPHI;
			}

			registers[index].MakeUnpacked(outValue, *outTag);
			return outValue;
			#undef BUILDER

			// TODO fix to unwrap if wrapped
		}

		NomValue ACompileEnv::GetArgument(int i)
		{
			return Arguments[i];
		}

		void ACompileEnv::PushArgument(NomValue arg)
		{
			Arguments.push_back(arg);
		}

		void ACompileEnv::PushDispatchPair(llvm::Value* dpair)
		{
			dispatcherPairs.push(dpair);
		}

		llvm::Value* ACompileEnv::PopDispatchPair()
		{
			auto ret = dispatcherPairs.top();
			dispatcherPairs.pop();
			return ret;
		}

		void ACompileEnv::ClearArguments()
		{
			Arguments.clear();
		}

		int ACompileEnv::GetArgCount()
		{
			return Arguments.size();
		}

		PhiNode* ACompileEnv::GetPhiNode(int index)
		{
			return (*phiNodes)[index];
		}

		size_t ACompileEnv::GetLocalTypeArgumentCount()
		{
			return TypeArguments.size();
		}

		llvm::Value* ACompileEnv::GetLocalTypeArgumentArray(NomBuilder& builder)
		{
			//if (localTypeArgArray == nullptr)
			//{
			auto argc = GetLocalTypeArgumentCount();
			localTypeArgArray = builder->CreateAlloca(TYPETYPE, MakeInt(argc));
			for (decltype(argc) i = 0; i < argc; i++)
			{
				MakeStore(builder, TypeArguments[i], builder->CreateGEP(localTypeArgArray, MakeInt32((int32_t)(argc - (i + 1)))));
			}
			//}
			return builder->CreateGEP(localTypeArgArray, MakeInt32(argc));
		}

#pragma endregion
#pragma region AFullArityCompileEnv

		AFullArityCompileEnv::AFullArityCompileEnv(const RegIndex regcount, const llvm::Twine contextName, llvm::Function* function, const NomMemberContext* context, const std::vector<PhiNode*>* phiNodes, const llvm::ArrayRef<NomTypeParameterRef> directTypeArgs, const llvm::ArrayRef<llvm::Value*> typeArgValues) : ACompileEnv(regcount, contextName, function, context, phiNodes, directTypeArgs, typeArgValues)
		{
		}
		AFullArityCompileEnv::AFullArityCompileEnv(const RegIndex regcount, const llvm::Twine contextName, llvm::Function* function, int argument_offset, const std::vector<PhiNode*>* phiNodes, const llvm::ArrayRef<NomTypeParameterRef> directTypeArgs, const NomMemberContext* context, const TypeList argtypes, NomTypeRef thisType) : ACompileEnv(regcount, contextName, function, argument_offset, phiNodes, directTypeArgs, context, argtypes, thisType)
		{
			size_t argcount = 0;
			RegIndex argindex = 0;
			size_t typeArgCount = directTypeArgs.size();
			for (auto& Arg : function->args())
			{
				if (argument_offset > 0)
				{
					argument_offset--;
					continue;
				}
				if (argcount == 0 && thisType != nullptr)
				{
					registers[argindex] = NomValue(&Arg, thisType);
					argindex++;
				}
				else if (argcount + (thisType != nullptr ? 0 : 1) <= typeArgCount)
				{
					TypeArguments.push_back(NomTypeVarValue(&Arg, directTypeArgs[argcount - (thisType != nullptr ? 1 : 0)]->GetVariable()));
				}
				else
				{
					registers[argindex] = NomValue(&Arg, argtypes[argindex - (thisType == nullptr ? 0 : 1)]);
					argindex++;
				}
				argcount++;
			}
			if (argcount != typeArgCount + argtypes.size() + (thisType == nullptr ? 0 : 1))
			{
				throw new std::exception();
			}
		}
		AFullArityCompileEnv::AFullArityCompileEnv(const RegIndex regcount, const llvm::Twine contextName, llvm::Function* function, const std::vector<PhiNode*>* phiNodes, const llvm::ArrayRef<NomTypeParameterRef> directTypeArgs, const NomMemberContext* context, const TypeList argtypes, NomTypeRef thisType) : AFullArityCompileEnv(regcount, contextName, function, 0, phiNodes, directTypeArgs, context, argtypes, thisType)
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
		llvm::Value* AVariableArityCompileEnv::LookupUnwrapped(const RegIndex index, llvm::Value** tag, NomTypeRef* type)
		{
			return ACompileEnv::LookupUnwrapped(index, tag, type);
		}

		AVariableArityCompileEnv::AVariableArityCompileEnv(const RegIndex regcount, const llvm::Twine contextName, llvm::Function* function, const NomMemberContext* context, const std::vector<PhiNode*>* phiNodes, const llvm::ArrayRef<NomTypeParameterRef> directTypeArgs, const llvm::ArrayRef<llvm::Value*> typeArgValues) : ACompileEnv(regcount, contextName, function, context, phiNodes, directTypeArgs, typeArgValues)
		{
		}
		AVariableArityCompileEnv::AVariableArityCompileEnv(const RegIndex regcount, const llvm::Twine contextName, llvm::Function* function, int argument_offset, const std::vector<PhiNode*>* phiNodes, const llvm::ArrayRef<NomTypeParameterRef> directTypeArgs, const NomMemberContext* context, const TypeList argtypes, NomTypeRef thisType) : ACompileEnv(regcount, contextName, function, argument_offset, phiNodes, directTypeArgs, context, argtypes, thisType)
		{
			auto argiter = function->arg_begin();
			argiter++;
			registers[0] = NomValue(argiter, thisType);
			argiter++;
			auto argpos = 0;
			auto valargpos = 0;
			for (decltype(RTConfig_NumberOfVarargsArguments) i = 0; i < RTConfig_NumberOfVarargsArguments; i++)
			{
				if (i < RTConfig_NumberOfVarargsArguments - 1 || argtypes.size() + directTypeArgs.size() <= RTConfig_NumberOfVarargsArguments)
				{
					if (argpos >= directTypeArgs.size()&& valargpos<argtypes.size())
					{
						registers[valargpos+1] = NomValue(argiter, argtypes[valargpos]);
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
		AVariableArityCompileEnv::AVariableArityCompileEnv(const RegIndex regcount, const llvm::Twine contextName, llvm::Function* function, const std::vector<PhiNode*>* phiNodes, const llvm::ArrayRef<NomTypeParameterRef> directTypeArgs, const NomMemberContext* context, const TypeList argtypes, NomTypeRef thisType) : AVariableArityCompileEnv(regcount, contextName, function, 0, phiNodes, directTypeArgs, context, argtypes, thisType)
		{
		}
		AVariableArityCompileEnv::~AVariableArityCompileEnv()
		{
		}
#pragma endregion
#pragma region InstanceMethodCompileEnv
		InstanceMethodCompileEnv::InstanceMethodCompileEnv(RegIndex regcount, const llvm::Twine contextName, llvm::Function* function, const std::vector<PhiNode*>* phiNodes, const llvm::ArrayRef<NomTypeParameterRef> directTypeArgs, const TypeList argtypes, NomClassTypeRef thisType, const NomMethod* method) : AFullArityCompileEnv(regcount, contextName, function, phiNodes, directTypeArgs, method, argtypes, thisType), Method(method)
		{
		}

		NomTypeVarValue InstanceMethodCompileEnv::GetTypeArgument(NomBuilder& builder, int i)
		{
			if ((size_t)i < Method->GetTypeParametersStart())
			{
				return NomTypeVarValue(ObjectHeader::GenerateReadTypeArgument(builder, registers[0], i + Method->GetContainer()->GetTypeParametersStart()), Context->GetTypeParameter(i)->GetVariable()); //needs to add container argument start because superclass variable instantiations may be different (e.g. B<T> extends A<Foo<T>>)
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
		llvm::Value* InstanceMethodCompileEnv::GetEnvTypeArgumentArray(NomBuilder& builder)
		{
			return builder->CreateGEP(ObjectHeader::GeneratePointerToTypeArguments(builder, registers[0]), MakeInt32(-(uint32_t)Method->GetContainer()->GetTypeParametersStart()));
		}
#pragma endregion

#pragma region StaticMethodCompileEnv
		StaticMethodCompileEnv::StaticMethodCompileEnv(RegIndex regcount, const llvm::Twine contextName, llvm::Function* function, const std::vector<PhiNode*>* phiNodes, const llvm::ArrayRef<NomTypeParameterRef> directTypeArgs, const TypeList argtypes, const NomStaticMethod* method) : AFullArityCompileEnv(regcount, contextName, function, phiNodes, directTypeArgs, method, argtypes, nullptr), Method(method)
		{
		}

		NomTypeVarValue StaticMethodCompileEnv::GetTypeArgument(NomBuilder& builder, int i)
		{
			return TypeArguments[i];
		}
		size_t StaticMethodCompileEnv::GetEnvTypeArgumentCount()
		{
			return 0;
		}
		llvm::Value* StaticMethodCompileEnv::GetEnvTypeArgumentArray(NomBuilder& builder)
		{
			return ConstantPointerNull::get(TYPETYPE->getPointerTo());
		}
#pragma endregion

#pragma region ConstructorCompileEnv
		ConstructorCompileEnv::ConstructorCompileEnv(RegIndex regcount, const llvm::Twine contextName, llvm::Function* function, const std::vector<PhiNode*>* phiNodes, const llvm::ArrayRef<NomTypeParameterRef> directTypeArgs, const TypeList argtypes, NomClassTypeRef thisType, const NomConstructor* method) : AFullArityCompileEnv(regcount, contextName, function, phiNodes, directTypeArgs, method, argtypes, thisType), Method(method)
		{
		}

		NomTypeVarValue ConstructorCompileEnv::GetTypeArgument(NomBuilder& builder, int i)
		{
			return TypeArguments[i];
		}
		size_t ConstructorCompileEnv::GetEnvTypeArgumentCount()
		{
			return 0;
		}
		llvm::Value* ConstructorCompileEnv::GetEnvTypeArgumentArray(NomBuilder& builder)
		{
			return ConstantPointerNull::get(TYPETYPE->getPointerTo());
		}
		bool ConstructorCompileEnv::GetInConstructor() { return inConstructor; }
		void ConstructorCompileEnv::SetPastInitialSetup() { inConstructor = false; }
#pragma endregion

#pragma region LambdaCompileEnv

		LambdaCompileEnv::LambdaCompileEnv(RegIndex regcount, const llvm::Twine contextName, llvm::Function* function, const std::vector<PhiNode*>* phiNodes, const llvm::ArrayRef<NomTypeParameterRef> directTypeArgs, const TypeList argtypes, const NomLambdaBody* lambda) : AVariableArityCompileEnv(regcount, contextName, function, phiNodes, directTypeArgs, lambda, argtypes, &NomDynamicType::LambdaInstance()), Lambda(lambda)
		{
		}


		NomTypeVarValue LambdaCompileEnv::GetTypeArgument(NomBuilder& builder, int i)
		{
			if ((size_t)i < Lambda->GetParent()->GetTypeParametersCount())
			{
				return NomTypeVarValue(LambdaHeader::GenerateReadTypeArgument(builder, registers[0], i, this->Lambda->Parent), Context->GetTypeParameter(i)->GetVariable());
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
		llvm::Value* LambdaCompileEnv::GetEnvTypeArgumentArray(NomBuilder& builder)
		{
			return LambdaHeader::GeneratePointerToTypeArguments(builder, registers[0], this->Lambda->Parent);
		}

#pragma endregion
#pragma region NomRecord
		StructMethodCompileEnv::StructMethodCompileEnv(RegIndex regcount, const llvm::Twine contextName, llvm::Function* function, const std::vector<PhiNode*>* phiNodes, const llvm::ArrayRef<NomTypeParameterRef> directTypeArgs, const TypeList argtypes, const NomRecordMethod* method) : AFullArityCompileEnv(regcount, contextName, function, phiNodes, directTypeArgs, method, argtypes, &NomDynamicType::RecordInstance()), Method(method)
		{
		}
		NomTypeVarValue StructMethodCompileEnv::GetTypeArgument(NomBuilder& builder, int i)
		{
			if ((size_t)i < Method->Container->GetTypeParametersCount())
			{
				return NomTypeVarValue(RecordHeader::GenerateReadTypeArgument(builder, registers[0], i, this->Method->Container), Context->GetTypeParameter(i)->GetVariable());
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
		llvm::Value* StructMethodCompileEnv::GetEnvTypeArgumentArray(NomBuilder& builder)
		{
			return RecordHeader::GeneratePointerToTypeArguments(builder, registers[0], this->Method->Container);
		}
#pragma endregion
#pragma region NomRecordInstantiation
		StructInstantiationCompileEnv::StructInstantiationCompileEnv(RegIndex regcount, llvm::Function* function, const llvm::ArrayRef<NomTypeParameterRef> directTypeArgs, const TypeList argtypes, const NomRecord* structure, RegIndex endargregcount) : AFullArityCompileEnv(regcount, *structure->GetSymbolName(), function, nullptr, directTypeArgs, structure, argtypes, nullptr), Record(structure)
		{
			int endArgBarrier = argtypes.size() - endargregcount;
			for (int i = argtypes.size(); i > 0; i--)
			{
				if (i > endArgBarrier)
				{
					(*this)[regcount - (i - argtypes.size() - 1)] = (*this)[i - 1];
				}
				else
				{
					(*this)[i] = (*this)[i - 1];
				}
			}
		}

		NomTypeVarValue StructInstantiationCompileEnv::GetTypeArgument(NomBuilder& builder, int i)
		{
			if ((size_t)i < Context->GetTypeParametersCount())
			{
				return NomTypeVarValue(ObjectHeader::GenerateReadTypeArgument(builder, registers[0], i + Context->GetTypeParametersStart()), Context->GetTypeParameter(i)->GetVariable());
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
		llvm::Value* StructInstantiationCompileEnv::GetEnvTypeArgumentArray(NomBuilder& builder)
		{
			return builder->CreateGEP(ObjectHeader::GeneratePointerToTypeArguments(builder, registers[0]), MakeInt32(-(uint32_t)Context->GetTypeParametersStart()));
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
		SimpleClassCompileEnv::SimpleClassCompileEnv(llvm::Function* function, const NomMemberContext* context, const llvm::ArrayRef<NomTypeParameterRef> directTypeArgs, const TypeList argtypes, NomTypeRef thisType) : ACompileEnv(function->getFunctionType()->getNumParams() - directTypeArgs.size(), *context->GetSymbolName(), function, nullptr, directTypeArgs, context, argtypes, thisType)
		{

		}

		NomTypeVarValue SimpleClassCompileEnv::GetTypeArgument(NomBuilder& builder, int i)
		{
			if ((size_t)i < Context->GetTypeParametersCount())
			{
				return NomTypeVarValue(ObjectHeader::GenerateReadTypeArgument(builder, registers[0], i + Context->GetTypeParametersStart()), Context->GetTypeParameter(i)->GetVariable());
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
		llvm::Value* SimpleClassCompileEnv::GetEnvTypeArgumentArray(NomBuilder& builder)
		{
			return builder->CreateGEP(ObjectHeader::GeneratePointerToTypeArguments(builder, registers[0]), MakeInt32(-(uint32_t)Context->GetTypeParametersStart()));
		}
#pragma endregion
#pragma region CastedValueCompileEnv
		CastedValueCompileEnv::CastedValueCompileEnv(const llvm::ArrayRef<NomTypeParameterRef> directTypeArgs, const llvm::ArrayRef<NomTypeParameterRef> instanceTypeArgs, llvm::Function* fun, int regular_args_begin, int funValueArgCount, llvm::Value* instanceTypeArrPtr)
			: CompileEnv("", nullptr, nullptr), directTypeArgs(directTypeArgs), instanceTypeArgs(instanceTypeArgs), function(fun), regularArgsBegin(regular_args_begin), funValueArgCount(funValueArgCount), instanceTypeArrPtr(instanceTypeArrPtr)
		{
		}
		NomValue& CastedValueCompileEnv::operator[](const RegIndex index)
		{
			throw new std::exception();
		}
		llvm::Value* CastedValueCompileEnv::LookupUnwrapped(const RegIndex index, llvm::Value** tag, NomTypeRef* type)
		{
			throw new std::exception();
		}
		NomTypeVarValue CastedValueCompileEnv::GetTypeArgument(NomBuilder& builder, int i)
		{
			if (i > instanceTypeArgs.size())
			{
				auto fargs = function->arg_begin();
				i -= instanceTypeArgs.size();
				auto funargcount = function->getFunctionType()->getNumParams() - regularArgsBegin;
				if (i < funargcount - 1 || funargcount >= directTypeArgs.size() + funValueArgCount)
				{
					for (int j = 0; j < regularArgsBegin + i; j++)
					{
						fargs++;
					}
					return NomTypeVarValue(fargs, new NomTypeVar(directTypeArgs[i - instanceTypeArgs.size()]));
				}
				else
				{
					for (int j = 0; j < function->getFunctionType()->getNumParams() - 1; j++)
					{
						fargs++;
					}
					return NomTypeVarValue(MakeInvariantLoad(builder, builder->CreatePointerCast(fargs, TYPETYPE), MakeInt32(i - (funargcount - 1))), new NomTypeVar(directTypeArgs[i - instanceTypeArgs.size()]));
				}
			}
			else
			{
				return NomTypeVarValue(MakeInvariantLoad(builder, builder->CreateGEP(instanceTypeArrPtr->getType() == TYPETYPE->getPointerTo()->getPointerTo() ? MakeInvariantLoad(builder, instanceTypeArrPtr) : instanceTypeArrPtr, MakeInt32(-(i + 1)))), new NomTypeVar(instanceTypeArgs[i]));
			}
		}
		NomValue CastedValueCompileEnv::GetArgument(int i)
		{
			throw new std::exception();
		}
		void CastedValueCompileEnv::PushArgument(NomValue arg)
		{
			throw new std::exception();
		}
		void CastedValueCompileEnv::ClearArguments()
		{
			throw new std::exception();
		}
		int CastedValueCompileEnv::GetArgCount()
		{
			throw new std::exception();
		}
		PhiNode* CastedValueCompileEnv::GetPhiNode(int index)
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
		llvm::Value* CastedValueCompileEnv::GetLocalTypeArgumentArray(NomBuilder& builder)
		{
			throw new std::exception();
		}
		llvm::Value* CastedValueCompileEnv::GetEnvTypeArgumentArray(NomBuilder& builder)
		{
			return instanceTypeArrPtr->getType() == TYPETYPE->getPointerTo()->getPointerTo() ? MakeInvariantLoad(builder, instanceTypeArrPtr) : instanceTypeArrPtr;
		}
		void CastedValueCompileEnv::PushDispatchPair(llvm::Value* dpair)
		{
			throw new std::exception();
		}
		llvm::Value* CastedValueCompileEnv::PopDispatchPair()
		{
			throw new std::exception();
		}
#pragma endregion
	}
}
