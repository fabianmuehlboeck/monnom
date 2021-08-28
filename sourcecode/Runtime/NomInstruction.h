#pragma once

#include "CompileEnv.h"
#include "Defs.h"
#include "NomBuilder.h"

namespace Nom
{
	namespace Runtime
	{

		enum class OpCode : unsigned char {
            Noop = 0,
            PhiNode = 1,
            Return = 11,
            ReturnVoid = 12,
            Argument = 13,
            EnsureCheckedMethod = 14,
            EnsureDynamicMethod = 15,
            LoadStringConstant = 21,
            LoadIntConstant = 22,
            LoadFloatConstant = 23,
            LoadBoolConstant = 24,
            LoadNullConstant = 25,
            CallCheckedStatic = 101,
            CallConstructor = 102,
            CreateClosure = 103,
            ConstructStruct = 104,
            InvokeCheckedInstance = 111,
            CallDispatchBest = 112,
            CallFinal = 113,
            WriteField = 51,
            ReadField = 52,
            Cast = 61,
            Branch = 31,
            CondBranch = 32,
            UnaryOp = 41,
            BinOp = 42,
            Debug = 121,
            RTCmd = 122,
            Error = 123
		};

		class NomInstruction
		{
		private:
			OpCode opcode;
		public:

			OpCode GetOpCode()
			{
				return opcode;
			}

			virtual void Compile(NomBuilder &builder, CompileEnv* env, int lineno) = 0;
			virtual void Print(bool resolve = false) = 0;

			NomInstruction(OpCode opcode):opcode(opcode)
			{

			}
			virtual ~NomInstruction();

			virtual void FillConstantDependencies(NOM_CONSTANT_DEPENCENCY_CONTAINER & result) = 0;
		};
	}
}