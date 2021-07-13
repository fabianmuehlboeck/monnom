using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Nom
{
    public class Comparer<T> : IComparer<T>
    {
        public Comparer(Func<T, T, int> compare)
        {
            this.compare = compare;
        }

        private Func<T, T, int> compare;
        public int Compare(T x, T y)
        {
            return compare(x, y);
        }
    }
}
