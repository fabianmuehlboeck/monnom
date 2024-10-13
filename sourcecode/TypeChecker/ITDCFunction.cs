using Nom.Language;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Nom.TypeChecker
{
    public interface ITDCFunction : ICallableSpec
    {
        public string LibraryName { get; }
        public string FunctionName { get; }

    }
}
