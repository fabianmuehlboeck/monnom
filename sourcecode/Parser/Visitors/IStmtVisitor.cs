using System;
using System.Collections.Generic;
using System.Text;

namespace Nom.Parser
{
    public partial interface IStmtVisitor<in S, out R>
    {
    }
}
