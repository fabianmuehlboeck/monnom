using Nom.Language;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Nom.Parser
{
    public class StaticMethodDef : AAnnotatedAstNode<IStaticMethodSpec>
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


        public DeclIdentifier Name
        {
            get;
            protected set;
        }

        private Block code;
        public Block Code => code;
        private IEnumerable<VarDecl> args;

        public IEnumerable<VarDecl> ArgDefs => args;

        public StaticMethodDef(VisibilityNode visibility, DeclIdentifier name, IEnumerable<VarDecl> args, IType returns, Block code, ISourceSpan locs) : base(locs)
        {
            this.code = code;
            this.args = args.ToList();
            Visibility = visibility;
            Name = name;
            Args = args.Select(a => a.ToArgDecl()).ToList();
            this.returns = returns;
        }

        public override R VisitAstNode<S, R>(IAstNodeVisitor<S, R> visitor, S state)
        {
            return visitor.VisitStaticMethodDef(this, state);
        }

        public override void PrettyPrint(PrettyPrinter p)
        {
            Visibility.PrettyPrint(p);
            p.WriteKeyword("static fun");
            Name.PrettyPrint(p);
            p.WritePunctuation("(");
            p.IncreaseIndent();
            p.PrintMembers(Args, ",");
            p.WritePunctuation(")");
            p.WriteWhitespace();
            p.WriteKeyword(":");
            Returns.PrettyPrint(p);
            p.DecreaseIndent();
            p.WriteLine();
            code.PrettyPrint(p);
            p.WriteLine();
        }
    }

    public partial interface IAstNodeVisitor<in S, out R>
    {
        Func<StaticMethodDef, S, R> VisitStaticMethodDef { get; }
    }
}

