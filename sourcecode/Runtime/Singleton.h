#pragma once

namespace Nom
{
	namespace Runtime
	{
		template<class T>
		class Singleton
		{
		public:
			static T& Instance() {
				static T instance; return instance;
			}
		};
	}
}
