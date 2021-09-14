#pragma once
//parts of RTConfig.h relevant for more wide-spread compilation code -- hopefully changes less

extern int NomLambdaOptimizationLevel;
extern int IMTsize;
extern bool PreferTypeArgumentsInVTables;

extern bool RTConfig_CheckLambdaSignaturesAtCast;
extern int RTConfig_NumberOfVarargsArguments;
extern bool RTConfig_AlwaysEnsureMethod;
extern bool RTConfig_IgnoreEnsureMethod;
extern bool RTConfig_RunUnncessesaryCallTagChecks;
extern bool RTConfig_OmitLambdaCallTags;
extern bool RTConfig_OmitCallTagCasts;