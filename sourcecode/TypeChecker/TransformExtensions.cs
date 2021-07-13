using System;
using System.Collections.Generic;
using System.Text;
using Nom.Language;

namespace Nom.TypeChecker.TransformExtensions
{
    public static class TransformExtensions
    {
        internal static Language.IType TransformType(this Nom.Parser.IType type, ITypeCheckLookup<IType, ITypeArgument> context)
        {
            return type.Visit(TypeTransformer.Instance, context);
        }
    }
}
