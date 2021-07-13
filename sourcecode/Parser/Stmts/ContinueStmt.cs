using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Antlr4.Runtime;

namespace Nom.Parser
{
    public class ContinueStmt : AStmt
    {
        public readonly int Depth;
        public ContinueStmt(IToken layersToken, ISourceSpan locs = null):base(locs)
        {
            int layers = int.Parse(layersToken?.Text ?? "0");
            Depth = layers;
        }

        public override IEnumerable<Identifier> UsedIdentifiers
        {
            get
            {
                yield break;
            }
        }
        public override void PrettyPrint(PrettyPrinter p)
        {
            p.Write("break " + Depth.ToString() + ";", Start);
            p.WriteWhitespace();
        }

        public override Ret Visit<Arg, Ret>(IStmtVisitor<Arg, Ret> visitor, Arg arg = default(Arg))
        {
            return visitor.VisitContinueStmt(this, arg);
        }
    }

    public partial interface IStmtVisitor<in S, out R>
    {
        Func<ContinueStmt, S, R> VisitContinueStmt { get; }
    }
}
