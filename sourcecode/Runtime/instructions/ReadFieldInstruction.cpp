#include "ReadFieldInstruction.h"
#include "../Common/CompileEnv.h"
#include "../Intermediate_Representation/Types/NomType.h"
#include "../Intermediate_Representation/NomConstants.h"
#include "../Intermediate_Representation/Data/NomClass.h"
#include "../Intermediate_Representation/Types/NomClassType.h"
#include "../Intermediate_Representation/Data/NomLambda.h"
#include "../Intermediate_Representation/Data/NomRecord.h"
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
		void ReadFieldInstruction::Compile(NomBuilder& builder, CompileEnv* env, int lineno)
		{
			if (ReceiverType != 0)
			{
				NomConstant* receiverConstant = NomConstants::Get(ReceiverType);
				switch (receiverConstant->Type)
				{
				case NomConstantType::CTClass: {

					auto reccls = ((NomClassConstant *)receiverConstant)->GetClass();
					auto field = reccls->GetField(NomConstants::GetString(FieldName)->GetText());
					RegisterValue(env, field->GenerateRead(builder, env, (*env)[Receiver]));
					break;
				}
				case NomConstantType::CTLambda: {
					auto reclambda = ((NomLambdaConstant*)receiverConstant)->GetLambda();
					auto field = reclambda->GetField(NomConstants::GetString(FieldName)->GetText());
					RegisterValue(env, field->GenerateRead(builder, env, (*env)[Receiver]));
					break;
				}
				case NomConstantType::CTRecord: {
					auto recstruct = ((NomRecordConstant*)receiverConstant)->GetRecord();
					auto field = recstruct->GetField(NomConstants::GetString(FieldName)->GetText());
					RegisterValue(env, field->GenerateRead(builder, env, (*env)[Receiver]));
					break;
				}
				default:
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