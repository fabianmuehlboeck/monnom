using System;
using System.Collections.Generic;
using System.Text;
using Nom.Language;
using System.Linq;
using System.Collections;

namespace Nom.TypeChecker
{
    public class TypeParametersSpec : ITypeParametersSpec
    {
        public static TypeParametersSpec Empty = new TypeParametersSpec(new List<ITypeParameterSpec>());
        public TypeParametersSpec(IEnumerable<ITypeParameterSpec> pars)
        {
            this.Entries = pars.ToList();
        }
        public IEnumerable<ITypeParameterSpec> Entries { get; }

        public IEnumerator<ITypeParameterSpec> GetEnumerator()
        {
            return Entries.GetEnumerator();
        }

        public bool IsDisjoint(ITypeParametersSpec other)
        {
            return other.Entries.Count() != this.Entries.Count() || Entries.Zip(other.Entries, (l, r) => l.UpperBound.IsDisjoint(r.UpperBound)).Any(x=>x);
        }

        public ITypeParametersSpec Merge(ITypeParametersSpec other)
        {
            throw new NotImplementedException();
        }

        IEnumerator IEnumerable.GetEnumerator()
        {
            return GetEnumerator();
        }
    }
}
