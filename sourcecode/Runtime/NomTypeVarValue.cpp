#include "NomTypeVarValue.h"
#include "NomTypeVar.h"

const Nom::Runtime::NomType * Nom::Runtime::NomTypeVarValue::UpperBound() const
{
	return var->GetUpperBound();
}

const Nom::Runtime::NomType * Nom::Runtime::NomTypeVarValue::LowerBound() const
{
	return var->GetLowerBound();
}
