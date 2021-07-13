using System;
using System.Collections.Generic;
using System.Text;
using System.IO;

namespace Nom.Bytecode
{
    public enum OpCode : byte
    {
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
    }
    public interface IInstruction
    {
        UInt64 InstructionCount { get; }
        OpCode OpCode { get; }
        void WriteByteCode(Stream ws);
    }
}
