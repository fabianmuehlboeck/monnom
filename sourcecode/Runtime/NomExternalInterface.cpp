#include "NomExternalInterface.h"
#include "Defs.h"
#include "CompileHelpers.h"
#include "NomVMInterface.h"

using namespace Nom::Runtime;

char* __NOM__NomStringToCString(void* nomstr, char* buffer, size_t size) {
	std::string str = ((NomStringRef)GetReadFieldFunction()(nomstr, 0))->ToStdString();
	const char* cstr=str.c_str();
	size_t ret = 0;
	return strncpy(buffer, cstr, size);
}
std::string __NOM__NomStringToCPPString(void* nomstr)
{
	return ((NomStringRef)GetReadFieldFunction()(nomstr, 0))->ToStdString();
}
void* __NOM__CStringToNomString(char* buffer, size_t maxsize)
{
	return NomString(buffer).GetStringObject();
}
void* __NOM__CPPStringToNomString(std::string& str)
{
	return NomString(str).GetStringObject();
}

void __NOM__SetField(void* object, int fieldidx, void* value)
{
	GetWriteFieldFunction()(object, fieldidx, value);
}

void* __NOM__GetField(void* object, int fieldidx)
{
	return GetReadFieldFunction()(object, fieldidx);
}

