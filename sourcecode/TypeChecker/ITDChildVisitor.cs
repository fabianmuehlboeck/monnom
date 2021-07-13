using System;
using System.Collections.Generic;
using System.Text;

namespace Nom.TypeChecker
{
    internal interface ITDChildVisitor<Arg, Ret>
    {
        Func<TDNamespace, Arg, Ret> VisitNamespace
        {
            get;
        }
        Func<TDInterface, Arg, Ret> VisitInterface
        {
            get;
        }
        Func<TDClass, Arg, Ret> VisitClass
        {
            get;
        }
    }
}
