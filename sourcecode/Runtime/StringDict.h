#pragma once

#include <string>
#include <unordered_map>
#include "Defs.h"
#include "NomString.h"

namespace Nom
{
	namespace Runtime
	{
		struct stringref_hash
		{
			std::size_t operator()(const std::string * const s) const noexcept
			{
				if (s == nullptr) {
					return 0;
				}
				return std::hash<std::string>()(*s);
			}
		};

		struct stringref_equalto
		{
			constexpr bool operator()(const std::string *lhs, const std::string *rhs) const
			{
				if (lhs == nullptr || rhs == nullptr)
				{
					return lhs == rhs;
				}
				return std::equal_to<std::string>()(*lhs, *rhs);
			}
		};

		template<typename T>
		using StringDict = std::unordered_map<const std::string *, T, stringref_hash, stringref_equalto>;

		struct nomstringref_hash
		{
			std::size_t operator()(const NomString * const s) const noexcept
			{
				if (s == nullptr) {
					return 0;
				}
				return std::hash<NomString>()(*s);
			}
		};

		struct nomstringref_equalto
		{
			constexpr bool operator()(const NomString * lhs, const NomString *rhs) const
			{
				if (lhs == nullptr || rhs == nullptr)
				{
					return lhs == rhs;
				}
				return std::equal_to<NomString>()(*lhs, *rhs);
			}
		};

		template<typename T>
		using NomStringDict = std::unordered_map<const NomString *, T, nomstringref_hash, nomstringref_equalto>;
	}
}
