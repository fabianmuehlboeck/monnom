using System;
using System.Collections.Generic;
using System.Text;

namespace Nom.Language
{
    public interface INamespaceSpecVisitor<in Arg, out Ret>
    {
        Func<INamespaceSpec, Arg, Ret> VisitNamespaceSpec
        {
            get;
        }
        Func<IClassSpec, Arg, Ret> VisitClassSpec
        {
            get;
        }
        Func<IInterfaceSpec, Arg, Ret> VisitInterfaceSpec
        {
            get;
        }
    }
}
