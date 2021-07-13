using System;
using System.Collections.Generic;
using System.Text;
using Nom.Parser;
using Nom.Language;

namespace Nom.TypeChecker
{
    public class TDLambdaField
    {
        public Identifier Identifier { get; }
        public string Name => Identifier.Name;
        public Language.IType Type { get; }
        public ITDLambda Lambda { get; }

        public TDLambdaField(Identifier ident, Language.IType type, ITDLambda lambda)
        {
            Identifier = ident;
            Type = type;
            Lambda = lambda;
        }
    }
}
