using System;
using System.Collections.Generic;
using System.Text;

namespace Nom.TypeChecker
{
    internal interface ITDClassContainer
    {
        TDClass GetClass(Parser.DeclIdentifier name);
    }
}
