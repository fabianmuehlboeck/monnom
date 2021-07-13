using System;
using System.Collections.Generic;
using System.Text;
using Nom.Language;
using System.Linq;

namespace Nom.TypeChecker
{
    public class ParametersSpec : IParametersSpec
    {
        public ParametersSpec(IEnumerable<IParameterSpec> specs)
        {
            Entries = specs.ToList();
        }
        public IEnumerable<IParameterSpec> Entries { get; }

        public bool IsDisjoint(IParametersSpec other)
        {
            return Entries.Count() != other.Entries.Count() || Entries.Zip(other.Entries, (l, r) => l.Type.IsDisjoint(r.Type)).Any(x=>x);
        }

        public IParametersSpec Merge(IParametersSpec other)
        {
            throw new NotImplementedException();
        }

        public bool WouldAccept(ITypeEnvironment<IType> env, IEnumerable<IType> args, bool optimistic=true) //where T : ITypeArgument, ISubstitutable<T>
        {
            if(args.Count()!=this.Entries.Count())
            {
                return false;
            }
            return args.Zip(Entries, (arg, entry) => arg.IsSubtypeOf(((ISubstitutable<IType>)entry.Type).Substitute(env), optimistic)).All(x => x);
        }
    }
}
