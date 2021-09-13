#include "WriteFieldInstruction.h"
#include "../CompileEnv.h"
#include "../NomType.h"
#include "../NomConstants.h"
#include "../NomClass.h"
#include "../NomClassType.h"
#include <iostream>

using namespace std;
namespace Nom
{
	namespace Runtime
	{

		WriteFieldInstruction::WriteFieldInstruction(const RegIndex receiver, const RegIndex value, const ConstantID fieldName, const ConstantID type) : NomInstruction(OpCode::WriteField), ValueRegister(value), Receiver(receiver), FieldName(fieldName), ReceiverType(type)
		{
		}


		WriteFieldInstruction::~WriteFieldInstruction()
		{
		}
		void WriteFieldInstruction::Compile(NomBuilder& builder, CompileEnv* env, int lineno)
		{
			if (ReceiverType != 0)
			{
				NomConstant* receiverConstant = NomConstants::Get(ReceiverType);
				switch (receiverConstant->Type)
				{
				case NomConstantType::CTClass: {

					auto reccls = ((NomClassConstant*)receiverConstant)->GetClass();
					auto field = reccls->GetField(NomConstants::GetString(FieldName)->GetText());
					field->GenerateWrite(builder, env, (*env)[Receiver], (*env)[ValueRegister]);
					break;
				}
				case NomConstantType::CTLambda: {
					//auto reclambda = ((NomLambdaConstant*)receiverConstant)->GetLambda();
					//auto field = reclambda->GetField(NomConstants::GetString(FieldName)->GetText());
					//field->GenerateWrite(builder, env, (*env)[Receiver], (*env)[ValueRegister]);
					//this should never happen, but if it does, we'll get the appropriate error by calling the dynamic field write function
					NomDictField::GetInstance(NomConstants::GetString(FieldName)->GetText())->GenerateWrite(builder, env, (*env)[Receiver], (*env)[ValueRegister]);
					break;
				}
				case NomConstantType::CTRecord: {
					auto recstruct = ((NomRecordConstant*)receiverConstant)->GetRecord();
					auto field = recstruct->GetField(NomConstants::GetString(FieldName)->GetText());
					field->GenerateWrite(builder, env, (*env)[Receiver], (*env)[ValueRegister]);
					break;
				}
				default:
					throw new std::exception();
				}
				//auto reccls = NomConstants::GetClass(ReceiverType)->GetClass();
				//auto field = reccls->GetField(NomConstants::GetString(FieldName)->GetText());
				//field->GenerateWrite(builder, env, (*env)[Receiver], (*env)[ValueRegister]);
			}
			else
			{
				NomDictField::GetInstance(NomConstants::GetString(FieldName)->GetText())->GenerateWrite(builder, env, (*env)[Receiver], (*env)[ValueRegister]);
			}
		}
		void WriteFieldInstruction::Print(bool resolve)
		{
			cout << "WriteField to #" << std::dec << Receiver;
			cout << " ";
			NomConstants::PrintConstant(ReceiverType, resolve);
			cout << ".";
			NomConstants::PrintConstant(FieldName, resolve);
			cout << " from #" << std::dec << ValueRegister;
			cout << "\n";
		}
		void WriteFieldInstruction::FillConstantDependencies(NOM_CONSTANT_DEPENCENCY_CONTAINER& result)
		{
			result.push_back(FieldName);
			result.push_back(ReceiverType);
		}
	}
}