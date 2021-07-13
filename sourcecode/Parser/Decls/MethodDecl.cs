using Nom.Language;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Nom.Parser
{
    public class MethodDecl : AAnnotatedAstNode<Language.IMethodSpec>
    {
        public IEnumerable<ArgDecl> Args
        {
            get;
            protected set;
        }

        private IType returns;
        public IType Returns
        {
            get
            {
                return returns ?? new DynamicType(Name.Locs);
            }
        }

        public VisibilityNode Visibility
        {
            get;
            protected set;
        }

        public bool IsCallTarget { get; }

        public DeclIdentifier Name
        {
            get;
            protected set;
        }

        
        public MethodDecl(VisibilityNode visibility, bool isCallTarget, DeclIdentifier name, IEnumerable<VarDecl> args, IType returns, ISourceSpan locs = null)
            : base(locs ?? new GenSourceSpan())
        {
            this.Visibility = visibility;
            Args = args.Select(a => a.ToArgDecl()).ToList();
            this.returns = returns;
            Name = name;
            if(Name.Name.Name=="this")
            {
                Name.Name.Name = "";
            }
            IsCallTarget = isCallTarget;
        }

        public MethodDecl(VisibilityNode visibility, bool isCallTarget, DeclIdentifier name, IEnumerable<ArgDecl> args, IType returns, ISourceSpan locs = null)
            : base(locs ?? new GenSourceSpan())
        {
            this.Visibility = visibility;
            Args = args.ToList();
            this.returns = returns;
            Name = name;
            if (Name.Name.Name == "this")
            {
                Name.Name.Name = "";
            }
            IsCallTarget = isCallTarget;
        }

        public override string ToString()
        {
            return Name.Name + "(" + String.Join(", ", Args.Select(a => String.Join(" \\/ ", a.Types.Select(t => t.ToString())))) + ")";
        }
        public override R VisitAstNode<S, R>(IAstNodeVisitor<S, R> visitor, S state)
        {
            return visitor.VisitMethodDecl(this, state);
        }

        public override void PrettyPrint(PrettyPrinter p)
        {
            Visibility.PrettyPrint(p);
            p.WriteWhitespace();
            p.WriteKeyword("fun");
            Name.PrettyPrint(p);
            p.WritePunctuation("(");
            p.IncreaseIndent();
            p.PrintMembers(Args, ",");
            p.WritePunctuation(")");
            p.WriteWhitespace();
            p.WriteKeyword(":");
            p.WriteWhitespace();
            Returns.PrettyPrint(p);
            p.WritePunctuation(";");
            p.DecreaseIndent();
            p.WriteLine();
        }
    }

    public partial interface IAstNodeVisitor<in S, out R>
    {
        Func<MethodDecl, S, R> VisitMethodDecl { get; }
    }
}

