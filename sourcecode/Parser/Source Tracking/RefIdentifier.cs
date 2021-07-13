using System;
using System.Collections.Generic;
using System.Text;
using System.Linq;
using Antlr4.Runtime;

namespace Nom.Parser
{
    public class RefIdentifier :  AArgIdentifier<Identifier, IType>
    {
        public RefIdentifier(Identifier name, IEnumerable<IType> args, ISourceSpan locs = null) : base(name, args, locs)
        {
            
        }

    }

}
