#pragma once
#include "RTTypePtr.h"
#include "NomTypeDecls.h"

namespace Nom
{
	namespace Runtime
	{
		class PWType;
		class RTType
		{
		protected:
			RTType()
			{

			}
		public:
			virtual ~RTType() {}
			virtual PWType GetPWType() const = 0;
			virtual NomTypeRef GetNomType() const = 0;
		};

		template<typename T>
		class RTPWType : RTType
		{
		protected:
			T Type;
			RTPWType(T _type) : RTType(), Type(_type)
			{

			}

		public:
			virtual ~RTPWType() {}
			virtual PWType GetPWType() const override
			{
				return Type;
			}
			T GetFullPWType() const
			{
				return Type;
			}
		};

		template<typename T, typename NT>
		class RTPWNTType : RTPWType<T>
		{
		protected:
			NT NomType;

			RTPWNTType(T _type, NT _nomType) : RTPWType(_type), NomType(_nomType)
			{

			}
		public:
			virtual ~RTPWNTType() {}
			virtual NomTypeRef GetNomType() const override
			{
				return NomType;
			}
			virtual NT GetFullNomType() const override
			{
				return NomType;
			}
		};

	}
}