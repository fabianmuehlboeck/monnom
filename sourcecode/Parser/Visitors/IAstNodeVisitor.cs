using System;
using System.Collections.Generic;
using System.Text;

namespace Nom.Parser
{
    public partial interface IAstNodeVisitor<in S, out R> : IExprVisitor<S, R>, ITypeVisitor<S, R>, INamedTypeDefVisitor<S, R>, IStmtVisitor<S, R>
    {
    }
}
