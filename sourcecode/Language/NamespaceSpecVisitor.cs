using System;
using System.Collections.Generic;
using System.Text;

namespace Nom.Language
{
    public class NamespaceSpecVisitor<Arg, Ret> : INamespaceSpecVisitor<Arg, Ret>
    {
        public NamespaceSpecVisitor(Func<INamespaceSpec, Arg, Ret> nsvisitor, Func<IInterfaceSpec, Arg, Ret> ifacevisitor = null, Func<IClassSpec, Arg, Ret> clsvisitor = null)
        {
            this.VisitNamespaceSpec = nsvisitor;
            this.VisitInterfaceSpec = ifacevisitor ?? this.VisitNamespaceSpec;
            this.VisitClassSpec = clsvisitor ?? this.VisitInterfaceSpec;
        }

        public Func<INamespaceSpec, Arg, Ret> VisitNamespaceSpec
        {
            get;
            private set;
        }

        public Func<IClassSpec, Arg, Ret> VisitClassSpec
        {
            get;
            private set;
        }

        public Func<IInterfaceSpec, Arg, Ret> VisitInterfaceSpec
        {
            get;
            private set;
        }
    }
}
