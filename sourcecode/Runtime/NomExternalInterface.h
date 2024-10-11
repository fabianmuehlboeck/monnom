#pragma once
#include <string>

extern "C" char* __NOM__NomStringToCString(void* nomstr, char* buffer, size_t maxsize);
extern "C" std::string __NOM__NomStringToCPPString(void* nomstr);

extern "C" void* __NOM__CStringToNomString(char* buffer);
extern "C" void* __NOM__CPPStringToNomString(std::string &str);

extern "C" void __NOM__SetField(void* object, int fieldidx, void* value);
extern "C" void* __NOM__GetField(void* object, int fieldidx);