#include "NomField.h"
#include "NomClass.h"
#include "NomConstants.h"
#include "NomClassType.h"
#include "instructions/CastInstruction.h"
#include "NomType.h"
#include "NomString.h"
#include "NomNameRepository.h"
#include <unordered_map>
#include "CompileHelpers.h"
#include "TypeOperations.h"
#include "NomLambda.h"
#include "LambdaHeader.h"
#include "RTOutput.h"
#include "RecordHeader.h"
#include "RTRecord.h"
#include "RefValueHeader.h"
#include "RTVTable.h"
#include "RTCast.h"
#include "IntClass.h"
#include "FloatClass.h"
#include "CallingConvConf.h"
#include "NomTopType.h"
#include "IMT.h"
#include "PWLambda.h"
#include "PWObject.h"
#include "PWRecord.h"

using namespace llvm;
using namespace std;
namespace Nom
{
	namespace Runtime
	{

		NomTypedField::NomTypedField(NomClass* _cls, const ConstantID _name, const ConstantID _type, Visibility _visibility, bool _readonly, bool _isvolatile) : Name(_name), Type(_type), Class(_cls), visibility(_visibility), readonly(_readonly), isvolatile(_isvolatile)
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

		NomValue NomTypedField::GenerateRead(NomBuilder& builder, [[maybe_unused]] CompileEnv* env, NomValue receiver) const
		{
			auto recinst = Class->GetInstantiation(receiver.GetNomType());
			if (recinst != nullptr)
			{
				llvm::Value* retval = PWObject(receiver).ReadField(builder, PWCInt32(this->Index, false) , this->Class->GetHasRawInvoke());
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
				ObjectHeader::WriteField(builder, receiver, PWCInt32(this->Index,false), value, this->Class->GetHasRawInvoke());
				return;

			}
			throw new std::exception();
		}

		void NomTypedField::SetIndex(size_t index) const
		{
			if (!IndexSet)
			{
				Index = index;
				IndexSet = true;
			}
			else
			{
				throw new std::exception();
			}
		}

		NomDictField* NomDictField::GetInstance(NomStringRef name)
		{
			[[clang::no_destroy]] static std::unordered_map<const NomString*, NomDictField, NomStringHash, NomStringEquality> fields;
			decltype(fields)::iterator ret = fields.find(name);
			if (ret == fields.end())
			{
				fields.emplace(name, NomDictField(name));
				return &(*fields.find(name)).second;
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
		void NomDictField::GenerateWrite(NomBuilder& builder, [[maybe_unused]] CompileEnv* env, NomValue receiver, NomValue value) const
		{
			BasicBlock* origBlock = builder->GetInsertBlock();
			Function* fun = origBlock->getParent();

			BasicBlock* refValueBlock = nullptr, * packedIntBlock = nullptr, * packedFloatBlock = nullptr, * primitiveIntBlock = nullptr, * primitiveFloatBlock = nullptr, * primitiveBoolBlock = nullptr;

			RefValueHeader::GenerateRefOrPrimitiveValueSwitch(builder, receiver, &refValueBlock, &packedIntBlock, &packedFloatBlock, false, &primitiveIntBlock, nullptr, &primitiveFloatBlock, nullptr, &primitiveBoolBlock, nullptr);

			if (refValueBlock != nullptr)
			{
				builder->SetInsertPoint(refValueBlock);
				auto vtableVar = RefValueHeader::GenerateReadVTablePointer(builder, receiver);
				auto fieldStoreFun = RTVTable::GenerateReadWriteFieldFunction(builder, vtableVar);
				builder->CreateCall(GetFieldWriteFunctionType(), fieldStoreFun, { MakeInt<DICTKEYTYPE>(NomNameRepository::Instance().GetNameID(this->Name->ToStdString())), receiver,  EnsurePacked(builder, value) })->setCallingConv(NOMCC);
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


		NomClosureField::NomClosureField(NomLambda* lambda, const ConstantID name, const ConstantID type, const size_t index) : Name(name), Type(type), Lambda(lambda), Index(index)
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
		NomValue NomClosureField::GenerateRead(NomBuilder& builder, [[maybe_unused]] CompileEnv* env, NomValue receiver) const
		{
			return NomValue(PWLambdaPrecise(receiver, Lambda).ReadLambdaField(builder, Index), GetType());
		}
		void NomClosureField::GenerateWrite([[maybe_unused]] NomBuilder& builder, [[maybe_unused]] CompileEnv* env, [[maybe_unused]] NomValue receiver, [[maybe_unused]] NomValue value) const
		{
			throw new std::exception();
		}
		NomRecordField::NomRecordField(NomRecord* _structure, const ConstantID _name, const ConstantID _type, bool _isReadOnly, const size_t _index, RegIndex _valueRegister) : readonly(_isReadOnly), Name(_name), Type(_type), Structure(_structure), Index(_index), ValueRegister(_valueRegister)
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
		NomValue NomRecordField::GenerateRead(NomBuilder& builder, [[maybe_unused]] CompileEnv* env, NomValue receiver) const
		{
			llvm::Value* retval = RecordHeader::GenerateReadField(builder, receiver, PWCInt32(Index, false), this->Structure->GetHasRawInvoke()); //loadinst;
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
				RecordHeader::GenerateWriteWrittenTag(builder, receiver, PWCInt32(Index, false), Structure->Fields.size());
			}
			RecordHeader::GenerateWriteField(builder, receiver, PWCInt32(Index, false), value, Structure->Fields.size());
			return;
		}
	}
}
