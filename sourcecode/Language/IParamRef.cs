using System;
using System.Collections.Generic;
using System.Text;

namespace Nom.Language
{
    public interface IParamRef<out T, P> : ISpecRef<T>, IParameterizedSpecRef<T> where T : INamespaceSpec where P : ITypeArgument, ISubstitutable<P>
    {
        IParamRef<T, Q> TransformArgs<Q>(Func<P,Q> transformer) where Q : ITypeArgument, ISubstitutable<Q>;
        ITypeEnvironment<P> PArguments { get; }
        Ret Visit<Arg, Ret>(IParamRefVisitor<Arg, Ret, P> visitor, Arg arg = default(Arg));
    }
    public interface IParamRefVisitor<Arg, Ret, P> where P : ITypeArgument, ISubstitutable<P>
    {
        Func<IParamRef<INamespaceSpec, P>, Arg, Ret> VisitNamespace { get; }
        Func<IParamRef<IInterfaceSpec, P>, Arg, Ret> VisitInterface { get; }
        Func<IParamRef<IClassSpec, P>, Arg, Ret> VisitClass { get; }
    }
    public class ParamRefVisitor<Arg, Ret, P> : IParamRefVisitor<Arg, Ret, P> where P : ITypeArgument, ISubstitutable<P>
    {
        public ParamRefVisitor(Func<IParamRef<INamespaceSpec, P>, Arg, Ret> namespaceVisitor, Func<IParamRef<IInterfaceSpec, P>, Arg, Ret> interfaceVisitor=null, Func<IParamRef<IClassSpec, P>, Arg, Ret> classVisitor = null)
        {
            VisitNamespace = namespaceVisitor;
            VisitInterface = interfaceVisitor ?? VisitNamespace;
            VisitClass = classVisitor ?? VisitInterface;
        }

        public Func<IParamRef<INamespaceSpec, P>, Arg, Ret> VisitNamespace { get; }

        public Func<IParamRef<IInterfaceSpec, P>, Arg, Ret> VisitInterface { get; }

        public Func<IParamRef<IClassSpec, P>, Arg, Ret> VisitClass { get; }
    }

    public class ParamRefEqualityComparer<T, P> : IEqualityComparer<IParamRef<T, P>> where T : INamespaceSpec where P : ITypeArgument, ISubstitutable<P>
    {
        private ParamRefEqualityComparer() { }
        public static ParamRefEqualityComparer<T, P> Instance { get; } = new ParamRefEqualityComparer<T, P>();
        public bool Equals(IParamRef<T, P> x, IParamRef<T, P> y)
        {
            return x.Element.Equals(y.Element);
        }

        public int GetHashCode(IParamRef<T, P> obj)
        {
            return obj.Element.GetHashCode();
        }
    }
}
