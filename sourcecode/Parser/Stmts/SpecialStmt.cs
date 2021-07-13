using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Nom.Parser
{
    public class SpecialStmt :AStmt
    {
        public readonly IEnumerable<string> Lines;
        public readonly bool CanThrowException;

        public override IEnumerable<Identifier> UsedIdentifiers
        {
            get
            {
                yield break;
            }
        }

        public SpecialStmt(IEnumerable<string> lines, bool canThrowException=true) :base(new GenSourceSpan())
        {
            this.Lines = lines;
            this.CanThrowException = canThrowException;
        }
        
        public override Ret Visit<Arg, Ret>(IStmtVisitor<Arg, Ret> visitor, Arg arg = default(Arg))
        {
            return visitor.VisitSpecialStmt(this, arg);
        }

        public override void PrettyPrint(PrettyPrinter p)
        {
            throw new NotImplementedException();
        }
    }

    public partial interface IStmtVisitor<in S, out R>
    {
        Func<SpecialStmt, S, R> VisitSpecialStmt { get; }
    }
}
