using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Nom.Parser
{
    public interface IType : IAnnotatedAstNode<Language.IType>, IPosNamedReference
    {

        T Visit<T>(TypeVisitor<T> visitor);

        R Visit<S, R>(ITypeVisitor<S, R> visitor, S state);

    }
}
