#include "CharStream.h"

namespace Nom
{
	namespace Runtime
	{
		CharStream::CharStream(std::string& path) :
#ifdef _WIN32
			stream(new std::basic_ifstream<unsigned char>(path, std::ios::in | std::ios::binary))
#else
			fp(open(path.c_str(), O_RDONLY))
#endif
		{
		}
		bool CharStream::HasNext()
		{
#ifdef _WIN32
			return !stream->eof();
#else
			if (pos < len)
			{
				return true;
			}
			if (!ateof)
			{
				len = ::read(fp, buf, 2048);
				ateof = len == 0;
				pos = 0;
			}
			return !ateof;
#endif
		}
		CharStream::int_type CharStream::peek_eof()
		{
#ifdef _WIN32
			return stream->peek();
#else
			if (HasNext())
			{
				return static_cast<int_type>(buf[pos]);
			}
			return CS_EOF;
#endif
		}
		unsigned char CharStream::peek() {
			int_type read = peek_eof();
			if (read == CS_EOF) {
				throw "Unexpected EOF";
			}
			return static_cast<unsigned char>(read);
		}
		bool CharStream::try_peek(unsigned char& val) {
			int_type read = peek_eof();
			if (read == CS_EOF) {
				return false;
			}
			val = static_cast<unsigned char>(read);
			return true;
		}
		unsigned char CharStream::read_char() {
			overallpos++;
#ifdef _WIN32
			int_type read = stream->get();
			if (read != CS_EOF)
			{
				return static_cast<unsigned char>(read);
			}
#else
			if (HasNext())
			{
				return static_cast<unsigned char>(buf[pos++]);
			}
#endif
			throw "Unexpected EOF";
		}
	}
}
