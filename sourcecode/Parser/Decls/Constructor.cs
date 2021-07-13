using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Antlr4.Runtime;
using Nom.Language;

namespace Nom.Parser
{
    public class Constructor : AAnnotatedAstNode<IConstructorSpec>
    {
        public IEnumerable<VarDecl> Args
        {
            get;
            private set;
        }

        public IEnumerable<TypeArgDecl> TypeArguments { get; } = new List<TypeArgDecl>();

        public readonly VisibilityNode Visibility;

        private Block prep_stmts;
        private IEnumerable<IExpr> super_args;
        private IEnumerable<IType> super_typeargs=new List<IType>();
        private Block after_stmts;

        public Block PreSuperStatements => prep_stmts;
        public Block PastSuperStatements => after_stmts;
        public IEnumerable<IExpr> SuperCallArgs => super_args;
        public IEnumerable<IType> SuperCallTypeArgs => super_typeargs;

        public Constructor(VisibilityNode visibility, IEnumerable<VarDecl> args, Block prep_stmts, IEnumerable<IExpr> super_args, Block after_stmts, ISourceSpan locs = null) : base(locs ?? new GenSourceSpan())
        {
            this.Visibility = visibility;
            this.Args = args.ToList();
            this.prep_stmts = prep_stmts;
            this.super_args = super_args;
            this.after_stmts = after_stmts;
        }
        protected Constructor(VisibilityNode visibility, IEnumerable<VarDecl> args, ISourceSpan locs=null) : base(locs??new GenSourceSpan())
        {
            this.Visibility = visibility;
            this.Args = args.ToList();
        }
        
        public override R VisitAstNode<S, R>(IAstNodeVisitor<S, R> visitor, S state)
        {
            return visitor.VisitConstructor(this, state);
        }

        public override void PrettyPrint(PrettyPrinter p)
        {
            Visibility.PrettyPrint(p);
            p.Write("constructor", Start);
            p.WritePunctuation("(");
            p.IncreaseIndent();
            p.PrintMembers(Args, ",");
            p.WritePunctuation(")");
            p.DecreaseIndent();
            p.WriteLine();
            p.WritePunctuation("{");
            p.IncreaseIndent();
            p.WriteLine();
            p.PrintMembers(PreSuperStatements);
            p.WriteLine();
            p.WriteKeyword("super");
            p.WritePunctuation("(");
            p.IncreaseIndent();
            p.PrintMembers(SuperCallArgs, ",");
            p.WritePunctuation(");");
            p.DecreaseIndent();
            p.WriteLine();
            p.PrintMembers(PastSuperStatements);
            p.WriteLine();
            p.DecreaseIndent();
            p.WritePunctuation("}");
            p.WriteLine();
        }
    }

    public partial interface IAstNodeVisitor<in S, out R>
    {
        Func<Constructor, S, R> VisitConstructor { get; }
    }
}

