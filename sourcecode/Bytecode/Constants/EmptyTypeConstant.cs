using System;
using System.Collections.Generic;
using System.IO;
using System.Text;
using Nom.Language;

namespace Nom.Bytecode
{
    public class EmptyTypeConstant : ITypeConstant
    {
        public IType Value => throw new NotImplementedException();

        public ulong ConstantID => 0;

        public ConstantType Type => throw new InternalException("Should not inspect empty constant");

        public IEnumerable<IConstant> Dependencies
        {
            get { yield break; }
        }

        public void Emit(Stream ws)
        {
            throw new InternalException("Should not emit empty constant");
        }
    }
}
