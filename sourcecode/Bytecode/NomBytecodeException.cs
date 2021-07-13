using System;
using System.Collections.Generic;
using System.Text;

namespace Nom.Bytecode
{
    class NomBytecodeException : NomException
    {
        public NomBytecodeException(string message, ISourceSpan span) : base(message, span)
        {
        }

        public NomBytecodeException(string message = "Unknown Bytecode Exception", ISourceLocs sourceLocation = null) : base(message, sourceLocation)
        {
        }

        public NomBytecodeException(string message, params IReference[] references) : base(message, references)
        {
        }

        public NomBytecodeException(string message, Exception innerException, ISourceSpan span = null) : base(message, innerException, span)
        {
        }

        public NomBytecodeException(string message, Exception innerException, ISourceLocs sourceLocation) : base(message, innerException, sourceLocation)
        {
        }

        public NomBytecodeException(string message, Exception innerException, params IReference[] references) : base(message, innerException, references)
        {
        }
    }
}
