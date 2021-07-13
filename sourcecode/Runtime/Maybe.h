#pragma once

namespace Nom
{
	namespace Runtime
	{
		template<typename T>
		class Maybe
		{
		private:
			bool hasValue;
			char data[sizeof(T)];
		public:
			Maybe();
			Maybe(T &&t);
			~Maybe();
		};


	}
}
