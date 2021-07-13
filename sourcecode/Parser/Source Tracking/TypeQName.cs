using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Antlr4.Runtime;

namespace Nom.Parser
{
    public class TypeQName : AQName<IArgIdentifier<Identifier, IType>>
    {
        public TypeQName(bool fromRootOnly, IEnumerable<IArgIdentifier<Identifier, IType>> ids, ISourceSpan locs=null) : base(fromRootOnly, locs, ids.ToArray())
        {
        }
        public TypeQName(bool fromRootOnly, ISourceSpan locs = null, params IArgIdentifier<Identifier, IType>[] ids) : base(fromRootOnly, locs, ids)
        {
        }

    }
  
}
