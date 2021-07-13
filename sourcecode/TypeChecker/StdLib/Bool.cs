using System;
using System.Collections.Generic;
using System.Text;
using System.Linq;
using Nom.Language;

namespace Nom.TypeChecker.StdLib
{
    internal class Bool : AStdClass
    {
        public static Bool Instance = new Bool();
        private Bool() : base("Bool", Object.Instance) { }
        
    }
}
