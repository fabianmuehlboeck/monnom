#pragma once

#include <array>

#ifdef _WIN32
#include <istream>
#include <fstream>
#include <iostream>
#else
#include <sys/stat.h> 
#include <unistd.h>
#include <fcntl.h>
#endif

namespace Nom
{
	namespace Runtime
	{
		class CharStream
		{
		private:
			size_t overallpos = 0;
#ifdef _WIN32
			std::basic_istream<unsigned char>* stream;
#else
			char buf[2048];
			int pos = 0;
			int len = 0;
			int fp;
			bool ateof = false;
#endif
		public:
#ifdef _WIN32
			using int_type = std::basic_istream<unsigned char>::traits_type::int_type;
#define CS_EOF (std::basic_istream<unsigned char>::traits_type::eof())
#else
#define CS_EOF EOF
			using int_type = int;
#endif
			CharStream(std::string& path);
			~CharStream();
			bool HasNext();

			int_type peek_eof();

			unsigned char peek();

			bool try_peek(unsigned char& val);

			unsigned char read_char();

			template<unsigned int len>
			std::array<unsigned char, len> read_chars() {
				std::array<unsigned char, len> arr;
				for (unsigned int i = 0; i < len; i++) {
					arr[i] = read_char();
				}
				return arr;
			}

			template<typename T>
			T read() {
				unsigned char arr[sizeof(T)];
				for (size_t i = 0; i < sizeof(T); i++) {
					arr[i] = read_char();
				}
				unsigned char* arrptr = arr;
				return *(reinterpret_cast<T*>(arrptr));
			}
		};
	}
}
