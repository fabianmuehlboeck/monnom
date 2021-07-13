using System;
using System.Collections.Generic;
using System.Text;

namespace Nom.Parser
{
    public interface IAnnotatedAstNode<T> : IAstNode
    {
        T Annotation
        { get; set; }
    }
}
