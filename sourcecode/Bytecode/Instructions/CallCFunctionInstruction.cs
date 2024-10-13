using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Nom.Bytecode
{
    public class CallCFunctionInstruction : ACallInstruction
    {
        public IConstantRef<CFunctionConstant> CFunction { get; }
        public int ResultRegister { get; }
        public CallCFunctionInstruction(IConstantRef<CFunctionConstant> cFunction, IConstantRef<TypeListConstant> typeArguments, IEnumerable<int> argumentRegisters, int resultRegister) : base(typeArguments, argumentRegisters, OpCode.CallC)
        {
            CFunction = cFunction;
            ResultRegister = resultRegister;
        }

        protected override void WriteArguments(Stream ws)
        {
            ws.WriteValue(ResultRegister);
            ws.WriteValue(CFunction.ConstantID);
            ws.WriteValue(TypeArguments.ConstantID);
        }
    }
}
