using System;
using System.Collections.Generic;
using System.Text;

namespace Nom.TypeChecker
{
    internal interface ITDInterfaceContainer
    {
        TDInterface GetInterface(Parser.DeclIdentifier name);
    }
}
