#pragma once
#include "llvm/ADT/ArrayRef.h"

namespace Nom
{
	namespace Runtime
	{
		enum class ParameterVariance : unsigned char 
		{
			Invariant=0,
			Covariant=1,
			Contravariant=2
		};

		class NomClassType;
		class NomTopType;
		class NomTypeVar;
		class NomBottomType;
		class NomType;
		class NomDynamicType;
		class NomMaybeType;

		using NomTypeRef = const NomType *;
		using NomDynamicTypeRef = const NomDynamicType*;
		using NomBottomTypeRef = const NomBottomType*;
		using NomClassTypeRef = const NomClassType *;
		using NomTopTypeRef = const NomTopType*;
		using NomTypeVarRef = const NomTypeVar *;
		using NomMaybeTypeRef = const NomMaybeType*;
		using TypeList = llvm::ArrayRef<NomTypeRef>;
	}
}