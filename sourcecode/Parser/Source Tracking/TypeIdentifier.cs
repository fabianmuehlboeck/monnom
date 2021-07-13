using System;
using System.Collections.Generic;
using System.Text;
using Antlr4.Runtime;
using System.Linq;

namespace Nom.Parser
{
    public class TypeIdentifier : AArgIdentifier<Identifier, IType>
    {
        public TypeIdentifier(Identifier name, IEnumerable<IType> args, ISourceSpan locs = null) : base(name, args, locs)
        {
        }

    }

}
