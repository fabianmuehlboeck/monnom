#include "NomBuilder.h"

#define PAGESIZE 4096

namespace Nom
{
	namespace Runtime
	{
		void NomBuilder::AllocNewPage()
		{
			char* newpage = reinterpret_cast<char*>(malloc(PAGESIZE));
			if (curallocstart != 0)
			{
				*curallocstart = newpage;
				if (firstpageptr == 0)
				{
					firstpageptr = curallocstart;
				}
			}
			curallocstart=reinterpret_cast<char**>(newpage);
			*curallocstart = 0;
			curallocend = newpage + PAGESIZE;
			curallocpos = reinterpret_cast<char*>(curallocstart+1);
		}
		NomBuilder::~NomBuilder()
		{
			while (firstpageptr != 0)
			{
				char** curptr = firstpageptr;
				firstpageptr = reinterpret_cast<char**>(*curptr);
				free(curptr);
			}
			if (curallocstart != 0)
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
