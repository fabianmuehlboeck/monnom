using System;
using System.Collections.Generic;
using System.Text;
using System.Linq;
using Nom.Language;

namespace Nom.TypeChecker
{
    internal class FieldSpec : IFieldDecl
    {
        public FieldSpec(IClassSpec container, Parser.Identifier name, IType type, Visibility visibility, bool isReadonly, bool isVolatile)
        {
            this.Identifier = name;
            Type = type;
            Visibility = visibility;
            IsReadonly = isReadonly;
            IsVolatile = isVolatile;
            Container = container;
        }
        INamespaceSpec IMember.Container => Container;
        public IClassSpec Container { get; }
        public bool IsReadonly { get; }

        public bool IsVolatile { get; }

        public Parser.Identifier Identifier { get; }

        public string Name => Identifier.Name;

        public IType Type { get; }

        public Visibility Visibility { get; }
        public IOptional<IExprTransformResult> DefaultValueExpr { get; set; }
    }
}
