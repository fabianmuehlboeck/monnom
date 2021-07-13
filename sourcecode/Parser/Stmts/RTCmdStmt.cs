using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Antlr4.Runtime;

namespace Nom.Parser
{
    public class RTCmdStmt : AStmt
    {
        private string str;
        public String Cmd => str;
        public RTCmdStmt(IToken str, ISourceSpan locs) : base(locs)
        {
            this.str = str.Text.Substring(1, str.Text.Length - 2);
        }

        public override IEnumerable<Identifier> UsedIdentifiers
        {
            get
            {
                yield break;
            }
        }


        public override Ret Visit<Arg, Ret>(IStmtVisitor<Arg, Ret> visitor, Arg arg = default(Arg))
        {
            return visitor.VisitRTCmdStmt(this, arg);
        }

        public override void PrettyPrint(PrettyPrinter p)
        {
            throw new NotImplementedException();
        }
    }

    public partial interface IStmtVisitor<in S, out R>
    {
        Func<RTCmdStmt, S, R> VisitRTCmdStmt { get; }
    }
}
