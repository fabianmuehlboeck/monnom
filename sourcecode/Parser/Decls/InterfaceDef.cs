using Nom.Language;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Nom.Parser
{
    public class InterfaceDef : AAnnotatedAstNode<Language.IInterfaceSpec>, NamedTypeDef
    {
        private IEnumerable<MethodDecl> methods;

        public IEnumerable<MethodDecl> Methods
        {
            get { return methods; }
        }

        public IEnumerable<TypeArgDecl> TypeArguments
        {
            get
            {
                return Name.Arguments;
            }
        }

        public DeclIdentifier Name
        {
            get;
            private set;
        }

        public VisibilityNode Visibility
        {
            get;
            protected set;
        }

        public bool IsPartial
        {
            get;
            protected set;
        }

        public bool IsShape
        {
            get;
            protected set;
        }
        public bool IsMaterial
        {
            get;
            protected set;
        }

        public IEnumerable<InterfaceDef> Interfaces
        {
            get;
            protected set;
        }

        public InterfaceDef(DeclIdentifier name, IEnumerable<InheritanceDecl> extends, IEnumerable<MethodDecl> methods, VisibilityNode visibility, bool isPartial, bool isShape, bool isMaterial, IEnumerable<InterfaceDef> interfaces, ISourceSpan locs) : base(locs)
        {
            this.Name = name;
            this.Visibility = visibility;
            this.IsPartial = isPartial;
            this.IsShape = isShape;
            this.IsMaterial = isMaterial;
            this.Interfaces = interfaces.Where(x => x != null).ToList();
            this.Implements = extends.Where(x => x != null).ToList();
            this.methods = methods.Where(x => x != null).ToList();
        }



        public IEnumerable<InheritanceDecl> Implements
        {
            get;
            protected set;
        }

        public virtual IEnumerable<Constructor> Constructors
        {
            get { yield break; }
        }

        public virtual T Visit<T>(Func<InterfaceDef, T> ifun, Func<ClassDef, T> cfun)
        {
            return ifun(this);
        }

        public override R VisitAstNode<S, R>(IAstNodeVisitor<S, R> visitor, S state)
        {
            return this.VisitNamedTypeDef(visitor, state);
        }

        public virtual R VisitNamedTypeDef<S, R>(INamedTypeDefVisitor<S, R> visitor, S state)
        {
            return visitor.VisitInterfaceDef(this, state);
        }

        public override void PrettyPrint(PrettyPrinter p)
        {
            StringBuilder sb = new StringBuilder();
            if(IsPartial)
            {
                sb.Append("partial ");
            }
            if (IsShape)
            {
                sb.Append("shape ");
            }
            if (IsMaterial)
            {
                sb.Append("material ");
            }
            sb.Append("interface");
            p.Write(sb.ToString(), Start);
            p.WriteWhitespace();
            p.IncreaseIndent();
            Name.PrettyPrint(p);
            p.WriteWhitespace();
            if (Implements.Any())
            {
                p.WriteKeyword("extends");
                p.PrintMembers(Implements, ",");
            }
            p.DecreaseIndent();
            p.WriteLine();
            p.WritePunctuation("{");
            p.WriteLine();
            p.IncreaseIndent();
            p.PrintMembers(this.Methods.Cast<IAstNode>().Concat(this.Interfaces));
            p.DecreaseIndent();
            p.WriteLine();
            p.WritePunctuation("}");
            p.WriteLine();
        }
    }

    public partial interface INamedTypeDefVisitor<in S, out R>
    {
        Func<InterfaceDef, S, R> VisitInterfaceDef { get; }
    }
}
