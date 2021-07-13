using System;
using System.Collections.Generic;
using System.Text;

namespace Nom.TypeChecker
{
    public interface ICallableBody
    {
        IEnumerable<IRegister> ArgumentRegisters { get; }
        IEnumerable<IInstruction> Instructions { get; }
    }

    public interface IInstanceCallableBody : ICallableBody
    {
        IRegister ThisRegister { get; }
    }

    public class ConstructorBody : IInstanceCallableBody
    {
        private ConstructorBody()
        {

        }

        public IRegister ThisRegister { get; }

        public IEnumerable<IRegister> ArgumentRegisters { get; }

        public IEnumerable<IInstruction> Instructions { get; }

    }
}
