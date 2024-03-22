#pragma once

#include "NomTypeDecls.h"

namespace Nom
{
	namespace Runtime
	{

		template<typename T>
		class NomInstantiationRef
		{
		public:
			const T * Elem;
			TypeList TypeArgs;
			NomInstantiationRef() : Elem(nullptr)
			{

			}
			NomInstantiationRef(const T * elem, const TypeList typeArgs) : Elem(elem), TypeArgs(typeArgs)
			{
				if (typeArgs.size() != elem->GetTypeParametersCount())
				{
					throw new std::exception();
				}
			}
			NomInstantiationRef(const NomInstantiationRef<T>& other) = default;
			NomInstantiationRef(NomInstantiationRef<T>&& other) = default;
			bool HasElem() const {
				return Elem != nullptr;
			}
			NomInstantiationRef& operator=(const NomInstantiationRef& other)
			{
				if (this != &other)
				{
					this->Elem = other.Elem;
					this->TypeArgs = other.TypeArgs;
				}
				return *this;
			}
		};
	}
}
