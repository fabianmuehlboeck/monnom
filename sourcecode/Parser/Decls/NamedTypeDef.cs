using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Nom.Parser
{
    public interface NamedTypeDef: IAstNode
    {
        DeclIdentifier Name
        {
            get;
        }

        IEnumerable<InheritanceDecl> Implements
        {
            get;
        }

        IEnumerable<Constructor> Constructors
        {
            get;
        }
        
        T Visit<T>(Func<InterfaceDef, T> ifun, Func<ClassDef, T> cfun);

        R VisitNamedTypeDef<S, R>(INamedTypeDefVisitor<S, R> visitor, S state);
    }

    public partial interface INamedTypeDefVisitor<in S, out R>
    {

    }
}
