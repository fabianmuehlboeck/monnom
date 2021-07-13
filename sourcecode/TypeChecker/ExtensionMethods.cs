using System;
using System.Collections.Generic;
using System.Text;
using System.Linq;

using Nom.Language;

namespace Nom.TypeChecker
{
    public static class ExtensionMethods
    {

        public static IExprTransformResult EnsureType(this IExprTransformResult etr, Language.IType type, ICodeTransformEnvironment env)
        {
            if(etr.Type.IsSubtypeOf(type, false))
            {
                return etr;
            }
            if(etr.Type.IsSubtypeOf(type, true))
            {
                Func<IType, IExprTransformResult> defaultAction = tp =>
                {
                    CastInstruction ci = new CastInstruction(type, etr.Register, env.CreateRegister());
                    return new ExprTransformResult(type, ci.Register, etr.Snoc(ci));
                };
                var visitor = new NoargTypeVisitor<IExprTransformResult>(defaultAction);
                visitor.VisitProbablyType = (pb) =>
                {
                    if (pb.PotentialType.IsSubtypeOf(type, true)&&type.IsSubtypeOf(StdLib.StdLib.ObjectType, true))
                    {
                        CastInstruction ci = new CastInstruction(StdLib.StdLib.ObjectType, etr.Register, env.CreateRegister());
                        return new ExprTransformResult(type, ci.Register, etr.Snoc(ci));
                    }
                    return defaultAction(pb);
                   };
                return etr.Type.Visit(visitor);
            }
            throw new TypeCheckException("@0: Expected type $2, but found $1", etr, type, etr.Type);
        }

        public static IEnumerable<IType> GetOrderedArgumentList(this Language.IParameterizedSpecRef<IParameterizedSpec> spec) 
        {
            return spec.ParameterizedParent.Extract<IEnumerable<IType>>(p => p.GetOrderedArgumentList(), new List<IType>()).Concat(spec.Arguments.Select(arg=>arg.AsType)); 
        }

        public static ITypeEnvironment<T> Push<T,TX>(this ITypeEnvironment<T> self, ITypeEnvironment<TX> env) where T : Language.ITypeArgument, Nom.Language.ISubstitutable<T> where TX : T, ISubstitutable<TX>
        {
            return self.Push(env.Select(x => x.Key), env.Select(x => x.Value));
        }


    }
}
