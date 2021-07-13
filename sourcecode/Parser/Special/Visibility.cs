using Nom.Language;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Nom.Parser
{

    public class VisibilityNode : AAstNode
    {
        public Visibility Visibility
        {
            get;
            private set;
        }
        public VisibilityNode(Visibility v, ISourceSpan locs) : base(locs)
        {
            Visibility = v;
        }

        public VisibilityNode Max(params VisibilityNode[] others)
        {
            VisibilityNode curMax = this;
            foreach (VisibilityNode n in others)
            {
                if (n > curMax)
                {
                    curMax = n;
                }
            }
            return curMax;
        }

        public VisibilityNode Min(params VisibilityNode[] others)
        {
            VisibilityNode curMin = this;
            foreach (VisibilityNode n in others)
            {
                if (n < curMin)
                {
                    curMin = n;
                }
            }
            return curMin;
        }

        public byte NumRep
        {
            get
            {
                return (byte)Visibility;
            }
        }

        public static readonly VisibilityNode Internal = new VisibilityNode(Visibility.Internal, new GenSourceSpan());
        public static readonly VisibilityNode Public = new VisibilityNode(Visibility.Public, new GenSourceSpan());
        public static readonly VisibilityNode Private = new VisibilityNode(Visibility.Private, new GenSourceSpan());
        public static readonly VisibilityNode Protected = new VisibilityNode(Visibility.Protected, new GenSourceSpan());
        public static readonly VisibilityNode ProtectedInternal = new VisibilityNode(Visibility.ProtectedInternal, new GenSourceSpan());

        public static bool operator <=(VisibilityNode left, VisibilityNode right)
        {
            return (int)left.Visibility <= (int)right.Visibility;
        }

        public static bool operator >=(VisibilityNode left, VisibilityNode right)
        {
            return (int)left.Visibility >= (int)right.Visibility;
        }

        public static bool operator <(VisibilityNode left, VisibilityNode right)
        {
            return (int)left.Visibility < (int)right.Visibility;
        }

        public static bool operator >(VisibilityNode left, VisibilityNode right)
        {
            return (int)left.Visibility > (int)right.Visibility;
        }

        public static bool operator ==(VisibilityNode left, VisibilityNode right)
        {
            return left.Visibility == right.Visibility;
        }

        public static bool operator !=(VisibilityNode left, VisibilityNode right)
        {
            return left.Visibility != right.Visibility;
        }

        public override int GetHashCode()
        {
            return Visibility.GetHashCode();
        }

        public override bool Equals(object obj)
        {
            return Visibility.Equals(obj);
        }

        public override string ToString()
        {
            return Visibility.ToString();
        }
        public override R VisitAstNode<S, R>(IAstNodeVisitor<S, R> visitor, S state)
        {
            return visitor.VisitVisibilityNode(this, state);
        }

        public override void PrettyPrint(PrettyPrinter p)
        {
            p.Write(Visibility.ToSignatureText(), Start);
            p.WriteWhitespace();
        }
    }

    public partial interface IAstNodeVisitor<in S, out R>
    {
        Func<VisibilityNode, S, R> VisitVisibilityNode { get; }
    }
}
