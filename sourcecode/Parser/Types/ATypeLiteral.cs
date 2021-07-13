using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Nom.Parser
{
    public abstract class ATypeLiteral : AType
    {
        protected ATypeLiteral(ISourceSpan locs) : base(locs)
        {
        }

    }
}
