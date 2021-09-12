#pragma once

#include "Defs.h"
#include "NomTypeDecls.h"
#include "CompileEnv.h"
#include "NomBuilder.h"
#include "NomValue.h"

namespace Nom
{
	namespace Runtime
	{
		enum class TypeFlags : unsigned char { Reference = 0, FloatZero = 1, Float = 2, Integer = 3 };

		llvm::Value* EnsureUnpackedInt(NomBuilder &builder, CompileEnv* env, NomValue& val);
		llvm::Value* EnsureUnpackedFloat(NomBuilder &builder, CompileEnv* env, NomValue& val);
		llvm::Value* EnsureUnpackedBool(NomBuilder &builder, CompileEnv* env, NomValue& val);
		NomValue EnsurePacked(NomBuilder& builder, NomValue& val);
		llvm::Value* EnsurePacked(NomBuilder& builder, llvm::Value* val);
		llvm::Value* EnsureType(NomBuilder& builder, CompileEnv* env, NomValue& val, NomTypeRef type, llvm::Type* expected);
		llvm::Value* EnsureType(NomBuilder& builder, CompileEnv* env, NomValue& val, NomTypeRef type);

		llvm::Value* WrapAsPointer(NomBuilder& builder, llvm::Value* val);

		llvm::Value* EnsurePackedUnpacked(NomBuilder& builder, llvm::Value* val, llvm::Type* type);

		llvm::Value* PackInt(NomBuilder& builder, llvm::Value* intval);
		llvm::Value* UnpackInt(NomBuilder& builder, NomValue value);
		NomValue UnpackPosMaskedInt(NomBuilder& builder, llvm::Value* refAsInt);
		NomValue UnpackNegMaskedInt(NomBuilder& builder, llvm::Value* refAsInt);
		llvm::Value* PackFloat(NomBuilder& builder, llvm::Value* floatval);
		llvm::Value* UnpackFloat(NomBuilder& builder, NomValue value);

		NomValue UnpackMaskedFloat(NomBuilder& builder, llvm::Value* refAsInt);

		llvm::Value* PackBool(NomBuilder& builder, llvm::Value* b);
		llvm::Value* UnpackBool(NomBuilder& builder, llvm::Value* b, bool verify=false);

	}
}