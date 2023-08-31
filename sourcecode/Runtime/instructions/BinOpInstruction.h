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
			RTValuePtr CompileLeftInt(NomBuilder& builder, CompileEnv* env, size_t lineno, llvm::Value* left);
			RTValuePtr CompileLeftFloat(NomBuilder& builder, CompileEnv* env, size_t lineno, llvm::Value* left);
			RTValuePtr CompileLeftBool(NomBuilder& builder, CompileEnv* env, size_t lineno, llvm::Value* left);
			RTValuePtr CompileLeftPointer(NomBuilder& builder, CompileEnv* env, size_t lineno, llvm::Value* left);
			RTValuePtr CompileIntInt(NomBuilder& builder, CompileEnv* env, size_t lineno, llvm::Value* left, llvm::Value* right);
			RTValuePtr CompileFloatFloat(NomBuilder& builder, CompileEnv* env, size_t lineno, llvm::Value* left, llvm::Value* right);
			RTValuePtr CompileBoolBool(NomBuilder& builder, CompileEnv* env, size_t lineno, llvm::Value* left, llvm::Value* right);
		public:
			const BinaryOperation Operation;
			const RegIndex Left;
			const RegIndex Right;
			BinOpInstruction(const BinaryOperation op, const RegIndex left, const RegIndex right, const RegIndex reg);
			virtual ~BinOpInstruction() override;
			virtual void Compile(NomBuilder& builder, CompileEnv* env, size_t lineno) override;

			// Inherited via NomValueInstruction
			virtual void Print(bool resolve = false) override;

			virtual void FillConstantDependencies(NOM_CONSTANT_DEPENCENCY_CONTAINER& result) override;
		};


	}
}
