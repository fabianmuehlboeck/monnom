using System;
using System.Collections.Generic;
using System.Text;
using Nom.Language;

namespace Nom.TypeChecker
{
    internal interface ITDInterface : ITDChild, IInterface
    {
        bool IsInheritanceInitialized
        {
            get;
        }
        void AddDefinition(TDInterfaceDef def);
    }
}
