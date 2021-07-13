#ifndef DEF_NG_ANY
#define DEF_NG_ANY
#include "datatypes.h"
#include <inttypes.h>

typedef struct _NG_TYPE NG_TYPE;
typedef size_t NG_TYPEID;
typedef NG_TYPEID NG_TYPETAG;

typedef struct _NG_TYPE {
	uint64_t kind;
	union
	{
		NG_TYPEID type;
		NG_TYPE * list;
	};
} NG_TYPE;


typedef struct _struct_Any
{
	NG_TYPE type;
} _class_Any;

typedef struct _struct_Null
{
	_class_Any base;
} _class_Null;

extern _class_Null * NULLOBJ;

typedef struct _struct_Void
{
	_class_Any base;
} _class_Void;

extern _class_Void * VOIDOBJ;

typedef union _NG_DATA
{
	NG_FLOAT floatval;
	NG_INT intval;
	_class_Any * ptrval;
	uintptr_t maskval;
} NG_DATA;
#endif