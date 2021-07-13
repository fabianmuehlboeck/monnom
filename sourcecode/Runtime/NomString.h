#pragma once

#include "Defs.h"
#include "AvailableExternally.h"

namespace Nom
{
	namespace Runtime
	{
		class NomString : public NomStringType, public AvailableExternally<llvm::Constant>
		{
		private:
			mutable size_t hashCode = 0;
			//mutable llvm::GlobalVariable* llvmVar = nullptr;
			//mutable llvm::GlobalVariable* llvmStruct = nullptr;
		public:
			NomString() : NomStringType()
			{
			}
			NomString(const NomString& other) : NomStringType(other)
			{
			}
			NomString(const NomString&& other) : NomStringType(other)
			{
			}
			NomString(const char* const other) : NomStringType(strlen(other), 0)
			{
				for (int i = strlen(other) - 1; i >= 0; i--)
				{
					(*this)[i] = other[i];
				}
			}
			NomString(std::string other) : NomStringType(other.size(), 0)
			{
				for (int i = other.size() - 1; i >= 0; i--)
				{
					(*this)[i] = other[i];
				}
			}
			NomString(const uint64_t length, CharStream &stream) : NomStringType(length, 0) {
				for (uint64_t i = 0; i < length; i++) {
					std::array<unsigned char, sizeof(NomChar)> chars = stream.read_chars<sizeof(NomChar)>();
					unsigned char* charptr = chars.data();
					(*this)[i] = *(reinterpret_cast<NomChar*>(charptr));
				}
			}
			std::string ToStdString() const
			{
				std::string ret(size(), 0);
				for (int i = size() - 1; i >= 0; i--)
				{
					ret[i] = (*this)[i];
				}
				return ret;
			}

			std::size_t HashCode() const noexcept
			{
				if (hashCode == 0)
				{
					size_t hcval = 0; //needed for concurrent computations
					size_t mult = 1;
					for (int i = size() - 1; i >= 0; i--)
					{
						size_t charval = (*this)[i];
						hcval += charval * mult;
						mult *= 31;
					}
					hashCode = hcval;
				}
				return hashCode;
			}
			//llvm::GlobalVariable * GetLLVMStringStruct(llvm::Module &mod, bool refOnly = false) const;
			llvm::Constant* GetLLVMConstant(llvm::Module& mod) const;
			//llvm::StructType* GetLLVMConstantType() const;

			//static void * GetStringObject(size_t size, NomChar * chars);
			void* GetStringObject() const;

			// Inherited via AvailableExternally
			virtual llvm::Constant* createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const override;
			virtual llvm::Constant* findLLVMElement(llvm::Module& mod) const override;
		};

		struct NomStringHash
		{
			std::size_t operator()(NomStringRef & s) const noexcept
			{
				return this->operator()(*s);
			}

			std::size_t operator()(NomString const& s) const noexcept
			{
				std::size_t ret = 0;
				std::size_t n = s.length();
				std::size_t mult = 1;
				constexpr std::size_t points = sizeof(std::size_t) / sizeof(NomChar);
				std::size_t singles = n % points;
				while (singles-- > 0)
				{
					n--;
					ret += ((const std::size_t)(*(s.data() + n))) * mult;
					mult *= 31;
				}
				while (n > 0)
				{
					n -= points;
					ret += (*((const std::size_t *)(s.data() + n))) * mult;
					mult *= 31;
				}
				return ret;
			}
		};

		struct NomStringEquality
		{
			bool operator()(NomStringRef &lhs, NomStringRef &rhs) const
			{
				return this->operator()(*lhs, *rhs);
			}

			bool operator()(const NomString &lhs, const NomString &rhs) const
			{
				return lhs==rhs;
			}
		};
	}
}

namespace std
{
	template<>
	struct hash<Nom::Runtime::NomString>
	{
		typedef Nom::Runtime::NomString argument_type;
		typedef std::size_t result_type;
		result_type operator()(argument_type const& s) const noexcept
		{
			return s.HashCode();
		}
	};

	template<>
	struct equal_to<Nom::Runtime::NomString>
	{
		typedef Nom::Runtime::NomString argument_type;
		constexpr bool operator()(const argument_type& lhs, const argument_type& rhs) const noexcept
		{
			return std::equal_to<Nom::Runtime::NomStringType>()(lhs, rhs);
		}

	};
}
