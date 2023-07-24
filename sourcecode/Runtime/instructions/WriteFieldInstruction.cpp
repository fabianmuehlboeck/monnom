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
		void WriteFieldInstruction::Compile(NomBuilder& builder, CompileEnv* env, [[maybe_unused]] size_t lineno)
		{
			if (ReceiverType != 0)
			{
				NomConstant* receiverConstant = NomConstants::Get(ReceiverType);
				if (receiverConstant->Type == NomConstantType::CTClass)
				{

					auto reccls = (static_cast<NomClassConstant*>(receiverConstant))->GetClass();
					auto field = reccls->GetField(NomConstants::GetString(FieldName)->GetText());
					field->GenerateWrite(builder, env, (*env)[Receiver], (*env)[ValueRegister]);
				}
				else if (receiverConstant->Type == NomConstantType::CTLambda)
				{
					//this should never happen, but if it does, we'll get the appropriate error by calling the dynamic field write function
					NomDictField::GetInstance(NomConstants::GetString(FieldName)->GetText())->GenerateWrite(builder, env, (*env)[Receiver], (*env)[ValueRegister]);
				}
				else if (receiverConstant->Type == NomConstantType::CTRecord)
				{
					auto recstruct = (static_cast<NomRecordConstant*>(receiverConstant))->GetRecord();
					auto field = recstruct->GetField(NomConstants::GetString(FieldName)->GetText());
					field->GenerateWrite(builder, env, (*env)[Receiver], (*env)[ValueRegister]);
				}
				else
				{
					throw new std::exception();
				}
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
