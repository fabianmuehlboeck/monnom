using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Nom.Parser
{
    public interface IAstNode : IPositionalReference, IPrettyPrintable, IPositionedElement
    {

        R VisitAstNode<S, R>(IAstNodeVisitor<S, R> visitor, S state);
    }
}
