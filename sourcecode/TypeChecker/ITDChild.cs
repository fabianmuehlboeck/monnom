using System;
using System.Collections.Generic;
using System.Text;
using Nom.Parser;
using Nom.Language;

namespace Nom.TypeChecker
{
    internal interface ITDChild : INamespaceSpec
    {

        void AddInterface(TDInterface iface);

        new IEnumerable<ITDChild> PublicChildren { get; }
        new IEnumerable<ITDChild> ProtectedChildren { get; }

        Ret Visit<Arg, Ret>(ITDChildVisitor<Arg, Ret> visitor, Arg arg = default(Arg));

        TDLambda CreateLambda(ITypeParametersSpec typeParameters, IParametersSpec parameters, Language.IType returnType);
        TDStruct CreateStruct();
    }
}
