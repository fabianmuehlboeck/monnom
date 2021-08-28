#pragma once

#include "llvm/IR/Type.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Constants.h"
#include "Context.h"
#include "RecursionBuffer.h"
#include "Defs.h"
#include "CompileEnv.h"
#include "AvailableExternally.h"
#include "NomTypeDecls.h"
#include "NomBuilder.h"

namespace Nom
{
	namespace Runtime
	{
		class NomNamed;
		class NomSubstitutionContext;

		enum class TypeReferenceType { Reference, UnpackedInteger, UnpackedFloat, UnpackedBool };

		NomTypeRef JoinTypes(NomTypeRef left, NomTypeRef right);

		inline static std::string TypeReferenceTypeString(const TypeReferenceType& trt)
		{
			switch (trt)
			{
			case TypeReferenceType::Reference:
				return "R";
			case TypeReferenceType::UnpackedInteger:
				return "I";
			case TypeReferenceType::UnpackedFloat:
				return "F";
			case TypeReferenceType::UnpackedBool:
				return "B";
			}
		}

		class NomType : public AvailableExternally<llvm::Constant>
		{
		public:
			static bool PointwiseSubtype(TypeList l, TypeList r, bool optimistic=false);

			static const NomTopType * const Anything;
			static const NomBottomType* const Nothing;
			static const NomDynamicType* const Dynamic;
			static NomTypeRef AnythingRef;
			static NomTypeRef NothingRef;
			static NomTypeRef DynamicRef;
			NomType();
			virtual ~NomType();

			/*bool IsSubtype(NomTypeRef other) const
			{
				return GetRTType().IsSubtype(other->GetRTType());
			}*/

			virtual bool ContainsVariables() const = 0;
			virtual bool ContainsVariableIndex(int index) const = 0;
			virtual llvm::Value * GenerateRTInstantiation(NomBuilder &builder, CompileEnv* env) const = 0;

			virtual size_t GetHashCode() const = 0;
			virtual TypeReferenceType GetTypeReferenceType() const = 0;

			virtual bool IsSubtype(NomTypeRef other, bool optimistic = false) const = 0;
			virtual bool IsSubtype(NomBottomTypeRef other, bool optimistic = false) const = 0;
			virtual bool IsSubtype(NomDynamicTypeRef other, bool optimistic = false) const = 0;
			virtual bool IsSubtype(NomClassTypeRef other, bool optimistic = false) const = 0;
			virtual bool IsSubtype(NomTopTypeRef other, bool optimistic = false) const = 0;
			virtual bool IsSubtype(NomTypeVarRef other, bool optimistic = false) const = 0;
			virtual bool IsSubtype(NomMaybeTypeRef other, bool optimistic = false) const = 0;
			virtual bool IsSupertype(NomTypeRef other, bool optimistic = false) const = 0;
			virtual bool IsSupertype(NomDynamicTypeRef other, bool optimistic = false) const = 0;
			virtual bool IsSupertype(NomBottomTypeRef other, bool optimistic = false) const = 0;
			virtual bool IsSupertype(NomClassTypeRef other, bool optimistic = false) const = 0;
			virtual bool IsSupertype(NomTopTypeRef other, bool optimistic = false) const = 0;
			virtual bool IsSupertype(NomTypeVarRef other, bool optimistic = false) const = 0;
			virtual bool IsSupertype(NomMaybeTypeRef other, bool optimistic = false) const = 0;

			virtual bool IsDisjoint(NomTypeRef other) const = 0;
			virtual bool IsDisjoint(NomBottomTypeRef other) const = 0;
			virtual bool IsDisjoint(NomClassTypeRef other) const = 0;
			virtual bool IsDisjoint(NomTopTypeRef other) const = 0;
			virtual bool IsDisjoint(NomTypeVarRef other) const = 0;
			virtual bool IsDisjoint(NomMaybeTypeRef other) const = 0;

			//Returns true iff type can represent a primitive
			virtual bool PossiblyPrimitive() const = 0;
			//Returns true iff type can represent a primitive, but doesn't have to
			virtual bool UncertainlyPrimitive() const = 0;

			virtual NomTypeRef SubstituteSubtyping(const NomSubstitutionContext* context) const = 0;

			virtual llvm::Type * GetLLVMType() const = 0;
			virtual const std::string GetSymbolRep() const = 0;


			virtual TypeKind GetKind() const = 0;

			virtual intptr_t GetRTElement() const = 0;
			virtual NomClassTypeRef GetClassInstantiation(const NomNamed *named) const = 0;
		};

		struct NomTypeRefHash
		{
			std::size_t operator()(const NomTypeRef & s) const noexcept
			{
				return s->GetHashCode();
			}
		};

		struct NomTypeRefEquality
		{
			bool operator()(const NomTypeRef &lhs, const NomTypeRef &rhs) const
			{
				return lhs == rhs;
			}

		};

		struct NomTypeRefArrayRefHash
		{
			std::size_t operator()(const llvm::ArrayRef<NomTypeRef> & s) const noexcept
			{
				NomTypeRefHash ntrh;
				size_t ret = 1;
				for (auto ntr : s)
				{
					ret = ret * 31 + ntrh(ntr);
				}
				return ret;
			}
		};

		struct NomRTHashArrHash
		{
			std::size_t operator()(const size_t* hashArr, int arrsize) const noexcept
			{
				size_t ret = 1;
				for (int i = -1; i >= -arrsize; i--)
				{
					ret = ret * 31 + hashArr[i];
				}
				return ret;
			}
		};

		struct NomTypeRefArrayRefEquality
		{
			bool operator()(const llvm::ArrayRef < NomTypeRef> &lhs, const llvm::ArrayRef < NomTypeRef> &rhs) const
			{
				if (lhs.size() != rhs.size()) {
					return false;
				}
				NomTypeRefEquality ntreq;
				for (size_t i = lhs.size(); i > 0;) {
					i--;
					if (!ntreq(lhs[i], rhs[i])) {
						return false;
					}
				}
				return true;
			}

		};
	}
}


