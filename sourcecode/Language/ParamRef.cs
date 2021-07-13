using System;
using System.Collections.Generic;
using System.Text;
using System.Linq;

namespace Nom.Language
{
    public abstract class ParamRef<T, ArgT> : IParamRef<T, ArgT> where T : INamespaceSpec where ArgT : ITypeArgument, ISubstitutable<ArgT>
    {
        public ParamRef(T elem, ITypeEnvironment<ArgT> env)
        {
            Element = elem;
            PArguments = env;
        }
        public ITypeEnvironment<ArgT> PArguments { get; }


        public T Element { get; }

        public IEnumerable<ITypeArgument> Arguments => Element.AllTypeParameters.Select(tp => Substitutions[tp]);

        public ITypeEnvironment<ITypeArgument> Substitutions => PArguments.Transform<ITypeArgument>(t => t);

        public IOptional<IParameterizedSpecRef<IParameterizedSpec>> ParameterizedParent => throw new NotImplementedException();

        public IParameterizedSpecRef<T> Substitute(ITypeEnvironment<ITypeArgument> substitutions)
        {
            throw new NotImplementedException();
        }

        public abstract IParamRef<T, Q> TransformArgs<Q>(Func<ArgT, Q> transformer) where Q : ITypeArgument, ISubstitutable<Q>;

        public abstract Ret Visit<Arg, Ret>(IParamRefVisitor<Arg, Ret, ArgT> visitor, Arg arg = default);
    }

    public class NamespaceRef<ArgT> : ParamRef<INamespaceSpec, ArgT> where ArgT : ITypeArgument, ISubstitutable<ArgT>
    {
        public NamespaceRef(INamespaceSpec elem, ITypeEnvironment<ArgT> env) : base(elem, env)
        {
        }

        public override IParamRef<INamespaceSpec, Q> TransformArgs<Q>(Func<ArgT, Q> transformer)
        {
            return new NamespaceRef<Q>(Element, PArguments.Transform<Q>(transformer));
        }

        public override Ret Visit<Arg, Ret>(IParamRefVisitor<Arg, Ret, ArgT> visitor, Arg arg = default)
        {
            return visitor.VisitNamespace(this, arg);
        }
    }

    public class InterfaceRef<ArgT> : ParamRef<IInterfaceSpec, ArgT> where ArgT : ITypeArgument, ISubstitutable<ArgT>
    {
        public InterfaceRef(IInterfaceSpec elem, ITypeEnvironment<ArgT> env) : base(elem, env)
        {
        }

        public override IParamRef<IInterfaceSpec, Q> TransformArgs<Q>(Func<ArgT, Q> transformer)
        {
            return new InterfaceRef<Q>(Element, PArguments.Transform<Q>(transformer));
        }

        public override Ret Visit<Arg, Ret>(IParamRefVisitor<Arg, Ret, ArgT> visitor, Arg arg = default)
        {
            return visitor.VisitInterface(this, arg);
        }
    }

    public class ClassRef<ArgT> : ParamRef<IClassSpec, ArgT> where ArgT : ITypeArgument, ISubstitutable<ArgT>
    {
        public ClassRef(IClassSpec elem, ITypeEnvironment<ArgT> env) : base(elem, env)
        {
        }

        public override IParamRef<IClassSpec, Q> TransformArgs<Q>(Func<ArgT, Q> transformer)
        {
            return new ClassRef<Q>(Element, PArguments.Transform<Q>(transformer));
        }

        public override Ret Visit<Arg, Ret>(IParamRefVisitor<Arg, Ret, ArgT> visitor, Arg arg = default)
        {
            return visitor.VisitClass(this, arg);
        }
    }
}
