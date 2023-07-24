#pragma once
PUSHDIAGSUPPRESSION
#include "llvm/IR/Constant.h"
POPDIAGSUPPRESSION
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
			NomString(const char* const other) : NomStringType(static_cast<NomStringType::size_type>(strlen(other)), 0)
			{
				NomStringType::size_type bound = static_cast<NomStringType::size_type>(strlen(other));
				if (bound > 0)
				{
					do
					{
						bound--;
						(*this)[bound] = static_cast<NomStringType::value_type>(other[bound]);
					} while (bound > 0);
				}
			}
			NomString(std::string other) : NomStringType(static_cast<NomStringType::size_type>(other.size()), 0)
			{
				NomStringType::size_type bound = static_cast<NomStringType::size_type>(other.size());
				if (bound > 0)
				{
					do
					{
						bound--;
						(*this)[bound] = static_cast<NomStringType::value_type>(other[bound]);
					} while (bound > 0);
				}
			}
			NomString(const uint64_t length, CharStream &stream) : NomStringType(static_cast<NomStringType::size_type>(length), 0) {
				auto llength = static_cast<NomStringType::size_type>(length);
				for (NomStringType::size_type i = 0; i < llength; i++) {
					std::array<unsigned char, sizeof(NomChar)> chars = stream.read_chars<sizeof(NomChar)>();
					unsigned char* charptr = chars.data();
					(*this)[i] = *(reinterpret_cast<NomChar*>(charptr));
				}
			}
			virtual ~NomString() override {}
			std::string ToStdString() const
			{
				NomStringType::size_type sz = size();
				std::string ret(static_cast<size_t>(sz), 0);
				if (sz > 0)
				{
					do
					{
						sz--;
						ret[sz] = static_cast<std::string::value_type>((*this)[sz]);
					} while (sz > 0);
				}
				return ret;
			}

			std::size_t HashCode() const noexcept
			{
				if (hashCode == 0)
				{
					size_t hcval = 0; //needed for concurrent computations
					size_t mult = 1;
					NomStringType::size_type sz = size();
					for (NomStringType::size_type i = 0; i < sz; i++)
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
					ret += (static_cast<const std::size_t>(*(s.data() + n))) * mult;
					mult *= 31;
				}
				while (n > 0)
				{
					n -= points;
					ret += (static_cast<const std::size_t>(*(s.data() + n))) * mult;
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

		struct StringPtrHash
		{
			std::size_t operator()(std::string*& s) const noexcept
			{
				return std::hash<std::string>()(*s);
			}
			std::size_t operator()(const std::string*& s) const noexcept
			{
				return std::hash<std::string>()(*s);
			}
		};
		struct StringPtrEq
		{
			bool operator()(std::string*& lhs, std::string *&rhs) const noexcept
			{
				return std::equal_to<std::string>()(*lhs, *rhs);
			}
			bool operator()(const std::string*& lhs, const std::string *&rhs) const noexcept
			{
				return std::equal_to<std::string>()(*lhs, *rhs);
			}
		};
		const std::string* getStringID(NomStringRef str);
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
