#pragma once
#include <iostream>
PUSHDIAGSUPPRESSION
#include "llvm/Support/TargetSelect.h"
#include "llvm/Target/TargetMachine.h"
POPDIAGSUPPRESSION
#include <vector>

class NomConstant;

// This is an example of a class exported from the Runtime.lib
class CRuntime
{
public:
    CRuntime();
    // TODO: add your methods here.
};

// This is an example of an exported variable
extern int nRuntime;

// This is an example of an exported function.
int fnRuntime(void);

int test();

int run(const std::vector<std::string> args);

void initLibrary();

int main(int argc, char** args);

int isInWarmup();
