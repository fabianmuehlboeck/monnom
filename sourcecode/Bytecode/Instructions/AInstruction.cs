using System;
using System.Collections.Generic;
using System.IO;
using System.Text;

namespace Nom.Bytecode
{
    public abstract class AInstruction : IInstruction
    {

        public AInstruction(OpCode opCode)
        {
            OpCode = opCode;
        }

        public OpCode OpCode { get; }

        public virtual UInt64 InstructionCount { get; } = 1;

        public virtual void WriteByteCode(Stream ws)
        {
            ws.WriteByte((byte)OpCode);
            WriteArguments(ws);
        }
        protected abstract void WriteArguments(Stream ws);

        public static IInstruction ReadInstruction(Stream s, IReadConstantSource rcs) 
        {
            OpCode opcode = (OpCode)s.ReadActualByte();
            switch (opcode)
            {
                case OpCode.Noop:
                    throw new NotImplementedException();//unused
                case OpCode.PhiNode:
                    return PhiNode.Read(s, rcs);
                case OpCode.Return:
                    return ReturnInstruction.Read(s, rcs);
                case OpCode.ReturnVoid:
                    return ReturnVoidInstruction.Read(s, rcs);
                case OpCode.Argument:
                    return ACallInstruction.Read(s, rcs, opcode);
                case OpCode.LoadStringConstant:
                    return LoadStringConstantInstruction.Read(s, rcs);
                case OpCode.LoadIntConstant:
                    return LoadIntConstantInstruction.Read(s, rcs);
                case OpCode.LoadFloatConstant:
                    return LoadFloatConstantInstruction.Read(s, rcs);
                case OpCode.LoadBoolConstant:
                    return LoadBoolConstantInstruction.Read(s, rcs);
                case OpCode.LoadNullConstant:
                    return LoadNullConstantInstruction.Read(s, rcs);
                case OpCode.InvokeCheckedInstance:
                    return ACallInstruction.Read(s, rcs, opcode);
                case OpCode.CallCheckedStatic:
                    return ACallInstruction.Read(s, rcs,opcode);
                case OpCode.CallConstructor:
                    return ACallInstruction.Read(s, rcs, opcode);
                case OpCode.CallDispatchBest:
                    return ACallInstruction.Read(s, rcs, opcode);
                case OpCode.CallFinal:
                    return ACallInstruction.Read(s, rcs, opcode);//unused
                case OpCode.CreateClosure:
                    return ACallInstruction.Read(s, rcs, opcode);
                case OpCode.ConstructStruct:
                    return ACallInstruction.Read(s, rcs, opcode);
                case OpCode.WriteField:
                    return WriteFieldInstruction.Read(s, rcs);
                case OpCode.ReadField:
                    return ReadFieldInstruction.Read(s, rcs);
                case OpCode.Cast:
                    return CastInstruction.Read(s, rcs);
                case OpCode.Branch:
                    return BranchInstruction.Read(s, rcs);
                case OpCode.CondBranch:
                    return CondBranchInstruction.Read(s, rcs);
                case OpCode.BinOp:
                    return BinOpInstruction.Read(s, rcs);
                case OpCode.UnaryOp:
                    return UnaryOpInstruction.Read(s, rcs);
                case OpCode.Debug:
                    return DebugInstruction.Read(s, rcs);
                case OpCode.RTCmd:
                    return RuntimeCmdInstruction.Read(s, rcs);
                case OpCode.Error:
                    return ErrorInstruction.Read(s, rcs);
                default:
                    throw new NomBytecodeException("Invalid opcode!");
            }
        }
    }
}
