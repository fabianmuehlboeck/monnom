#include "../Intermediate_Representation/NomField.h"
#include "../Intermediate_Representation/Data/NomClass.h"
#include "../Intermediate_Representation/NomConstants.h"
#include "../Intermediate_Representation/Types/NomClassType.h"
#include "../instructions/CastInstruction.h"
#include "../Intermediate_Representation/Types/NomType.h"
#include "../Intermediate_Representation/NomString.h"
#include "../Intermediate_Representation/NomNameRepository.h"
#include <unordered_map>
#include "../Common/CompileHelpers.h"
#include "../Common/TypeOperations.h"
#include "../Intermediate_Representation/Data/NomLambda.h"
#include "../Runtime_Data/Headers/LambdaHeader.h"
#include "../Runtime/RTOutput.h"
#include "../Runtime_Data/Headers/RecordHeader.h"
#include "../Runtime_Data/VTables/RTRecord.h"
#include "../Runtime_Data/Headers/RefValueHeader.h"
#include "../Runtime_Data/VTables/RTVTable.h"
#include "../Runtime/Typing/RTCast.h"
#include "../Library/IntClass.h"
#include "../Library/FloatClass.h"
#include "../Common/CallingConvConf.h"
#include "../Intermediate_Representation/Types/NomTopType.h"
#include "../Runtime_Data/IMT.h"

using namespace llvm;
using namespace std;
namespace Nom
{
	namespace Runtime
	{

		NomTypedField::NomTypedField(NomClass* cls, const ConstantID name, const ConstantID type, Visibility visibility, bool readonly, bool isvolatile) : Name(name), Type(type), Class(cls), visibility(visibility), readonly(readonly), isvolatile(isvolatile)
		{
		}

		NomField::NomField()
		{

		}

		NomDictField::NomDictField(NomStringRef name) : Name(name)
		{

		}

		NomField::~NomField()
		{
		}

		NomTypedField::~NomTypedField()
		{
		}

		NomTypeRef NomTypedField::GetType() const
		{
			NomSubstitutionContextMemberContext nscmc(Class);
			return NomConstants::GetType(&nscmc, Type);
		}

		NomStringRef NomTypedField::GetName() const
		{
			return NomConstants::GetString(Name)->GetText();
		}

		NomValue NomTypedField::GenerateRead(NomBuilder& builder, CompileEnv* env, NomValue receiver) const
		{
			auto recinst = Class->GetInstantiation(receiver.GetNomType());
			if (recinst != nullptr)
			{
				llvm::Value* retval = ObjectHeader::ReadField(builder, receiver, this->Index, this->Class->GetHasRawInvoke());
				if (this->GetType()->IsSubtype(NomIntClass::GetInstance()->GetType(), false))
				{
					retval = builder->CreatePtrToInt(retval, INTTYPE);
				}
				else if (this->GetType()->IsSubtype(NomFloatClass::GetInstance()->GetType(), false))
				{
					retval = builder->CreateBitCast(builder->CreatePtrToInt(retval, INTTYPE), FLOATTYPE);
				}
				NomSubstitutionContextList nscl(recinst->Arguments);
				return NomValue(retval, GetType()->SubstituteSubtyping(&nscl));
			}
			throw new std::exception();
		}

		void NomTypedField::GenerateWrite(NomBuilder& builder, CompileEnv* env, NomValue receiver, NomValue value) const
		{
			auto recinst = Class->GetInstantiation(receiver.GetNomType());
			if (recinst != nullptr)
			{
				NomSubstitutionContextList nscl(recinst->Arguments);
				if (!value.GetNomType()->IsSubtype(this->GetType()->SubstituteSubtyping(&nscl)))
				{
					value = CastInstruction::MakeCast(builder, env, value, this->GetType()->SubstituteSubtyping(&nscl));
				}
				if (this->GetType()->IsSubtype(NomIntClass::GetInstance()->GetType(), false))
				{
					value = NomValue(builder->CreateIntToPtr(EnsureUnpackedInt(builder, env, value), REFTYPE), value.GetNomType());
				}
				else if (this->GetType()->IsSubtype(NomFloatClass::GetInstance()->GetType(), false))
				{
					value = NomValue(builder->CreateIntToPtr(builder->CreateBitCast(EnsureUnpackedFloat(builder, env, value), INTTYPE), REFTYPE), value.GetNomType());
				}
				else
				{
					value = EnsurePacked(builder, value);
				}
				ObjectHeader::WriteField(builder, receiver, this->Index, value, this->Class->GetHasRawInvoke());
				return;

			}
			throw new std::exception();
		}

		void NomTypedField::SetIndex(int index) const
		{
			if (Index < 0)
			{
				Index = index;
			}
			else
			{
				throw new std::exception();
			}
		}

		NomDictField* NomDictField::GetInstance(NomStringRef name)
		{
			static std::unordered_map<const NomString*, NomDictField, NomStringHash, NomStringEquality> fields;
			auto ret = fields.find(name);
			if (ret == fields.end())
			{
				fields.emplace(name, NomDictField(name));
				ret = fields.find(name);
			}
			return &(*ret).second;
		}

		NomDictField::~NomDictField()
		{
		}
		NomTypeRef NomDictField::GetType() const
		{
			return NomType::Anything;
		}
		NomStringRef NomDictField::GetName() const
		{
			return Name;
		}
		NomValue NomDictField::GenerateRead(NomBuilder& builder, CompileEnv* env, NomValue receiver) const
		{
			std::string key = GetName()->ToStdString();
			return NomValue(builder->CreatePointerCast(ObjectHeader::CreateDictionaryLoad(builder, env, receiver, MakeInt(NomNameRepository::Instance().GetNameID(key))), REFTYPE, key), NomType::DynamicRef);
		}
		void NomDictField::GenerateWrite(NomBuilder& builder, CompileEnv* env, NomValue receiver, NomValue value) const
		{
			BasicBlock* origBlock = builder->GetInsertBlock();
			Function* fun = origBlock->getParent();
			//BasicBlock* outBlock = BasicBlock::Create(LLVMCONTEXT, "DictWriteOut", fun);

			BasicBlock* refValueBlock = nullptr, * packedIntBlock = nullptr, * packedFloatBlock = nullptr, * primitiveIntBlock = nullptr, * primitiveFloatBlock = nullptr, * primitiveBoolBlock = nullptr;

			RefValueHeader::GenerateRefOrPrimitiveValueSwitch(builder, receiver, &refValueBlock, &packedIntBlock, &packedFloatBlock, false, &primitiveIntBlock, nullptr, &primitiveFloatBlock, nullptr, &primitiveBoolBlock, nullptr);

			if (refValueBlock != nullptr)
			{
				builder->SetInsertPoint(refValueBlock);
				auto vtableVar = RefValueHeader::GenerateReadVTablePointer(builder, receiver);
				//BasicBlock* realVtableBlock = nullptr, * pureLambdaBlock = nullptr, * pureStructBlock = nullptr, * purePartialAppBlock = nullptr;
				//Value* vtableVar = nullptr;
				//RefValueHeader::GenerateRefValueKindSwitch(builder, receiver, &vtableVar, &realVtableBlock, &pureLambdaBlock, &pureStructBlock, &purePartialAppBlock);

				//if (realVtableBlock != nullptr)
				//{
				//	builder->SetInsertPoint(realVtableBlock);
				auto fieldStoreFun = RTVTable::GenerateReadWriteFieldFunction(builder, vtableVar);
				builder->CreateCall(GetFieldWriteFunctionType(), fieldStoreFun, { MakeInt<DICTKEYTYPE>(NomNameRepository::Instance().GetNameID(this->Name->ToStdString())), receiver,  EnsurePacked(builder, value) })->setCallingConv(NOMCC);
				//	builder->CreateBr(outBlock);
				//}

				//if (pureLambdaBlock != nullptr)
				//{
				//	RTOutput_Fail::MakeBlockFailOutputBlock(builder, "Lambdas have no dictionary fields to write to!", pureLambdaBlock);
				//}
				//if (pureStructBlock != nullptr)
				//{
				//	builder->SetInsertPoint(pureStructBlock);
				//	auto vtable = vtableVar;
				//	auto fieldStoreFun = RTVTable::GenerateReadWriteFieldFunction(builder, vtable);
				//	builder->CreateCall(GetFieldWriteFunctionType(), fieldStoreFun, { MakeInt<DICTKEYTYPE>(NomNameRepository::Instance().GetNameID(this->Name->ToStdString())), receiver, EnsurePacked(builder, value) })->setCallingConv(NOMCC);
				//	builder->CreateBr(outBlock);
				//}
				//if (purePartialAppBlock != nullptr)
				//{
				//	RTOutput_Fail::MakeBlockFailOutputBlock(builder, "Partial Applications have no dictionary fields to write to!", purePartialAppBlock);
				//}
				refValueBlock = builder->GetInsertBlock();

			}
			if (packedIntBlock != nullptr)
			{
				RTOutput_Fail::MakeBlockFailOutputBlock(builder, "Integers have no dictionary fields to write to!", packedIntBlock);
			}
			if (packedFloatBlock != nullptr)
			{
				RTOutput_Fail::MakeBlockFailOutputBlock(builder, "Integers have no dictionary fields to write to!", packedFloatBlock);
			}
			if (primitiveIntBlock != nullptr)
			{
				throw new std::exception();
			}
			if (primitiveFloatBlock != nullptr)
			{
				throw new std::exception();
			}
			if (primitiveBoolBlock != nullptr)
			{
				throw new std::exception();
			}
			if (refValueBlock != nullptr)
			{
				builder->SetInsertPoint(refValueBlock);
			}
		}


		NomClosureField::NomClosureField(NomLambda* lambda, const ConstantID name, const ConstantID type, const int index) : Name(name), Type(type), Lambda(lambda), Index(index)
		{
		}
		NomClosureField::~NomClosureField()
		{
		}
		NomTypeRef NomClosureField::GetType() const
		{
			NomSubstitutionContextMemberContext nscmc(Lambda);
			return NomConstants::GetType(&nscmc, Type);
		}
		NomStringRef NomClosureField::GetName() const
		{
			return NomConstants::GetString(Name)->GetText();
		}
		NomValue NomClosureField::GenerateRead(NomBuilder& builder, CompileEnv* env, NomValue receiver) const
		{
			return NomValue(LambdaHeader::GenerateReadField(builder, Lambda, receiver, Index), GetType());
		}
		void NomClosureField::GenerateWrite(NomBuilder& builder, CompileEnv* env, NomValue receiver, NomValue value) const
		{
			throw new std::exception();
		}
		NomRecordField::NomRecordField(NomRecord* structure, const ConstantID name, const ConstantID type, bool isReadOnly, const int index, RegIndex valueRegister) : readonly(isReadOnly), Name(name), Type(type), Structure(structure), Index(index), ValueRegister(valueRegister)
		{
		}
		NomRecordField::~NomRecordField()
		{
		}
		NomTypeRef NomRecordField::GetType() const
		{
			NomSubstitutionContextMemberContext nscmc(Structure);
			return NomConstants::GetType(&nscmc, Type);
		}
		NomStringRef NomRecordField::GetName() const
		{
			return NomConstants::GetString(Name)->GetText();
		}
		NomValue NomRecordField::GenerateRead(NomBuilder& builder, CompileEnv* env, NomValue receiver) const
		{
			llvm::Value* retval = RecordHeader::GenerateReadField(builder, receiver, Index, this->Structure->GetHasRawInvoke()); //loadinst;
			return NomValue(retval, GetType());
		}
		void NomRecordField::GenerateWrite(NomBuilder& builder, CompileEnv* env, NomValue receiver, NomValue value) const
		{
			if (!value.GetNomType()->IsSubtype(this->GetType()))
			{
				value = CastInstruction::MakeCast(builder, env, value, this->GetType());
			}
			value = EnsurePacked(builder, value);
			if (!env->GetInConstructor())
			{
				RecordHeader::GenerateWriteWrittenTag(builder, receiver, Index, Structure->Fields.size());
			}
			RecordHeader::GenerateWriteField(builder, receiver, Index, value, Structure->Fields.size());
			return;
		}
	}
}