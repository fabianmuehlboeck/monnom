using System;
using System.Collections.Generic;
using System.Text;
using System.Linq;
using Nom.Language;
using Nom.Parser;
using Nom.TypeChecker.TransformExtensions;

namespace Nom.TypeChecker
{
    internal static class TypeTransformerReverse
    {
        public static Parser.IType GetParserType(this Language.IType type)
        {
            var ret = type.Visit(new TypeVisitor<object, Parser.IType>()
            {
                DefaultAction = (t, o) => throw new NotImplementedException(),
                VisitClassType = (t, o) => Parser.ClassType.GetInstance(t.ReferenceName),
                VisitInterfaceType = (t, o) => Parser.ClassType.GetInstance(t.ReferenceName),
                VisitBotType = (t, o) => Parser.ClassType.GetInstance("Nothing"),
                VisitDynamicType = (t, o) => new Parser.DynamicType(new GenSourceSpan()),
                VisitMaybeType = (t, o) => new Parser.MaybeType(t.PotentialType.GetParserType()),
                VisitProbablyType = (t, o) => new Parser.ProbablyType(t.PotentialType.GetParserType()),
                VisitTopType = (t, o) => Parser.ClassType.GetInstance("Anything"),
                VisitTypeVariable = (t, o) => Parser.ClassType.GetInstance(t.ReferenceName)
            });
            ret.Annotation = type;
            return ret;
        }
    }
    internal class TypeTransformer : Nom.Parser.ITypeVisitor<ITypeCheckLookup<Nom.Language.IType, Language.ITypeArgument>, Language.IType>
    {
        public static TypeTransformer Instance { get; } = new TypeTransformer();
        public Func<ArrayType, ITypeCheckLookup<Language.IType, Language.ITypeArgument>, Language.IType> VisitArrayType => throw new NotImplementedException();


        public Func<Parser.ClassType, ITypeCheckLookup<Language.IType, Language.ITypeArgument>, Language.IType> VisitClassType => (ct, context) =>
        {
            if ((!ct.IsEmpty) && (!ct.HasNext) && (ct.Current.Elem.Arguments.Count() == 0))
            {
                IOptional<ITypeParameterSpec> tvar = context.GetTypeVariable(ct.Current.Elem.Name.Name);
                if (tvar.HasElem)
                {
                    var tv = new TypeVariable(tvar.Elem);
                    ct.Annotation = tv;
                    return tv;
                }
            }
            IParamRef<INamespaceSpec, Language.ITypeArgument> type = context.GetChild(ct.Transform(id => id.Transform(n => n.Name, t => (ITypeArgument)t.TransformType(context))));
            Language.IType retType = type.Element.Visit(new NamespaceSpecVisitor<object, Language.IType>(
                (ns, o) => { throw new TypeCheckException("Expected class type, but %1 is a namespace.", ct); },
                (iface, o) => new InterfaceType(iface, type.PArguments),
                (cls, o) => new Language.ClassType(cls, type.PArguments)
                ));

            ct.Annotation = retType;
            return retType;
        };

        public Func<Parser.DynamicType, ITypeCheckLookup<Language.IType, ITypeArgument>, Language.IType> VisitDynamicType => (dt, context) =>
        {
            var tp = new Language.DynamicType();
            dt.Annotation = tp;
            return tp;
        };

        public Func<Parser.MaybeType, ITypeCheckLookup<Language.IType, ITypeArgument>, Language.IType> VisitMaybeType => (mb, context) =>
        {
            return new Nom.Language.MaybeType(mb.Type.TransformType(context));
        };

        public Func<Parser.ProbablyType, ITypeCheckLookup<Language.IType, ITypeArgument>, Language.IType> VisitProbablyType => (pb, context) =>
        {
            return new Nom.Language.ProbablyType(pb.Type.TransformType(context));
        };


    }
}
