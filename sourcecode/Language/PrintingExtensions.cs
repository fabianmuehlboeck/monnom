using System;
using System.Linq;
using System.Collections.Generic;
using System.Text;

namespace Nom.Language
{
    public static class PrintingExtensions
    {
        public static string ToSignatureText(this IType type)
        {
            return type.ToString();
        }
        public static string ToSignatureText(this ITypeParameterSpec typeParam)
        {
            return typeParam.Name + typeParam.UpperBound.InjectOptional().Extract(ub => " extends "+ub.ToSignatureText(), "") + typeParam.LowerBound.InjectOptional().Extract(lb=>" super "+lb.ToSignatureText());
        }
        public static string ToSignatureText(this ITypeParametersSpec typeParameters)
        {
            return typeParameters.Any()?"<" + String.Join(", ", typeParameters.Select(tp => tp.ToSignatureText()))+">":"";
        }
        public static string ToSignatureText(this IParameterSpec param)
        {
            return param.Name + " : " + param.Type.ToSignatureText();
        }
        public static string ToSignatureText(this IParametersSpec parameters)
        {
            return "(" + String.Join(", ", parameters.Entries.Select(p => p.ToSignatureText())) + ")";
        }
        public static string ToSignatureText(this Visibility visibility)
        {
            switch(visibility)
            {
                case Visibility.Internal:
                    return "internal";
                case Visibility.Private:
                    return "private";
                case Visibility.Protected:
                    return "protected";
                case Visibility.ProtectedInternal:
                    return "protected internal";
                case Visibility.Public:
                    return "public";
            }
            return "[ERROR]";
        }
        public static string ToSignatureText(this IMethodSpec method)
        {
            return method.Visibility.ToSignatureText() + " " + method.Name +method.TypeParameters.ToSignatureText()+ method.Parameters.ToSignatureText() + " : " + method.ReturnType.ToSignatureText();
        }
    }
}
