#include "../Parse/ComponentSource.h"
#include "../Intermediate_Representation/Data/NomProgram.h"

namespace Nom
{
	namespace Runtime
	{

		ComponentSource::ComponentSource(NomProgram * program) : Program(program)
		{
		}


		ComponentSource::~ComponentSource()
		{
		}
	}
}