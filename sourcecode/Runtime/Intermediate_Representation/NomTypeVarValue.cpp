#include "../Intermediate_Representation/NomTypeVarValue.h"
#include "../Intermediate_Representation/Types/NomTypeVar.h"

const Nom::Runtime::NomType * Nom::Runtime::NomTypeVarValue::UpperBound() const
{
	return var->GetUpperBound();
}

const Nom::Runtime::NomType * Nom::Runtime::NomTypeVarValue::LowerBound() const
{
	return var->GetLowerBound();
}
