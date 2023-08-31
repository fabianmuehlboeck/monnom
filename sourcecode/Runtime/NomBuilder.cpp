#include "NomBuilder.h"

#define PAGESIZE 4096

namespace Nom
{
	namespace Runtime
	{
		void NomBuilder::AllocNewPage()
		{
			char* newpage = reinterpret_cast<char*>(malloc(PAGESIZE));
			if (curallocstart != nullptr)
			{
				*curallocstart = newpage;
				if (firstpageptr == nullptr)
				{
					firstpageptr = curallocstart;
				}
			}
			curallocstart=reinterpret_cast<char**>(newpage);
			*curallocstart = nullptr;
			curallocend = newpage + PAGESIZE;
			curallocpos = reinterpret_cast<char*>(curallocstart+1);
		}
		NomBuilder::~NomBuilder()
		{
			while (firstpageptr != nullptr)
			{
				char** curptr = firstpageptr;
				firstpageptr = reinterpret_cast<char**>(*curptr);
				free(curptr);
			}
			if (curallocstart != nullptr)
			{
				free(curallocstart);
			}
		}
		char* NomBuilder::Malloc(size_t size)
		{
			if (curallocpos + size >= curallocend)
			{
				if (size > PAGESIZE-sizeof(char*))
				{
					throw new std::exception();
				}
				AllocNewPage();
			}
			char* ret = curallocpos;
			curallocpos += size;
			return ret;
		}
	}
}
