using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Nom.Parser
{
    public class NameResolutionException : ListableException
    {
        public NameResolutionException(String message = "Unknown Name Resolution Error", ISourceLocs locs=null)
            : base(message, locs)
        {
        }

        public NameResolutionException(String message, Exception innerException, ISourceLocs locs = null)
            : base(message, innerException, locs)
        {
        }
    }
}
