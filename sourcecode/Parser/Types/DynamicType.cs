using System;
using System.Collections.Generic;
using System.Text;

namespace Nom.Parser
{
    public class DynamicType : AType
    {
        public DynamicType(ISourceSpan locs) : base(locs)
        {
        }

        public override string ReferenceName => "dynamic";

        public override void PrettyPrint(PrettyPrinter p)
        {
            p.Write("dyn", Start);
            p.WriteWhitespace();
        }

        public override R Visit<S, R>(ITypeVisitor<S, R> visitor, S state)
        {
            return visitor.VisitDynamicType(this, state);
        }
    }

    public partial interface ITypeVisitor<in S, out R>
    {
        Func<DynamicType, S, R> VisitDynamicType { get; }
    }
}
