using System;
using System.Collections.Generic;
using System.Text;
using Nom.Language;
using System.Linq;

namespace Nom.TypeChecker
{
    public class TDStructField
    {
        public Parser.Identifier Identifier { get; }
        public string Name => Identifier.Name;
        public Language.IType Type { get; }
        public ITDStruct Struct { get; }
        public IExprTransformResult InitializerExpr { get; }

        public virtual bool IsReadOnly { get; }

        public TDStructField(Parser.Identifier ident, Language.IType type, ITDStruct tdstruct, bool isReadOnly, IExprTransformResult initExpr)
        {
            Identifier = ident;
            Type = type;
            Struct = tdstruct;
            InitializerExpr = initExpr;
        }
    }
}
