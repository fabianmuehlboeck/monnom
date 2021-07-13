using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Nom.Parser
{
    public class ParseException:NomException
    {
        public ParseException(String message = "Unknown Parsing Error", ISourceLocs locs=null):base(message, locs)
        {

        }
        public ParseException(String message, Exception innerException, ISourceLocs locs=null):base(message, innerException,locs)
        {
        }
    }
}
