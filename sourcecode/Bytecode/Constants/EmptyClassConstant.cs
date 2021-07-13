using System;
using System.Collections.Generic;
using System.IO;
using System.Text;

namespace Nom.Bytecode
{
    class EmptyClassConstant : IClassConstant
    {
        private EmptyClassConstant() { }
        public static EmptyClassConstant Instance { get; } = new EmptyClassConstant();

        public ulong ConstantID => 0;

        public ConstantType Type => throw new InternalException("Should not inspect empty constant");

        public IEnumerable<IConstant> Dependencies
        {
            get { yield break; }
        }

        public IConstantRef<StringConstant> LibraryNameConstant => throw new InternalException("Should not inspect empty constant");

        public IConstantRef<StringConstant> NameConstant => throw new InternalException("Should not inspect empty constant");

        public string QualifiedName => throw new InternalException("Should not inspect empty constant");

        public void Emit(Stream ws)
        {
            throw new InternalException("Should not emit empty constant");
        }
    }
}
