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
				auto reccls = NomConstants::GetClass(ReceiverType)->GetClass();
				auto field = reccls->GetField(NomConstants::GetString(FieldName)->GetText());
				field->GenerateWrite(builder, env, (*env)[Receiver], (*env)[ValueRegister]);

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