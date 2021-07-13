using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Antlr4.Runtime;

namespace Nom.Parser
{
    public class IfTypeStmt : AStmt
    {
        public readonly Identifier Var;
        public readonly Block Thenb;
        public readonly Block Elseb;
        public readonly IType Type;
        public IfTypeStmt(Identifier var, IType type, Block thenb, Block elseb, ISourceSpan locs) : base(locs)
        {
            this.Var = var;
            this.Thenb = thenb;
            this.Elseb = elseb ?? new Block(new List<IStmt>(), locs);
            this.Type = type;
        }
        public override IEnumerable<Identifier> UsedIdentifiers
        {
            get
            {
                return Thenb.UsedIdentifiers.Concat(Elseb.Select(s => s.UsedIdentifiers).Flatten()).Cons(Var);
            }
        }

        public override Ret Visit<Arg, Ret>(IStmtVisitor<Arg, Ret> visitor, Arg arg = default(Arg))
        {
            return visitor.VisitIfTypeStmt(this, arg);
        }

        public override void PrettyPrint(PrettyPrinter p)
        {
            throw new NotImplementedException();
        }
    }

    public partial interface IStmtVisitor<in S, out R>
    {
        Func<IfTypeStmt, S, R> VisitIfTypeStmt { get; }
    }
}
