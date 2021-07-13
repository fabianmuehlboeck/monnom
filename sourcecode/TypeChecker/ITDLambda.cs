using System;
using System.Collections.Generic;
using System.Text;
using Nom.Language;

namespace Nom.TypeChecker
{
    public interface ITDLambda
    {
        ITypeParametersSpec ClosureTypeParameters { get; set; }
        IParametersSpec ClosureParameters { get; set; }

        IEnumerable<TDLambdaField> Fields { get; }
        ITypeParametersSpec TypeParameters { get; }
        IParametersSpec Parameters { get; }
        IType ReturnType { get; }
        int RegisterCount { get; set; }
        IEnumerable<IInstruction> Instructions { get; set; }
        string LambdaID { get; }
    }
}
