using System;
using System.Collections.Generic;
using System.Text;
using System.Linq;
using Nom.Language;

namespace Nom.TypeChecker
{
    class MethodReference : AParameterized, IMethodSpec
    {
        public IMethodSpec Reference { get; }
        public ITypeEnvironment<IType> Substitutions { get; }
        public MethodReference(IMethodSpec reference, ITypeEnvironment<IType> subst)
        {
            Reference = reference;
            Substitutions = subst;
        }
        public string Name => Reference.Name;

        public bool IsFinal => Reference.IsFinal;

        public bool IsVirtual => Reference.IsVirtual;

        public IParametersSpec Parameters => new ParametersSpec(Reference.Parameters.Entries.Select(ps => new ArgumentDeclDef(new Parser.Identifier(ps.Name), ((ISubstitutable<IType>)ps.Type).Substitute(Substitutions))));

        public IType ReturnType => ((ISubstitutable<IType>)Reference.ReturnType).Substitute(Substitutions);

        protected override IOptional<IParameterizedSpec> ParamParent => Reference.ParameterizedParent;

        public override ITypeParametersSpec TypeParameters => new TypeParametersSpec(Reference.TypeParameters.Entries.Select(tp => new TDTypeArgDeclRef(tp, ((ISubstitutable<IType>)tp.UpperBound).Substitute(Substitutions), ((ISubstitutable<IType>)tp.LowerBound).Substitute(Substitutions))));

        public override int OverallTypeParameterCount => Reference.OverallTypeParameterCount;

        public INamespaceSpec Container => Reference.Container;

        public Visibility Visibility => Reference.Visibility;

        public bool Overrides(IMethodSpec other, ITypeEnvironment<IType> substitutions)
        {
    
            bool ret = Name == other.Name && TypeParameters.Count() == other.TypeParameters.Count() && Parameters.Entries.Count() == other.Parameters.Entries.Count() && Visibility == other.Visibility; 
            ret = ret && ReturnType.IsSubtypeOf(((ISubstitutable<IType>)other.ReturnType).Substitute(substitutions), true);
            foreach(var tp in Parameters.Entries.Zip(other.Parameters.Entries, (x,y)=>new Tuple<IType, IType>(x.Type,y.Type)))
            {
                ret = ret && ((ISubstitutable<IType>)tp.Item2).Substitute(substitutions).IsSubtypeOf(tp.Item1, true);
            }
            //TODO: type parameter compatibility
            return ret;
        }

        public override string ToString()
        {
            return "Reference to " + Reference.ToSignatureText();
        }


        public override IParameterizedSpecRef<IParameterizedSpec> GetAsRef()
        {
            return new ParameterizedSpecRef<IParameterizedSpec>(this, Substitutions.Transform<ITypeArgument>(t=>t));
        }

        public IMethodSpec Substitute(ITypeParameterSpec param, IType type)
        {
            return new StdLib.MethodSpec(Name, Container,
                new TypeParametersSpec(TypeParameters.Select(tp => new TDTypeArgDeclDef(new Parser.Identifier(tp.Name), tp.Index, ((ISubstitutable<IType>)tp.UpperBound)?.Substitute(param, type), ((ISubstitutable<IType>)tp.LowerBound)?.Substitute(param, type)))),
                new ParametersSpec(Parameters.Entries.Select(p => new StdLib.ParameterSpec(p.Name, ((ISubstitutable<IType>)p.Type).Substitute(param, type)))),
                ((ISubstitutable<IType>)ReturnType).Substitute(param, type),
                Visibility,
                IsFinal,
                IsVirtual);
        }

        public IMethodSpec Substitute<E>(ITypeEnvironment<E> env) where E : ITypeArgument, ISubstitutable<E>
        {
            return new StdLib.MethodSpec(Name, Container,
                new TypeParametersSpec(TypeParameters.Select(tp => new TDTypeArgDeclDef(new Parser.Identifier(tp.Name), tp.Index, ((ISubstitutable<IType>)tp.UpperBound)?.Substitute(env), ((ISubstitutable<IType>)tp.LowerBound)?.Substitute(env)))),
                new ParametersSpec(Parameters.Entries.Select(p => new StdLib.ParameterSpec(p.Name, ((ISubstitutable<IType>)p.Type).Substitute(env)))),
                ((ISubstitutable<IType>)ReturnType).Substitute(env),
                Visibility,
                IsFinal,
                IsVirtual);
        }
    }
}
