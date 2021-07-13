#ifndef NG_STACKFRAME
#define NG_STACKFRAME
#include <stdio.h>
#include "Any.h"

typedef struct SourcePos
{
	const char * file;
	unsigned int line;
	unsigned int col;
} SourcePos;

typedef struct StackFrame
{
	struct StackFrame * prev;
	const char * methodName;
	NG_CONTEXTID context;
	SourcePos currentCodePos;
} StackFrame;

_class_Any * ThrowException(StackFrame * f, char * msg, ...);
#endif