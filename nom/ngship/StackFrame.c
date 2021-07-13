#include "StackFrame.h"
#include <stdarg.h>
#include <stdlib.h>

_class_Any * ThrowException(StackFrame * f, char * msg, ...)
{
	va_list ap;
	printf("Method %s (%s, Line %u, Column %u): ", f->methodName, f->currentCodePos.file, f->currentCodePos.line, f->currentCodePos.col);
	va_start(ap, msg);
	vprintf(msg,ap);
	va_end(ap);
	f = f->prev;
	printf("\n");
	while (f != NULL)
	{
		printf("@%s (%s, Line %u, Column %u)\n", f->methodName, f->currentCodePos.file, f->currentCodePos.line, f->currentCodePos.col);
		f = f->prev;
	}
	exit(1);
	return (_class_Any *)NULL;
}

//void ThrowExceptionArgs(StackFrame * f, char * msg, int argc, _class_Any ** args)
//{
//	printf("Exception!\n@%s (%s, Line %u, Column %u): %s\n", f->methodName, f->currentCodePos.file, f->currentCodePos.line, f->currentCodePos.col, msg);
//	for (int i = 0;i < argc;i++)
//	{
//		printf("  %s\n", _ng_global_typenames[args[i]->type]);
//	}
//	f = f->prev;
//	while (f->prev != NULL)
//	{
//		printf("@%s (%s, Line %u, Column %u)\n", f->methodName, f->currentCodePos.file, f->currentCodePos.line, f->currentCodePos.col);
//		f = f->prev;
//	}
//	exit(0);
//}