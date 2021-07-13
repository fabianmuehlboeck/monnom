using System;
using System.Collections.Generic;
using System.Text;
using System.Linq;

namespace Nom.TypeChecker
{
    internal class TDNamespaceDef
    {
        public ISourceSpan Locs
        {
            get;
            private set;
        }
        public TDNamespaceDef(string name, ISourceSpan locs)
        {
            this.Name = name;
            this.Locs = locs;
        }

        public String Name
        {
            get;
            private set;
        }
    }
}
