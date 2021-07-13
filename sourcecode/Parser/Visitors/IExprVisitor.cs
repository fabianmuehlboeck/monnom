using System;
using System.Collections.Generic;
using System.Text;

namespace Nom.Parser
{
    public partial interface IExprVisitor<in S, out R> : IDefaultExprVisitor<S,R>
    {

    }
}
