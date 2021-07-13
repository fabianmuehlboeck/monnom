#pragma once

#include "CompileEnv.h"
#include "Defs.h"
#include "NomBuilder.h"

namespace Nom
{
	namespace Runtime
	{

		enum class OpCode : unsigned char {
            Noop,
            PhiNode,
            PhiEntry,
            Return,
            ReturnVoid,
            Argument,
            TypeArgument,
            New,
            LoadStringConstant,
            LoadIntConstant,
            LoadFloatConstant,
            LoadBoolConstant,
            LoadNullConstant,
            InvokeCheckedInstance,
            CallCheckedInterface,
            CallCheckedStatic,
            CallConstructor,
            CallDispatchFixed,
            CallDispatchStatic,
            CallDispatchJoin,
            CallDispatchBest,
            CallFinal,
            CreateClosure,
            ConstructStruct,
            WriteField,
            ReadField,
            Cast,
            Branch,
            CondBranch,
            BinOp,
            UnaryOp,
            Debug,
            RTCmd,
            Error
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