using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Nom.Parser
{
    public class NewCall : AAstNode
    {
        public RefQName Type
        {
            get;
        }
        public NewCall(RefQName name, ISourceSpan locs):base(locs)
        {
            this.Type = name;
        }

        public override R VisitAstNode<S, R>(IAstNodeVisitor<S, R> visitor, S state)
        {
            return visitor.VisitNewCall(this, state);
        }

        public override void PrettyPrint(PrettyPrinter p)
        {
            p.Write("new", Start);
            p.WriteWhitespace();
            Type.PrettyPrint(p);
        }
    }

    public partial interface IAstNodeVisitor<in S, out R>
    {
        Func<NewCall, S, R> VisitNewCall { get; }
    }
}
