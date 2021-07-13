#pragma once
#include "variant.hpp"

namespace Nom
{
	namespace Runtime
	{
		template <typename... Ts> using variant = mpark::variant<Ts ...>;
	}
}