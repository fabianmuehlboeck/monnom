using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Nom
{
    public class InternalException : NomException
    {
        public InternalException(String message, Exception innerException, ISourceLocs sourceLocation = null) : base(message, innerException, sourceLocation)
        {

        }
        public InternalException(String message, ISourceLocs sourceLocation = null) : base(message, null, sourceLocation)
        {

        }

        public InternalException(String message, Exception innerException, params IReference[] references) : base(message, innerException, references)
        {

        }
        public InternalException(String message, params IReference[] references) : base(message, references)
        {

        }
    }
}
