using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Nom
{
    public abstract class ListableException:NomException
    {
        public ListableException(String message, Exception innerException, ISourceSpan span=null):base(message,innerException, span)
        {

        }
        public ListableException(String message, Exception innerException, ISourceLocs sourceLocation) :
            base(message,innerException, sourceLocation)
        {
        }

        public ListableException(String message = "Unknown Compiler Exception", ISourceSpan span = null) :
            base(message, span)
        {
        }
        public ListableException(String message = "Unknown Compiler Exception", ISourceLocs sourceLocation = null) :
            base(message,sourceLocation)
        {
        }

        public ListableException(String message, Exception innerException, params IReference[] references) : base(message, innerException, references)
        {

        }
        public ListableException(String message, params IReference[] references) : base(message, references)
        {

        }
    }
}
