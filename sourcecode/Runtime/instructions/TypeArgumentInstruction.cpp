#include "TypeArgumentInstruction.h"


namespace Nom
{
	namespace Runtime
	{

		TypeArgumentInstruction::TypeArgumentInstruction(const NomType * type) : NomInstruction(OpCode::TypeArgument), Type(type)
		{
		}


		TypeArgumentInstruction::~TypeArgumentInstruction()
		{
		}
		void TypeArgumentInstruction::Compile(NomBuilder& builder, CompileEnv* env, int lineno)
		{
			throw "Not implemented";
		}
		void TypeArgumentInstruction::FillConstantDependencies(NOM_CONSTANT_DEPENCENCY_CONTAINER& result)
		{
		}
	}
}
