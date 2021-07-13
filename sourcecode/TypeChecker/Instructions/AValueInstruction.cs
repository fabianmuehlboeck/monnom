using System;
using System.Collections.Generic;
using System.Text;

namespace Nom.TypeChecker
{
    public abstract class AValueInstruction : AInstruction
    {
        public IRegister Register
        {
            get;
        }
        public AValueInstruction(IRegister register)
        {
            Register = register;
        }
        public override IEnumerable<IRegister> WriteRegisters
        {
            get
            {
                yield return Register;
            }
        }
    }
}
