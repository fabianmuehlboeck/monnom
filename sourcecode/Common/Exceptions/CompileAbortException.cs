using System;
using System.Collections.Generic;
using System.Text;

namespace Nom
{
    public class CompileAbortException : NomException
    {
        public CompileAbortException(String message="Compilation aborted!", Exception innerException = null) :base(message, innerException)
        {

        }
        public CompileAbortException(String message, Exception innerException, params IReference[] references) : base(message, innerException, references)
        {

        }
        public CompileAbortException(String message, params IReference[] references) : base(message, references)
        {

        }
    }
}
