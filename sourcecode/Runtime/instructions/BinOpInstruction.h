#pragma once
#include "../NomValueInstruction.h"
#include "../NomValue.h"

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
			NomValue CompileLeftInt(NomBuilder& builder, CompileEnv* env, int lineno, llvm::Value* left);
			NomValue CompileLeftFloat(NomBuilder& builder, CompileEnv* env, int lineno, llvm::Value* left);
			NomValue CompileLeftBool(NomBuilder& builder, CompileEnv* env, int lineno, llvm::Value* left);
			NomValue CompileLeftPointer(NomBuilder& builder, CompileEnv* env, int lineno, llvm::Value* left);
			NomValue CompileIntInt(NomBuilder& builder, CompileEnv* env, int lineno, llvm::Value* left, llvm::Value* right);
			NomValue CompileFloatFloat(NomBuilder& builder, CompileEnv* env, int lineno, llvm::Value* left, llvm::Value* right);
			NomValue CompileBoolBool(NomBuilder& builder, CompileEnv* env, int lineno, llvm::Value* left, llvm::Value* right);
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
