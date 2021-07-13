using Nom.Language;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Nom.Parser
{
    public class Namespace : AAnnotatedAstNode<INamespaceSpec>
    {
        public DeclQName Name
        {
            get;
            private set;
        }

        public IEnumerable<InterfaceDef> Interfaces
        {
            get;
            protected set;
        }

        public IEnumerable<ClassDef> Classes
        {
            get;
            protected set;
        }

        public IEnumerable<Namespace> Namespaces
        {
            get;
            protected set;
        }

        public Namespace(DeclQName name, IEnumerable<InterfaceDef> ifaces, IEnumerable<ClassDef> classes, IEnumerable<Namespace> namespaces, ISourceSpan locs =null):base(locs)
        {
            this.Name = name;
            this.Interfaces = ifaces;
            this.Classes = classes;
            this.Namespaces = namespaces;
        }
  
        public override R VisitAstNode<S, R>(IAstNodeVisitor<S, R> visitor, S state)
        {
            return visitor.VisitNamespace(this, state);
        }

        public override void PrettyPrint(PrettyPrinter p)
        {
            if (Name.Count() > 0)
            {
                p.Write("namespace", Start);
                p.IncreaseIndent();
                Name.PrettyPrint(p);
                p.DecreaseIndent();
                p.WriteLine();
                p.WritePunctuation("{");
                p.IncreaseIndent();
                p.WriteLine();
                p.PrintMembers(Classes.Cast<IAstNode>().Concat(Interfaces).Concat(Namespaces));
                p.DecreaseIndent();
                p.WriteLine();
                p.WritePunctuation("}");
                p.WriteLine();
            }
            else
            {
                p.PrintMembers(Classes.Cast<IAstNode>().Concat(Interfaces).Concat(Namespaces));
            }
        }
    }

    public partial interface IAstNodeVisitor<in S, out R>
    {
        Func<Namespace, S, R> VisitNamespace { get; }
    }
}

