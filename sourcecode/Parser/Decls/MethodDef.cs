using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Nom.Parser
{
    public class MethodDef : MethodDecl
    {
        public readonly Block Code;
        private IEnumerable<VarDecl> args;

        public readonly bool IsVirtual;
        public readonly bool IsOverride;
        public readonly bool IsFinal;

        public MethodDef(bool isFinal, bool isVirtual, bool isOverride, bool isCalltarget, VisibilityNode visibility, DeclIdentifier name, IEnumerable<VarDecl> args, IType returns, Block code, ISourceSpan locs)
            : base(visibility, isCalltarget, name, args, returns, locs)
        {
            this.Code = code;
            this.args = args.ToList();
            this.IsVirtual = isVirtual;
            this.IsFinal = isFinal;
            this.IsOverride = isOverride;
        }

        public IEnumerable<VarDecl> ArgDefs => args;
        
        public override R VisitAstNode<S, R>(IAstNodeVisitor<S, R> visitor, S state)
        {
            return visitor.VisitMethodDef(this, state);
        }

        public override void PrettyPrint(PrettyPrinter p)
        {
            Visibility.PrettyPrint(p);
            p.WriteKeyword("fun");
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
            Code.PrettyPrint(p);
            p.WriteLine();
        }

    }

    public partial interface IAstNodeVisitor<in S, out R>
    {
        Func<MethodDef, S, R> VisitMethodDef { get; }
    }
}

