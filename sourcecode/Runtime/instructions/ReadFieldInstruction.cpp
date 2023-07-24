#include "ReadFieldInstruction.h"
#include "../CompileEnv.h"
#include "../NomType.h"
#include "../NomConstants.h"
#include "../NomClass.h"
#include "../NomClassType.h"
#include "../NomLambda.h"
#include "../NomRecord.h"
#include <iostream>

using namespace std;
namespace Nom
{
	namespace Runtime
	{

		ReadFieldInstruction::ReadFieldInstruction(const RegIndex reg, const RegIndex receiver, const ConstantID fieldName, const ConstantID type) : NomValueInstruction(reg, OpCode::ReadField), Register(reg), Receiver(receiver), FieldName(fieldName), ReceiverType(type)
		{
		}


		ReadFieldInstruction::~ReadFieldInstruction()
		{
		}
		void ReadFieldInstruction::Compile(NomBuilder& builder, CompileEnv* env, [[maybe_unused]] size_t lineno)
		{
			if (ReceiverType != 0)
			{
				NomConstant* receiverConstant = NomConstants::Get(ReceiverType);
				if (receiverConstant->Type == NomConstantType::CTClass)
				{
					auto reccls = (static_cast<NomClassConstant*>(receiverConstant))->GetClass();
					auto field = reccls->GetField(NomConstants::GetString(FieldName)->GetText());
					RegisterValue(env, field->GenerateRead(builder, env, (*env)[Receiver]));
				}
				else if(receiverConstant->Type == NomConstantType::CTLambda)
				{
					auto reclambda = (static_cast<NomLambdaConstant*>(receiverConstant))->GetLambda();
					auto field = reclambda->GetField(NomConstants::GetString(FieldName)->GetText());
					RegisterValue(env, field->GenerateRead(builder, env, (*env)[Receiver]));
				}
				else if (receiverConstant->Type == NomConstantType::CTRecord)
				{
					auto recstruct = (static_cast<NomRecordConstant*>(receiverConstant))->GetRecord();
					auto field = recstruct->GetField(NomConstants::GetString(FieldName)->GetText());
					RegisterValue(env, field->GenerateRead(builder, env, (*env)[Receiver]));
				}
				else
				{
					throw new std::exception();
				}

			}
			else
			{
				RegisterValue(env, NomDictField::GetInstance(NomConstants::GetString(FieldName)->GetText())->GenerateRead(builder, env, (*env)[Receiver]));
			}
		}
		void ReadFieldInstruction::Print(bool resolve)
		{
			cout << "ReadField from #" << std::dec << Receiver;
			cout << " ";
			NomConstants::PrintConstant(ReceiverType, resolve);
			cout << ".";
			NomConstants::PrintConstant(FieldName, resolve);
			cout << " -> #" << std::dec << WriteRegister;
			cout << "\n";
		}
		void ReadFieldInstruction::FillConstantDependencies(NOM_CONSTANT_DEPENCENCY_CONTAINER& result)
		{
			result.push_back(FieldName);
			result.push_back(ReceiverType);
		}
	}
}
