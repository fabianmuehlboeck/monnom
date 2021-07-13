using System;
using System.Collections.Generic;
using System.Text;
using System.Linq;

namespace Nom.Parser
{
    public class AArgIdentifier<NameT, ArgT> : AAstNode, IArgIdentifier<NameT, ArgT>, IPosNamedReference where ArgT: IReference
    {
        public AArgIdentifier(NameT name, IEnumerable<ArgT> args, ISourceSpan locs =null) : base(locs)
        {
            this.Name = name;
            this.Arguments = args.ToList();
        }
        public NameT Name { get; }

        public IEnumerable<ArgT> Arguments { get; }

        public string ReferencePosName => ReferenceName + " (" + ReferencePosition + ")";

        public string ReferenceName => Name.ToString()+"<"+String.Join(", ", Arguments.Select(arg=> arg.GetReferenceName()))+">";

        public bool Equals(IArgIdentifier<NameT, ArgT> other)
        {
            return other.Name.Equals(Name) && other.Arguments.Count() == this.Arguments.Count() && other.Arguments.Zip(this.Arguments, (x, y) => x.Equals(y)).All(x => x);
        }

        public override bool Equals(object obj)
        {
            var oai = obj as IArgIdentifier<NameT, ArgT>;
            if(oai!=null)
            {
                return Equals(oai);
            }
            var oqn = obj as IQName<IArgIdentifier<NameT, ArgT>>;
            if(oqn!=null)
            {
                return (!oqn.IsEmpty) && Equals(oqn.First());
            }
            return false;
        }

        public override int GetHashCode()
        {
            //int n = Arguments.Count();
            //return Arguments.Aggregate<ArgT, int>(Name.GetHashCode(), (acc, arg) => arg.GetHashCode()*(31 ^ (--n)));
            return ToString().GetHashCode();
        }

        public IArgIdentifier<NameT, X> TransformArg<X>(Func<ArgT, X> transformer) where X : IReference
        {
            return new AArgIdentifier<NameT, X>(Name, Arguments.Select(transformer), this.Locs);
        }

        public override R VisitAstNode<S, R>(IAstNodeVisitor<S, R> visitor, S state)
        {
            return visitor.GetVisitArgIdentifier<NameT, ArgT>()(this, state);
        }

        public override string ToString()
        {
            string argstr = String.Join(", ", Arguments);
            if(argstr.Length>0)
            {
                argstr = "<" + argstr + ">";
            }
            return Name.ToString() + argstr;
        }

        public IArgIdentifier<NX, AX> Transform<NX, AX>(Func<NameT, NX> nameTransformer, Func<ArgT, AX> argTransformer) where AX: IReference
        {
            return new AArgIdentifier<NX, AX>(nameTransformer(Name), Arguments.Select(argTransformer), Locs);
        }

        public override void PrettyPrint(PrettyPrinter p)
        {
            if (Name.ToString().Length == 0)
            {
                p.Write("this", Start);
            }
            else
            {
                p.Write(Name.ToString(), Start);
            }
            if (Arguments.Any())
            {
                p.WritePunctuation("<");
                p.PrintList(Arguments, ",");
                p.WritePunctuation(">");
            }

        }
    }

    public partial interface IAstNodeVisitor<in S, out R>
    {
        Func<IArgIdentifier<NameT, ArgT>, S, R> GetVisitArgIdentifier<NameT, ArgT>() where ArgT : IReference;
    }
}
