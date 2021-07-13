using System;
using System.Collections.Generic;
using System.Text;

namespace Nom.Parser
{
    public class DeclIdentifier : AArgIdentifier<Identifier, TypeArgDecl>
    {
        public DeclIdentifier(Identifier name, IEnumerable<TypeArgDecl> args, ISourceSpan locs=null) : base(name, args, locs)
        {
        }
    }
}
