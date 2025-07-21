#pragma once
#include "../instructions/NomValueInstruction.h"
#include "../Intermediate_Representation/NomValue.h"

namespace Nom
{
	namespace Runtime
	{
		//enum of operations
		enum class BinaryOperation : unsigned char
		{
			Equals,
			RefEquals,
			Add,
			Subtract,
			Multiply,
			Divide,
			Power,
			Mod,
			Concat,
			And,
			Or,
			BitAND,
			BitOR,
			BitXOR,
			ShiftLeft,
			ShiftRight,
			LessThan,
			GreaterThan,
			LessOrEqualTo,
			GreaterOrEqualTo
		};

		const char* GetBinOpName(BinaryOperation op);

		class BinOpInstruction : public NomValueInstruction
		{
		private:
			llvm::Value* CompileIntIntLLVM(NomBuilder& builder, CompileEnv* env, int lineno, llvm::Value* left, llvm::Value* right, llvm::Value** resultTag);
			llvm::Value* CompileFloatFloatLLVM(NomBuilder& builder, CompileEnv* env, int lineno, llvm::Value* left, llvm::Value* right, llvm::Value** resultTag);
			llvm::Value* CompileBoolBoolLLVM(NomBuilder& builder, CompileEnv* env, int lineno, llvm::Value* left, llvm::Value* right, llvm::Value** resultTag);
			NomValue CompileBoolBool(NomBuilder& builder, CompileEnv* env, int lineno, llvm::Value* left, llvm::Value* right);
			NomTypeRef NomReturnType(NomTypeRef leftTy, NomTypeRef rightTy);
		public:
			const BinaryOperation Operation;
			const RegIndex Left;
			const RegIndex Right;
			BinOpInstruction(const BinaryOperation op, const RegIndex left, const RegIndex right, const RegIndex reg);
			virtual ~BinOpInstruction();
			virtual void Compile(NomBuilder& builder, CompileEnv* env, int lineno) override;

			// Inherited via NomValueInstruction
			virtual void Print(bool resolve = false) override;

			virtual void FillConstantDependencies(NOM_CONSTANT_DEPENCENCY_CONTAINER& result) override;
		};


	}
}
