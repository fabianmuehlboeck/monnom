using System;
using System.Collections.Generic;
using System.IO;
using System.Text;
using System.Linq;

namespace Nom.Bytecode
{
    public abstract class ACallInstruction : AInstruction
    {
        public IEnumerable<int> ArgumentRegisters { get; }
        public IConstantRef<TypeListConstant> TypeArguments { get; }
        public ACallInstruction(IConstantRef<TypeListConstant> typeArguments, IEnumerable<int> argumentRegisters, OpCode opCode) : base(opCode)
        {
            ArgumentRegisters = argumentRegisters;
            TypeArguments = typeArguments;
        }
        public override ulong InstructionCount => (ulong)(ArgumentRegisters.LongCount() + 1);
        public override void WriteByteCode(Stream ws)
        {
            foreach (int reg in ArgumentRegisters)
            {
                ws.WriteByte((byte)OpCode.Argument);
                ws.WriteValue(reg);
            }
            base.WriteByteCode(ws);
        }

        public static IInstruction Read(Stream s, IReadConstantSource rcs, OpCode opcode)
        {
            List<int> argRegs = new List<int>();
            while(true)
            { 
                switch(opcode)
                {
                    case OpCode.Argument:
                        argRegs.Add(s.ReadInt());
                        break;
                    case OpCode.CallCheckedStatic:
                        return CallCheckedStaticMethodInstruction.Read(s, rcs, argRegs);
                    case OpCode.CallDispatchBest:
                        return CallExpandoMethodInstruction.Read(s, rcs, argRegs);
                    case OpCode.CallConstructor:
                        return CallCheckedConstructorInstruction.Read(s, rcs, argRegs);
                    case OpCode.InvokeCheckedInstance:
                        return CallCheckedInstanceMethodInstruction.Read(s, rcs, argRegs);
                    case OpCode.ConstructStruct:
                        return ConstructStructInstruction.Read(s, rcs, argRegs);
                    case OpCode.CreateClosure:
                        return CreateClosureInstruction.Read(s, rcs, argRegs);
                    default:
                        throw new NomBytecodeException("Malformed Bytecode!");
                }
                opcode = (OpCode)s.ReadActualByte();
            }
        }
    }
}
