using System;
using System.Collections.Generic;
using System.Text;
using System.Linq;
using Nom.Language;

namespace Nom.TypeChecker.StdLib
{
    internal class ParameterSpec : IParameterSpec
    {
        public ParameterSpec(string name, IType type)
        {
            Name = name;
            Type = type;
        }
        public string Name { get; }

        public IType Type { get; }
    }
    internal class MethodSpec : AParameterized, IMethodSpec
    {
        public MethodSpec(string name, INamespaceSpec container, ITypeParametersSpec typeParameters, IParametersSpec parameters, IType returnType, Visibility visibility=Visibility.Public, bool isFinal=true, bool isVirtual=false)
        {
            Name = name;
            Container = container;
            Parameters = parameters;
            ReturnType = returnType;
            TypeParameters = typeParameters;
            Visibility = visibility;
            IsFinal = isFinal;
            IsVirtual = isVirtual;
        }
        public string Name { get; }

        public bool IsFinal { get; }
        public bool IsVirtual { get; }

        public IParametersSpec Parameters { get; }

        public IType ReturnType { get; }


        public override ITypeParametersSpec TypeParameters { get; }

        protected override IOptional<IParameterizedSpec> ParamParent => Container.InjectOptional();

        public INamespaceSpec Container { get; }

        public Visibility Visibility { get; }

        public bool Overrides(IMethodSpec other, ITypeEnvironment<IType> substitutions)
        {
            bool ret = Name == other.Name && TypeParameters.Count() == other.TypeParameters.Count() && Parameters.Entries.Count() == other.Parameters.Entries.Count() && Visibility == other.Visibility;
            ret = ret && ReturnType.IsSubtypeOf(((ISubstitutable<IType>)other.ReturnType).Substitute(substitutions), true);
            foreach (var tp in Parameters.Entries.Zip(other.Parameters.Entries, (x, y) => new Tuple<IType, IType>(x.Type, y.Type)))
            {
                ret = ret && ((ISubstitutable<IType>)tp.Item2).Substitute(substitutions).IsSubtypeOf(tp.Item1, true);
            }
            //TODO: type parameter compatibility
            return ret;
        }

        public IMethodSpec Substitute(ITypeParameterSpec param, IType type)
        {
            return new MethodSpec(Name, Container,
                new TypeParametersSpec(TypeParameters.Select(tp => new TDTypeArgDeclDef(new Parser.Identifier(tp.Name), tp.Index, ((ISubstitutable<IType>)tp.UpperBound)?.Substitute(param, type), ((ISubstitutable<IType>)tp.LowerBound)?.Substitute(param, type)))),
                new ParametersSpec(Parameters.Entries.Select(p => new ParameterSpec(p.Name, ((ISubstitutable<IType>)p.Type).Substitute(param, type)))),
                ((ISubstitutable<IType>)ReturnType).Substitute(param, type),
                Visibility,
                IsFinal,
                IsVirtual);
        }

        public IMethodSpec Substitute<E>(ITypeEnvironment<E> env) where E : ITypeArgument, ISubstitutable<E>
        {
            return new MethodSpec(Name, Container,
                new TypeParametersSpec(TypeParameters.Select(tp => new TDTypeArgDeclDef(new Parser.Identifier(tp.Name), tp.Index, ((ISubstitutable<IType>)tp.UpperBound)?.Substitute(env), ((ISubstitutable<IType>)tp.LowerBound)?.Substitute(env)))),
                new ParametersSpec(Parameters.Entries.Select(p => new ParameterSpec(p.Name, ((ISubstitutable<IType>)p.Type).Substitute(env)))),
                ((ISubstitutable<IType>)ReturnType).Substitute(env),
                Visibility,
                IsFinal,
                IsVirtual);
        }
    }
}
