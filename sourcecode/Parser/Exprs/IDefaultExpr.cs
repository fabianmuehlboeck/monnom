using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Nom.Parser
{
    public interface IDefaultExpr:IExpr
    {
        R Visit<S, R>(IDefaultExprVisitor<S, R> visitor, S state = default(S));
    }

    public partial interface IDefaultExprVisitor<in S, out R>
    {

    }
}
