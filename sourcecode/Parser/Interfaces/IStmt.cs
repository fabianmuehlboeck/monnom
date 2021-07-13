using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Collections;

namespace Nom.Parser
{
    public interface IStmt : IAstNode
    {
        IEnumerable<Identifier> UsedIdentifiers
        {
            get;
        }


        Ret Visit<Arg, Ret>(IStmtVisitor<Arg, Ret> visitor, Arg arg=default(Arg));

        Block AsBlock();
    }
}
