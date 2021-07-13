#ifndef NG_METADATA
#define NG_METADATA
#include <inttypes.h>
#include "Any.h"
#include "datatypes.h"
#include "StackFrame.h"
#include "constants.h"

typedef struct struct_NG_DISPATCHER_ENTRY
{
	unsigned int name;
	_class_Any *(*method)(StackFrame *, _class_Any *, unsigned int, _class_Any **);
} NG_DISPATCHER_ENTRY;


typedef struct struct_NG_DISPATCHERTABLE
{
	unsigned int entrycount;
	NG_DISPATCHER_ENTRY* entries;
} NG_DISPATCHERTABLE;


typedef struct struct_NG_DYNDISPATCHER_ENTRY
{
	unsigned int name;
	unsigned int (*method)(StackFrame *, _class_Any *, unsigned int, _class_Any **);
} NG_DYNDISPATCHER_ENTRY;


typedef struct NG_DYNDISPATCHERTABLE
{
	unsigned int entrycount;
	NG_DYNDISPATCHER_ENTRY* entries;
} NG_DYNDISPATCHERTABLE;


typedef struct struct_NG_SUPERTYPE_ENTRY
{
	NG_TYPE type;
	unsigned int methodstart;
} NG_SUPERTYPE_ENTRY;


typedef struct struct_NG_SUPERTYPE_LIST
{
	unsigned int entrycount;
	NG_SUPERTYPE_ENTRY* entries;
} NG_SUPERTYPE_LIST;

typedef struct struct_NG_METHODDATA_ENTRY
{
	NG_TYPE type;
	NG_CONTEXTID context;
	unsigned int access;
	unsigned int name;
	unsigned int argcount;
	NG_TYPE *argtypes;
	unsigned int supercount;
	struct struct_NG_METHODDATA_ENTRY **supermethods;
} NG_METHODDATA_ENTRY;

typedef struct struct_NG_METHODDATA_LIST
{
	unsigned int entrycount;
	NG_METHODDATA_ENTRY* entries;
} NG_METHODDATA_LIST;

typedef struct struct_NG_FIELDDICT_ENTRY
{
	unsigned int name;
	NG_CONTEXTID context;
	unsigned int access;
	unsigned int readonly;
	NG_TYPE type;
	unsigned int offset;
} NG_FIELDDICT_ENTRY;

typedef struct struct_NG_FIELDDICT
{
	unsigned int entrycount;
	NG_FIELDDICT_ENTRY *entries;
} NG_FIELDDICT;


typedef struct struct_NG_CONTEXT_ENTRY
{
	NG_CONTEXTID contextID;
	NG_ACCESSLEVEL access;
} NG_CONTEXT_ENTRY;

typedef struct struct_NG_CONTEXT_DATA NG_CONTEXT_DATA;
typedef struct struct_NG_CONTEXT_DATA
{
	NG_CONTEXT_DATA * parent;
	NG_CONTEXTID contextID;
	NG_ACCESSLEVEL parentAccess;
	unsigned int entrycount;
	NG_CONTEXT_ENTRY *entries;

} NG_CONTEXT_DATA;

#ifdef NG_DISPATCHER_CACHE_SIZE
typedef struct struct_NG_DISPATCHER_CACHE_ENTRY
{
	NG_TYPE type;
	unsigned int offset;
	NG_TYPE *argtypes;
} NG_DISPATCHER_CACHE_ENTRY;

typedef struct struct_NG_DISPATCHER_CACHE
{
	NG_DISPATCHER_CACHE_ENTRY entries[NG_DISPATCHER_CACHE_SIZE];
	unsigned short pos;
} NG_DISPATCHER_CACHE;
#endif

#ifdef NG_INTERFACE_CACHE_SIZE
typedef struct struct_NG_INTERFACE_CACHE_ENTRY
{
	NG_TYPE type;
	unsigned int offset;
} NG_INTERFACE_CACHE_ENTRY;

typedef struct struct_NG_INTERFACE_CACHE
{
	NG_INTERFACE_CACHE_ENTRY entries[NG_INTERFACE_CACHE_SIZE];
	unsigned int pos;
} NG_INTERFACE_CACHE;
#endif

#endif