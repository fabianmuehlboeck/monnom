using System;
using System.Collections.Generic;
using System.Text;
using Nom.Parser;
using Nom.Language;

namespace Nom.TypeChecker
{
    internal interface ITDNamespace : INamespace, ITDChild, ITDInterfaceContainer, ITDClassContainer
    {
        void AddDefinition(TDNamespaceDef def);
        void AddNamespace(TDNamespace def);
        void AddClass(TDClass cls);
    }
}
