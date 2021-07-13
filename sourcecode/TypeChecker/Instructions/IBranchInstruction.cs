using System;
using System.Collections.Generic;
using System.Text;

namespace Nom.TypeChecker
{
    public interface IBranchInstruction : IInstruction
    {
        BranchEnvironment OutEnvironment { get; }
        void RegisterIncoming(IRegister to, IRegister from);
    }
}
