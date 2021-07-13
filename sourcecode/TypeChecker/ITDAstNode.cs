using System;
using System.Collections.Generic;
using System.Text;
using Nom.Parser;

namespace Nom.TypeChecker
{
    public interface ITDAstNode
    {
        R VisitAst<S, R>(ITDAstNodeVisitor<S, R> visitor, S state = default(S));
    }

    public partial interface ITDAstNodeVisitor<in S, out R> : IInstructionVisitor<S, R>
    {

    }
}
