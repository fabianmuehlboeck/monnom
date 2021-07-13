#pragma once

namespace Nom
{
	namespace Runtime
	{
		class NomSubstitutionContext;
		class NomSubstitutionStack
		{
		public:
			const NomSubstitutionContext* const Context;
			const NomSubstitutionStack* const Rest;

			NomSubstitutionStack(const NomSubstitutionContext* context, const NomSubstitutionStack* rest) : Context(context), Rest(rest)
			{

			}
			~NomSubstitutionStack()
			{

			}
		};
	}
}