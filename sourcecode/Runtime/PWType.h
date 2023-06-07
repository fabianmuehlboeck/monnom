#pragma once
#include "PWrapper.h"
#include "Defs.h"
#include "NomBuilder.h"

namespace Nom
{
	namespace Runtime
	{
		class PWNomType;
		class PWCastFunction;
		class PWType : public PWrapper
		{
		public:
			PWType(llvm::Value* wrapped) :PWrapper(wrapped)
			{

			}
			void InitializeType(NomBuilder& builder, TypeKind kind, llvm::Value* hash, PWNomType nomtypeptr, PWCastFunction castFun);
			llvm::Value* ReadKind(NomBuilder& builder);
			llvm::Value* ReadHash(NomBuilder& builder);
			PWCastFunction ReadCastFun(NomBuilder& builder);

		};
	}
}