#pragma once
#include "PWInt.h"
#include "RTValuePtr.h"
#include "PWSubstStack.h"

namespace Nom
{
	namespace Runtime
	{
		class RTType;
		template<typename T>
		class RTPWType;

		class RTTypePtr
		{
		protected:
			RTType* type;
		public:
			RTTypePtr(RTType* _type = nullptr) : type(_type)
			{

			}

			RTType* operator->() const
			{
				return type;
			}
			operator RTType* () const
			{
				return type;
			}

			virtual PWBool CastTo(NomBuilder& builder, RTValuePtr value, PWSubstStack substitutions, bool optimistic = false) = 0;
			virtual PWBool InstanceOf(NomBuilder& builder, RTValuePtr value, PWSubstStack substitutions, bool optimistic = false) = 0;
			virtual PWBool SubtypeOf(NomBuilder& builder, RTTypePtr type, PWSubstStack leftSubst, PWSubstStack rightSubst, bool optimistic = false) = 0;
			virtual PWBool TypeEq(NomBuilder& builder, RTTypePtr type, PWSubstStack leftSubst, PWSubstStack rightSubst, bool optimistic = false) = 0;
		};

		template<typename T>
		class RTPWTypePtr : RTTypePtr
		{
		public:
			RTPWTypePtr(RTPWType<T>* _type = nullptr) : RTTypePtr(_type)
			{

			}
			RTPWType<T>* operator->() const
			{
				return static_cast<RTPWType<T>*>(type);
			}
			operator RTPWType<T>* () const
			{
				return static_cast<RTPWType<T>*>(type);
			}


			virtual PWBool CastTo(NomBuilder& builder, RTValuePtr value, PWSubstStack substitutions, bool optimistic = false) override
			{
				this->operator Nom::Runtime::RTPWType<T> *()->
			}
			virtual PWBool InstanceOf(NomBuilder& builder, RTValuePtr value, PWSubstStack substitutions, bool optimistic = false) override
			{}
			virtual PWBool SubtypeOf(NomBuilder& builder, RTTypePtr type, PWSubstStack leftSubst, PWSubstStack rightSubst, bool optimistic = false) override
			{}
			virtual PWBool TypeEq(NomBuilder& builder, RTTypePtr type, PWSubstStack leftSubst, PWSubstStack rightSubst, bool optimistic = false) override
			{}

		};
	}
}