using System;
using System.Linq;
using System.Collections.Generic;
using System.Text;
using Nom.Parser;
using Nom.Language;

namespace Nom.TypeChecker
{
    public abstract class ATDAstNode : ITDAstNode
    {
        public ATDAstNode(ILocationData loc)
        {
            LocationData = loc;
        }

        public ILocationData LocationData { get; }

        public abstract R VisitAst<S, R>(ITDAstNodeVisitor<S, R> visitor, S state = default(S));
    }
}
