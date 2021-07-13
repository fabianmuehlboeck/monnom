// RuntimeData.cpp : Defines the exported functions for the static library.
//

#include "RuntimeData.h"

// This is an example of an exported variable
int nRuntimeData=0;

// This is an example of an exported function.
int fnRuntimeData(void)
{
    return 42;
}

// This is the constructor of a class that has been exported.
// see RuntimeData.h for the class definition
CRuntimeData::CRuntimeData()
{
    return;
}
