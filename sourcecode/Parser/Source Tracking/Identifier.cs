using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Antlr4.Runtime;

namespace Nom.Parser
{
    public class Identifier : AAstNode, IPosNamedReference, IEquatable<Identifier>
    {
        public Identifier(IToken token) : base(token, token)
        {
            this.Name = token.Text;
        }

        public Identifier(String name, ISourceSpan locs =null)
            : base(locs ?? new GenSourceSpan())
        {
            this.Name = name;
        }

        public String Name { get; set; }

        public override bool Equals(object obj)
        {
            if(obj is TypeIdentifier)
            {
                TypeIdentifier other = (TypeIdentifier) obj;
                return (other.Arguments.Count() == 0 && other.Name.Name == this.Name);
            }
            if (obj is Identifier)
            {
                return ((Identifier)obj).Name == this.Name;
            }
            return false;
        }
        
        private class EQC : IEqualityComparer<Identifier>
        {
            public bool Equals(Identifier x, Identifier y)
            {
                return x.Equals(y);
            }

            public int GetHashCode(Identifier obj)
            {
                return obj.GetHashCode();
            }
        }

        private static IEqualityComparer<Identifier> eqc = new EQC();
        public static IEqualityComparer<Identifier> EqualityComparer
        {
            get
            {
                return eqc;
            }
        }

        public string ReferencePosName => Name;


        public string ReferenceName => Name + " (" + Locs.GetReferencePosition() + ")";

        public override string ToString()
        {
            return Name.Replace("%", "$");
        }
        public override R VisitAstNode<S, R>(IAstNodeVisitor<S, R> visitor, S state)
        {
            return visitor.VisitIdentifier(this, state);
        }

        public override int GetHashCode()
        {
            return Name.GetHashCode();
        }

        public bool Equals(Identifier other)
        {
            return other.Name == this.Name;
        }

        public override void PrettyPrint(PrettyPrinter p)
        {
            p.Write(Name, Start);
            p.WriteWhitespace();
        }
    }

    public partial interface IAstNodeVisitor<in S, out R>
    {
        Func<Identifier, S, R> VisitIdentifier { get; }
    }
}
