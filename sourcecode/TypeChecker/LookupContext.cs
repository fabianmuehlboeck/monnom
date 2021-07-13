using System;
using System.Collections.Generic;
using System.Text;
using System.Linq;
using Nom.Language;
using System.Transactions;

namespace Nom.TypeChecker
{
    internal interface ITDParamRefVisitor<Arg, Ret, SRet, P> : IParamRefVisitor<Arg, SRet, P> where Ret : SRet where P : ITypeArgument, ISubstitutable<P>
    {
        Func<ITDParamRef<ITDNamespace, P>, Arg, Ret> VisitTDNamespace { get; }
        Func<ITDParamRef<ITDClass, P>, Arg, Ret> VisitTDClass { get; }
        Func<ITDParamRef<ITDInterface, P>, Arg, Ret> VisitTDInterface { get; }
    }
    internal class TDParamRefVisitor<Arg, Ret, SRet, P> : ParamRefVisitor<Arg, SRet, P>, ITDParamRefVisitor<Arg, Ret, SRet, P> where Ret : SRet where P : ITypeArgument, ISubstitutable<P>
    {
        public TDParamRefVisitor(Func<IParamRef<INamespaceSpec, P>, Arg, SRet> namespaceVisitor,
            Func<ITDParamRef<ITDChild, P>, Arg, Ret> tdDefaultVisitor,
            Func<ITDParamRef<ITDNamespace, P>, Arg, Ret> tdNamespaceVisitor = null,
            Func<IParamRef<IInterfaceSpec, P>, Arg, SRet> interfaceVisitor = null,
            Func<ITDParamRef<ITDInterface, P>, Arg, Ret> tdInterfaceVisitor = null,
            Func<IParamRef<IClassSpec, P>, Arg, SRet> classVisitor = null,
            Func<ITDParamRef<ITDClass, P>, Arg, Ret> tdClassVisitor = null) : base(namespaceVisitor, interfaceVisitor, classVisitor)
        {
            VisitTDNamespace = tdNamespaceVisitor ?? tdDefaultVisitor;
            VisitTDInterface = tdInterfaceVisitor ?? tdDefaultVisitor;
            VisitTDClass = tdClassVisitor ?? tdDefaultVisitor;
        }
        public Func<ITDParamRef<ITDNamespace, P>, Arg, Ret> VisitTDNamespace { get; }

        public Func<ITDParamRef<ITDClass, P>, Arg, Ret> VisitTDClass { get; }

        public Func<ITDParamRef<ITDInterface, P>, Arg, Ret> VisitTDInterface { get; }

    }
    internal abstract class ParamRefChildrenVisitor<P> : INamespaceSpecVisitor<ITypeEnvironment<P>, IParamRef<INamespaceSpec, P>>, ITDChildVisitor<ITypeEnvironment<P>, ITDParamRef<INamespaceSpec, P>>, ITDParamRefVisitor<IArgIdentifier<string, P>, IEnumerable<ITDParamRef<INamespaceSpec, P>>, IEnumerable<IParamRef<INamespaceSpec, P>>, P> where P : ITypeArgument, ISubstitutable<P>, INamedReference
    {
        public Func<INamespaceSpec, ITypeEnvironment<P>, IParamRef<INamespaceSpec, P>> VisitNamespaceSpec => (elem, env) => new NamespaceRef<P>(elem, env);

        public Func<IClassSpec, ITypeEnvironment<P>, IParamRef<INamespaceSpec, P>> VisitClassSpec => (elem, env) => new ClassRef<P>(elem, env);

        public Func<IInterfaceSpec, ITypeEnvironment<P>, IParamRef<INamespaceSpec, P>> VisitInterfaceSpec => (elem, env) => new InterfaceRef<P>(elem, env);

        public abstract Func<ITDParamRef<ITDNamespace, P>, IArgIdentifier<string, P>, IEnumerable<ITDParamRef<INamespaceSpec, P>>> VisitTDNamespace { get; }
        public abstract Func<ITDParamRef<ITDClass, P>, IArgIdentifier<string, P>, IEnumerable<ITDParamRef<INamespaceSpec, P>>> VisitTDClass { get; }
        public abstract Func<ITDParamRef<ITDInterface, P>, IArgIdentifier<string, P>, IEnumerable<ITDParamRef<INamespaceSpec, P>>> VisitTDInterface { get; }
        public abstract Func<IParamRef<INamespaceSpec, P>, IArgIdentifier<string, P>, IEnumerable<IParamRef<INamespaceSpec, P>>> VisitNamespace { get; }
        public abstract Func<IParamRef<IClassSpec, P>, IArgIdentifier<string, P>, IEnumerable<IParamRef<INamespaceSpec, P>>> VisitClass { get; }
        public abstract Func<IParamRef<IInterfaceSpec, P>, IArgIdentifier<string, P>, IEnumerable<IParamRef<INamespaceSpec, P>>> VisitInterface { get; }

        Func<TDNamespace, ITypeEnvironment<P>, ITDParamRef<INamespaceSpec, P>> ITDChildVisitor<ITypeEnvironment<P>, ITDParamRef<INamespaceSpec, P>>.VisitNamespace => (elem, env) => new TDNamespaceRef<P>(elem, env);

        Func<TDInterface, ITypeEnvironment<P>, ITDParamRef<INamespaceSpec, P>> ITDChildVisitor<ITypeEnvironment<P>, ITDParamRef<INamespaceSpec, P>>.VisitInterface => (elem, env) => new TDInterfaceRef<P>(elem, env);

        Func<TDClass, ITypeEnvironment<P>, ITDParamRef<INamespaceSpec, P>> ITDChildVisitor<ITypeEnvironment<P>, ITDParamRef<INamespaceSpec, P>>.VisitClass => (elem, env) => new TDClassRef<P>(elem, env);
    }
    internal class ParamRefPublicChildrenVisitor<P> : ParamRefChildrenVisitor<P> where P : ITypeArgument, ISubstitutable<P>, INamedReference
    {
        public override Func<ITDParamRef<ITDNamespace, P>, IArgIdentifier<string, P>, IEnumerable<ITDParamRef<INamespaceSpec, P>>> VisitTDNamespace => (elem, name) => elem.Element.PublicChildren.Where(chld => chld.Name == name.Name && chld.TypeParameters.Count() == name.Arguments.Count()).Select(chld => chld.Visit(this, elem.PArguments.Push(chld.TypeParameters, name.Arguments)));

        public override Func<ITDParamRef<ITDClass, P>, IArgIdentifier<string, P>, IEnumerable<ITDParamRef<INamespaceSpec, P>>> VisitTDClass => (elem, name) => elem.Element.PublicChildren.Where(chld => chld.Name == name.Name && chld.TypeParameters.Count() == name.Arguments.Count()).Select(chld => chld.Visit(this, elem.PArguments.Push(chld.TypeParameters, name.Arguments)));

        public override Func<ITDParamRef<ITDInterface, P>, IArgIdentifier<string, P>, IEnumerable<ITDParamRef<INamespaceSpec, P>>> VisitTDInterface => (elem, name) => elem.Element.PublicChildren.Where(chld => chld.Name == name.Name && chld.TypeParameters.Count() == name.Arguments.Count()).Select(chld => chld.Visit(this, elem.PArguments.Push(chld.TypeParameters, name.Arguments)));

        public override Func<IParamRef<INamespaceSpec, P>, IArgIdentifier<string, P>, IEnumerable<IParamRef<INamespaceSpec, P>>> VisitNamespace => (elem, name) => elem.Element.PublicChildren.Where(chld => chld.Name == name.Name && chld.TypeParameters.Count() == name.Arguments.Count()).Select(chld => chld.Visit(this, elem.PArguments.Push(chld.TypeParameters, name.Arguments)));

        public override Func<IParamRef<IClassSpec, P>, IArgIdentifier<string, P>, IEnumerable<IParamRef<INamespaceSpec, P>>> VisitClass => (elem, name) => elem.Element.PublicChildren.Where(chld => chld.Name == name.Name && chld.TypeParameters.Count() == name.Arguments.Count()).Select(chld => chld.Visit(this, elem.PArguments.Push(chld.TypeParameters, name.Arguments)));

        public override Func<IParamRef<IInterfaceSpec, P>, IArgIdentifier<string, P>, IEnumerable<IParamRef<INamespaceSpec, P>>> VisitInterface => (elem, name) => elem.Element.PublicChildren.Where(chld => chld.Name == name.Name && chld.TypeParameters.Count() == name.Arguments.Count()).Select(chld => chld.Visit(this, elem.PArguments.Push(chld.TypeParameters, name.Arguments)));
    }

    internal class ParamRefProtectedChildrenVisitor<P> : ParamRefChildrenVisitor<P>, ITDParamRefVisitor<IArgIdentifier<string, P>, IEnumerable<ITDParamRef<INamespaceSpec, P>>, IEnumerable<IParamRef<INamespaceSpec, P>>, P> where P : ITypeArgument, ISubstitutable<P>, INamedReference
    {
        public override Func<ITDParamRef<ITDNamespace, P>, IArgIdentifier<string, P>, IEnumerable<ITDParamRef<INamespaceSpec, P>>> VisitTDNamespace => (elem, name) => elem.Element.ProtectedChildren.Where(chld => chld.Name == name.Name && chld.TypeParameters.Count() == name.Arguments.Count()).Select(chld => chld.Visit(this, elem.PArguments.Push(chld.TypeParameters, name.Arguments)));

        public override Func<ITDParamRef<ITDClass, P>, IArgIdentifier<string, P>, IEnumerable<ITDParamRef<INamespaceSpec, P>>> VisitTDClass => (elem, name) => elem.Element.ProtectedChildren.Where(chld => chld.Name == name.Name && chld.TypeParameters.Count() == name.Arguments.Count()).Select(chld => chld.Visit(this, elem.PArguments.Push(chld.TypeParameters, name.Arguments)));

        public override Func<ITDParamRef<ITDInterface, P>, IArgIdentifier<string, P>, IEnumerable<ITDParamRef<INamespaceSpec, P>>> VisitTDInterface => (elem, name) => elem.Element.ProtectedChildren.Where(chld => chld.Name == name.Name && chld.TypeParameters.Count() == name.Arguments.Count()).Select(chld => chld.Visit(this, elem.PArguments.Push(chld.TypeParameters, name.Arguments)));

        public override Func<IParamRef<INamespaceSpec, P>, IArgIdentifier<string, P>, IEnumerable<IParamRef<INamespaceSpec, P>>> VisitNamespace => (elem, name) => elem.Element.ProtectedChildren.Where(chld => chld.Name == name.Name && chld.TypeParameters.Count() == name.Arguments.Count()).Select(chld => chld.Visit(this, elem.PArguments.Push(chld.TypeParameters, name.Arguments)));

        public override Func<IParamRef<IClassSpec, P>, IArgIdentifier<string, P>, IEnumerable<IParamRef<INamespaceSpec, P>>> VisitClass => (elem, name) => elem.Element.ProtectedChildren.Where(chld => chld.Name == name.Name && chld.TypeParameters.Count() == name.Arguments.Count()).Select(chld => chld.Visit(this, elem.PArguments.Push(chld.TypeParameters, name.Arguments)));

        public override Func<IParamRef<IInterfaceSpec, P>, IArgIdentifier<string, P>, IEnumerable<IParamRef<INamespaceSpec, P>>> VisitInterface => (elem, name) => elem.Element.ProtectedChildren.Where(chld => chld.Name == name.Name && chld.TypeParameters.Count() == name.Arguments.Count()).Select(chld => chld.Visit(this, elem.PArguments.Push(chld.TypeParameters, name.Arguments)));
    }

    internal interface ITDParamRef<out T, P> : IParamRef<T, P> where T : INamespaceSpec where P : ITypeArgument, ISubstitutable<P>
    {
        new ITDParamRef<T, Q> TransformArgs<Q>(Func<P, Q> transformer) where Q : ITypeArgument, ISubstitutable<Q>;
        Ret Visit<Arg, Ret, SRet>(ITDParamRefVisitor<Arg, Ret, SRet, P> visitor, Arg arg = default(Arg)) where Ret : SRet;
    }
    internal abstract class ParamRef<T, P> : IParamRef<T, P> where T : INamespaceSpec where P : ITypeArgument, ISubstitutable<P>
    {
        public ParamRef(T elem, ITypeEnvironment<P> args)
        {
            this.Element = elem;
            this.PArguments = args;
        }

        public ITypeEnvironment<P> PArguments { get; }

        public T Element { get; }

        public IEnumerable<ITypeArgument> Arguments => Element.AllTypeParameters.Select(tp => Substitutions[tp]);

        public ITypeEnvironment<ITypeArgument> Substitutions => PArguments.Transform<ITypeArgument>(t => t);

        public IOptional<IParameterizedSpecRef<IParameterizedSpec>> ParameterizedParent => Element.ParameterizedParent.Bind(pp => new ParameterizedSpecRef<IParameterizedSpec>(pp, pp.GetAsRef().Substitutions));

        public IParamRef<T, Q> TransformArgs<Q>(Func<P, Q> transformer) where Q : ITypeArgument, ISubstitutable<Q>
        {
            return TransformArgsImpl(transformer);
        }
        protected abstract IParamRef<T, Q> TransformArgsImpl<Q>(Func<P, Q> transformer) where Q : ITypeArgument, ISubstitutable<Q>;

        public override int GetHashCode()
        {
            return ToString().GetHashCode();
        }
        public abstract Ret Visit<Arg, Ret>(IParamRefVisitor<Arg, Ret, P> visitor, Arg arg = default(Arg));

        public IParameterizedSpecRef<T> Substitute(ITypeEnvironment<ITypeArgument> substitutions)
        {
            throw new NotImplementedException();
        }
    }
    internal abstract class TDParamRef<T, P> : ParamRef<T, P>, ITDParamRef<T, P> where T : INamespaceSpec where P : ITypeArgument, ISubstitutable<P>
    {
        public TDParamRef(T elem, ITypeEnvironment<P> args) : base(elem, args)
        {
        }
        public abstract Ret Visit<Arg, Ret, SRet>(ITDParamRefVisitor<Arg, Ret, SRet, P> visitor, Arg arg = default(Arg)) where Ret : SRet;

        public new abstract ITDParamRef<T, Q> TransformArgs<Q>(Func<P, Q> transformer) where Q : ITypeArgument, ISubstitutable<Q>;


        protected override IParamRef<T, Q> TransformArgsImpl<Q>(Func<P, Q> transformer)
        {
            return TransformArgs(transformer);
        }
    }

    internal class TDNamespaceRef<P> : TDParamRef<TDNamespace, P> where P : ITypeArgument, ISubstitutable<P>
    {
        public TDNamespaceRef(TDNamespace elem, ITypeEnvironment<P> args) : base(elem, args)
        {
        }

        public override ITDParamRef<TDNamespace, Q> TransformArgs<Q>(Func<P, Q> transformer)
        {
            return new TDNamespaceRef<Q>(Element, PArguments.Transform(transformer));
        }

        public override Ret Visit<Arg, Ret, SRet>(ITDParamRefVisitor<Arg, Ret, SRet, P> visitor, Arg arg = default(Arg))
        {
            return visitor.VisitTDNamespace(this, arg);
        }
        public override Ret Visit<Arg, Ret>(IParamRefVisitor<Arg, Ret, P> visitor, Arg arg = default(Arg))
        {
            return visitor.VisitNamespace(this, arg);
        }
    }
    internal class TDClassRef<P> : TDParamRef<TDClass, P> where P : ITypeArgument, ISubstitutable<P>
    {
        public TDClassRef(TDClass elem, ITypeEnvironment<P> args) : base(elem, args)
        {
        }
        public override ITDParamRef<TDClass, Q> TransformArgs<Q>(Func<P, Q> transformer)
        {
            return new TDClassRef<Q>(Element, PArguments.Transform(transformer));
        }

        public override Ret Visit<Arg, Ret, SRet>(ITDParamRefVisitor<Arg, Ret, SRet, P> visitor, Arg arg = default(Arg))
        {
            return visitor.VisitTDClass(this, arg);
        }

        public override Ret Visit<Arg, Ret>(IParamRefVisitor<Arg, Ret, P> visitor, Arg arg = default(Arg))
        {
            return visitor.VisitClass(this, arg);
        }
    }
    internal class TDInterfaceRef<P> : TDParamRef<TDInterface, P> where P : ITypeArgument, ISubstitutable<P>
    {
        public TDInterfaceRef(TDInterface elem, ITypeEnvironment<P> args) : base(elem, args)
        {
        }
        public override ITDParamRef<TDInterface, Q> TransformArgs<Q>(Func<P, Q> transformer)
        {
            return new TDInterfaceRef<Q>(Element, PArguments.Transform(transformer));
        }

        public override Ret Visit<Arg, Ret, SRet>(ITDParamRefVisitor<Arg, Ret, SRet, P> visitor, Arg arg = default(Arg))
        {
            return visitor.VisitTDInterface(this, arg);
        }
        public override Ret Visit<Arg, Ret>(IParamRefVisitor<Arg, Ret, P> visitor, Arg arg = default(Arg))
        {
            return visitor.VisitInterface(this, arg);
        }
    }

    internal class NamespaceRef<P> : ParamRef<INamespaceSpec, P> where P : ITypeArgument, ISubstitutable<P>
    {
        public NamespaceRef(INamespaceSpec elem, ITypeEnvironment<P> args) : base(elem, args)
        {
        }
        protected override IParamRef<INamespaceSpec, Q> TransformArgsImpl<Q>(Func<P, Q> transformer)
        {
            return new NamespaceRef<Q>(Element, PArguments.Transform(transformer));
        }

        public override Ret Visit<Arg, Ret>(IParamRefVisitor<Arg, Ret, P> visitor, Arg arg = default(Arg))
        {
            return visitor.VisitNamespace(this, arg);
        }
    }
    internal class ClassRef<P> : ParamRef<IClassSpec, P> where P : ITypeArgument, ISubstitutable<P>
    {
        public ClassRef(IClassSpec elem, ITypeEnvironment<P> args) : base(elem, args)
        {
        }
        protected override IParamRef<IClassSpec, Q> TransformArgsImpl<Q>(Func<P, Q> transformer)
        {
            return new ClassRef<Q>(Element, PArguments.Transform(transformer));
        }

        public override Ret Visit<Arg, Ret>(IParamRefVisitor<Arg, Ret, P> visitor, Arg arg = default(Arg))
        {
            return visitor.VisitClass(this, arg);
        }
    }
    internal class InterfaceRef<P> : ParamRef<IInterfaceSpec, P> where P : ITypeArgument, ISubstitutable<P>
    {
        public InterfaceRef(IInterfaceSpec elem, ITypeEnvironment<P> args) : base(elem, args)
        {
        }
        protected override IParamRef<IInterfaceSpec, Q> TransformArgsImpl<Q>(Func<P, Q> transformer)
        {
            return new InterfaceRef<Q>(Element, PArguments.Transform(transformer));
        }

        public override Ret Visit<Arg, Ret>(IParamRefVisitor<Arg, Ret, P> visitor, Arg arg = default(Arg))
        {
            return visitor.VisitInterface(this, arg);
        }
    }

    public interface ILookupContext<P, PX> where P : PX, ISubstitutable<P>, INamedReference where PX : ITypeArgument, ISubstitutable<PX>, INamedReference
    {
        IEnumerable<IParamRef<INamespaceSpec, PX>> FindChildren(IArgIdentifier<string, PX> name);
        IEnumerable<IParamRef<INamespaceSpec, PX>> FindChildren(IQName<IArgIdentifier<string, PX>> name);
        IEnumerable<IParamRef<INamespaceSpec, P>> FindChildren(IArgIdentifier<string, P> name);
        IEnumerable<IParamRef<INamespaceSpec, P>> FindChildren(IQName<IArgIdentifier<string, P>> name);
        ILookupContext<Q, QX> TransformArgs<Q, QX>(Func<P, Q> transformer) where Q : QX, ISubstitutable<Q>, INamedReference where QX : ITypeArgument, ISubstitutable<QX>, INamedReference;
        Visibility MembersVisibleAt(INamespaceSpec ns);
    }

    public interface ITDLookupContext<P, PX> : ILookupContext<P, PX> where P : PX, ISubstitutable<P>, INamedReference where PX : ITypeArgument, ISubstitutable<PX>, INamedReference
    {
        IParamRef<INamespaceSpec, P> Root { get; }
        void AddUsing(ILookupContext<P, PX> usecontext);
        ITDLookupContext<P, PX> Push(IArgIdentifier<string, P> name, IEnumerable<ILookupContext<P, PX>> protecteds);
        ITDLookupContext<P, PX> Push(IQName<IArgIdentifier<string, P>> name, IEnumerable<ILookupContext<P, PX>> protecteds);
        ITDLookupContext<Q, QX> TransformTDArgs<Q, QX>(Func<P, Q> transformer) where Q : QX, ISubstitutable<Q>, INamedReference where QX : ITypeArgument, ISubstitutable<QX>, INamedReference;

    }

    internal abstract class ALookupContext<P, PX> : ILookupContext<P, PX> where P : PX, ISubstitutable<P>, INamedReference where PX : ITypeArgument, ISubstitutable<PX>, INamedReference
    {
        protected static readonly ParamRefChildrenVisitor<P> PVisitor = new ParamRefPublicChildrenVisitor<P>();
        protected static readonly ParamRefChildrenVisitor<PX> PXVisitor = new ParamRefPublicChildrenVisitor<PX>();
        public IParamRef<INamespaceSpec, P> Root { get; }
        public ALookupContext(IParamRef<INamespaceSpec, P> root)
        {
            this.Root = root;
        }

        public abstract IEnumerable<IParamRef<INamespaceSpec, P>> FindChildren(IArgIdentifier<string, P> name);
        public IEnumerable<IParamRef<INamespaceSpec, P>> FindChildren(IQName<IArgIdentifier<string, P>> name)
        {
            foreach (var rname in GetPossibleChildPaths(name))
            {
                var wname = rname;
                if (wname.IsEmpty)
                {
                    yield return Root;
                    yield break;
                }
                else
                {
                    IEnumerable<IParamRef<INamespaceSpec, P>> refs = FindChildren(wname.Current.Elem);
                    while (wname.HasNext)
                    {
                        IQName<IArgIdentifier<string, P>> curname = wname.Next.Elem;
                        IEnumerable<IParamRef<INamespaceSpec, P>> current = refs;
                        wname = curname;
                        refs = current.SelectMany(rf => rf.Visit(PVisitor, curname.Current.Elem));
                    }
                    foreach (IParamRef<INamespaceSpec, P> rf in refs)
                    {
                        yield return rf;
                    }
                }
            }
        }

        public abstract ILookupContext<Q, QX> TransformArgs<Q, QX>(Func<P, Q> transformer) where Q : QX, ISubstitutable<Q>, INamedReference where QX : ITypeArgument, ISubstitutable<QX>, INamedReference;

        public abstract IEnumerable<IParamRef<INamespaceSpec, PX>> FindChildren(IArgIdentifier<string, PX> name);

        public IEnumerable<IParamRef<INamespaceSpec, PX>> FindChildren(IQName<IArgIdentifier<string, PX>> name)
        {
            foreach (var rname in GetPossibleChildPaths(name))
            {
                var wname = rname;
                if (wname.IsEmpty)
                {
                    yield return Root.TransformArgs<PX>(x => x);
                    yield break;
                }
                else
                {
                    IEnumerable<IParamRef<INamespaceSpec, PX>> refs = FindChildren(wname.Current.Elem);
                    while (wname.HasNext)
                    {
                        IQName<IArgIdentifier<string, PX>> curname = wname.Next.Elem;
                        IEnumerable<IParamRef<INamespaceSpec, PX>> current = refs;
                        wname = curname;
                        refs = current.SelectMany(rf => rf.Visit(PXVisitor, curname.Current.Elem));
                    }
                    foreach (IParamRef<INamespaceSpec, PX> rf in refs)
                    {
                        yield return rf;
                    }
                }
            }
        }

        public IEnumerable<IQName<IArgIdentifier<string, X>>> GetPossibleChildPaths<X>(IQName<IArgIdentifier<string, X>> name) where X : IReference
        {
            List<(string, int)> previousLevels = new List<(string, int)>() { (Root.Element.Name, Root.Element.TypeParameters.Count()) };
            var pns = Root.Element.ParentNamespace;
            while (pns.HasElem)
            {
                previousLevels.Insert(0, (pns.Elem.Name, pns.Elem.TypeParameters.Count()));
                pns = pns.Elem.ParentNamespace;
            }

            var curname = name;
            int nextstart = -1;
            bool matched = false;
            for (int i = 0; i < previousLevels.Count; i++)
            {
                var pl = previousLevels[i];
                var current = curname.Current.Elem;
                if (nextstart < 0 && curname != name && pl.Item1 == name.Current.Elem.Name && pl.Item2 == name.Current.Elem.Arguments.Count())
                {
                    nextstart = i;
                }
                if (pl.Item1 == current.Name && pl.Item2 == current.Arguments.Count())
                {
                    if (name.HasNext)
                    {
                        matched = true;
                        curname = curname.Next.Elem;
                    }
                    else
                    {
                        if (i == previousLevels.Count - 1)
                        {
                            yield return new Parser.AQName<IArgIdentifier<string, X>>(false);
                        }
                    }
                }
                else
                {
                    matched = false;
                    if (nextstart > 0)
                    {
                        i = nextstart - 1;
                        nextstart = -1;
                    }
                    curname = name;
                }
            }
            if (matched)
            {
                yield return curname;
            }
            yield return name;
        }

        public abstract Visibility MembersVisibleAt(INamespaceSpec ns);
    }

    internal class LibLookupContext<P, PX> : ALookupContext<P, PX> where P : PX, ISubstitutable<P>, INamedReference where PX : ITypeArgument, ISubstitutable<PX>, INamedReference
    {
        public LibLookupContext(IParamRef<INamespaceSpec, P> rf) : base(rf)
        {
        }

        public override IEnumerable<IParamRef<INamespaceSpec, P>> FindChildren(IArgIdentifier<string, P> name)
        {
            return Root.Visit(PVisitor, name);
        }

        public override ILookupContext<Q, QX> TransformArgs<Q, QX>(Func<P, Q> transformer)
        {
            return new LibLookupContext<Q, QX>(Root.TransformArgs<Q>(transformer));
        }

        public override IEnumerable<IParamRef<INamespaceSpec, PX>> FindChildren(IArgIdentifier<string, PX> name)
        {
            return Root.TransformArgs<PX>(x => x).Visit(PXVisitor, name);
        }

        public override Visibility MembersVisibleAt(INamespaceSpec ns)
        {
            return Visibility.Public;
        }
    }

    internal class LibRootLookupContext<P, PX> : LibLookupContext<P, PX> where P : PX, ISubstitutable<P>, INamedReference where PX : ITypeArgument, ISubstitutable<PX>, INamedReference
    {
        public readonly ILibrary Library;
        public LibRootLookupContext(ILibrary lib) : base(new NamespaceRef<P>(lib.GlobalNamespace, new TypeEnvironment<P>()))
        {
            Library = lib;
        }

        public override IEnumerable<IParamRef<INamespaceSpec, P>> FindChildren(IArgIdentifier<string, P> name)
        {
            return base.FindChildren(name).Concat(Library.FindVarargsChildren(name));
        }

        public override IEnumerable<IParamRef<INamespaceSpec, PX>> FindChildren(IArgIdentifier<string, PX> name)
        {
            return base.FindChildren(name).Concat(Library.FindVarargsChildren(name));
        }

        public override ILookupContext<Q, QX> TransformArgs<Q, QX>(Func<P, Q> transformer)
        {
            return new LibRootLookupContext<Q, QX>(Library);
        }
    }

    internal class ProtectedLookupContext<T, P, PX> : ALookupContext<P, PX> where P : PX, ISubstitutable<P>, INamedReference where PX : ITypeArgument, ISubstitutable<PX>, INamedReference
    {
        protected static readonly ParamRefProtectedChildrenVisitor<P> ProtectedPVisitor = new ParamRefProtectedChildrenVisitor<P>();
        protected static readonly ParamRefProtectedChildrenVisitor<PX> ProtectedPXVisitor = new ParamRefProtectedChildrenVisitor<PX>();
        public ProtectedLookupContext(IParamRef<INamespaceSpec, P> rf) : base(rf)
        {
        }

        public override IEnumerable<IParamRef<INamespaceSpec, P>> FindChildren(IArgIdentifier<string, P> name)
        {
            return Root.Visit(ProtectedPVisitor, name); 
        }
        public override ILookupContext<Q, QX> TransformArgs<Q, QX>(Func<P, Q> transformer)
        {
            return new ProtectedLookupContext<T, Q, QX>(Root.TransformArgs<Q>(transformer));
        }

        public override IEnumerable<IParamRef<INamespaceSpec, PX>> FindChildren(IArgIdentifier<string, PX> name)
        {
            return Root.TransformArgs<PX>(x => x).Visit(ProtectedPXVisitor, name);
        }

        public override Visibility MembersVisibleAt(INamespaceSpec ns)
        {
            if (ns == Root.Element)
            {
                return Visibility.Protected;
            }
            return Visibility.Public;
        }
    }

    internal abstract class ATDLookupContext<P, PX> : ALookupContext<P, PX>, ITDLookupContext<P, PX> where P : PX, ISubstitutable<P> where PX : ITypeArgument, ISubstitutable<PX>, INamedReference
    {
        private List<ILookupContext<P, PX>> usings = new List<ILookupContext<P, PX>>();
        public ATDLookupContext(ITDParamRef<INamespaceSpec, P> root) : base(root)
        {
        }

        protected abstract IEnumerable<ITDParamRef<TDClass, P>> GetTDClassChildren(IArgIdentifier<string, P> name);
        protected abstract IEnumerable<ITDParamRef<TDInterface, P>> GetTDInterfaceChildren(IArgIdentifier<string, P> name);
        protected abstract IEnumerable<ITDParamRef<TDNamespace, P>> GetTDNamespaceChildren(IArgIdentifier<string, P> name);
        protected abstract IEnumerable<IParamRef<INamespaceSpec, P>> GetOtherChildren(IArgIdentifier<string, P> name);

        protected abstract IEnumerable<ITDParamRef<TDClass, PX>> GetTDClassChildren(IArgIdentifier<string, PX> name);
        protected abstract IEnumerable<ITDParamRef<TDInterface, PX>> GetTDInterfaceChildren(IArgIdentifier<string, PX> name);
        protected abstract IEnumerable<ITDParamRef<TDNamespace, PX>> GetTDNamespaceChildren(IArgIdentifier<string, PX> name);
        protected abstract IEnumerable<IParamRef<INamespaceSpec, PX>> GetOtherChildren(IArgIdentifier<string, PX> name);
        public override IEnumerable<IParamRef<INamespaceSpec, P>> FindChildren(IArgIdentifier<string, P> name)
        {
            return GetTDClassChildren(name).Cast<IParamRef<INamespaceSpec, P>>().Concat(GetTDInterfaceChildren(name)).Concat(GetTDNamespaceChildren(name)).Concat(GetOtherChildren(name)).Concat(usings.SelectMany(u => u.FindChildren(name)));
        }
        public override IEnumerable<IParamRef<INamespaceSpec, PX>> FindChildren(IArgIdentifier<string, PX> name)
        {
            return GetTDClassChildren(name).Cast<IParamRef<INamespaceSpec, PX>>().Concat(GetTDInterfaceChildren(name)).Concat(GetTDNamespaceChildren(name)).Concat(GetOtherChildren(name)).Concat(usings.SelectMany(u => u.FindChildren(name)));
        }


        public abstract ITDLookupContext<P, PX> Push(IArgIdentifier<string, P> name, IEnumerable<ILookupContext<P, PX>> protecteds);
        public ITDLookupContext<P, PX> Push(IQName<IArgIdentifier<string, P>> name, IEnumerable<ILookupContext<P, PX>> protecteds)
        {
            if (!name.IsEmpty)
            {
                ITDLookupContext<P, PX> child = Push(name.Current.Elem, protecteds);
                if (name.HasNext)
                {
                    return child.Push(name.Next.Elem, protecteds);
                }
                return child;
            }
            return this;
        }

        public abstract ITDLookupContext<Q, QX> TransformTDArgs<Q, QX>(Func<P, Q> transformer) where Q : QX, ISubstitutable<Q>, INamedReference where QX : ITypeArgument, ISubstitutable<QX>, INamedReference;

        public void AddUsing(ILookupContext<P, PX> usecontext)
        {
            usings.Add(usecontext);
        }
        public override Visibility MembersVisibleAt(INamespaceSpec ns)
        {
            if (ns == Root.Element)
            {
                return Visibility.Private;
            }
            return usings.Aggregate(Visibility.Public, (viz, context) => { var cviz = context.MembersVisibleAt(ns); return (cviz < viz) ? viz : cviz; });
        }

    }
    internal class TDNamespaceLookupContext<P, PX> : ATDLookupContext<P, PX> where P : PX, ISubstitutable<P> where PX : ITypeArgument, ISubstitutable<PX>, INamedReference
    {
        public IEnumerable<ILookupContext<P, PX>> Siblings { get; }
        protected ITDParamRef<ATDContainer, P> TDRoot { get; }
        public TDNamespaceLookupContext(ITDParamRef<ATDContainer, P> root, IEnumerable<ILookupContext<P, PX>> siblings) : base(root)
        {
            this.Siblings = siblings;
            this.TDRoot = root;

        }


        public override ITDLookupContext<P, PX> Push(IArgIdentifier<string, P> name, IEnumerable<ILookupContext<P, PX>> protecteds)
        {
            var nskids = GetTDNamespaceChildren(name);
            if (nskids.Any(x => true))
            {
                var sibskids = Siblings.SelectMany(s => s.FindChildren(name)).Where(sib => sib.Element.Visit(new NamespaceSpecVisitor<object, bool>((ns, o) => true, (iface, o) => false, (cls, o) => false))).Select(rf => new LibLookupContext<P, PX>(rf));
                var newlookup = new TDNamespaceLookupContextChild<P, PX>(nskids.First(), sibskids, this);
                foreach (var plookup in protecteds)
                {
                    newlookup.AddUsing(plookup);
                }
                return newlookup;
            }
            else
            {
                var otherkids = GetTDInterfaceChildren(name).Cast<ITDParamRef<ATDContainer, P>>().Concat(GetTDClassChildren(name));
                if (otherkids.Any(x => true))
                {
                    var newlookup = new TDNamespaceLookupContextChild<P, PX>(otherkids.First(), new List<ILookupContext<P, PX>>(), this);
                    foreach (var plookup in protecteds)
                    {
                        newlookup.AddUsing(plookup);
                    }
                    return newlookup;
                }
            }
            throw new InternalException("Could not push lookup context because child context $0 was not found!", name);
        }


        protected override IEnumerable<ITDParamRef<TDClass, P>> GetTDClassChildren(IArgIdentifier<string, P> name)
        {
            return TDRoot.Element.TDClasses.Where(cls => cls.Name == name.Name && cls.TypeParameters.Count() == name.Arguments.Count()).Select(cls => new TDClassRef<P>(cls, TDRoot.PArguments.Push(cls.TypeParameters, name.Arguments)));
        }

        protected override IEnumerable<ITDParamRef<TDInterface, P>> GetTDInterfaceChildren(IArgIdentifier<string, P> name)
        {
            return TDRoot.Element.TDInterfaces.Where(cls => cls.Name == name.Name && cls.TypeParameters.Count() == name.Arguments.Count()).Select(cls => new TDInterfaceRef<P>(cls, TDRoot.PArguments.Push(cls.TypeParameters, name.Arguments)));
        }

        protected override IEnumerable<ITDParamRef<TDNamespace, P>> GetTDNamespaceChildren(IArgIdentifier<string, P> name)
        {
            return TDRoot.Element.TDNamespaces.Where(ns => ns.Name == name.Name && ns.TypeParameters.Count() == name.Arguments.Count() && name.Arguments.Count() == 0).Select(ns => new TDNamespaceRef<P>(ns, TDRoot.PArguments));
        }

        protected override IEnumerable<IParamRef<INamespaceSpec, P>> GetOtherChildren(IArgIdentifier<string, P> name)
        {
            return Siblings.Select(s => s.FindChildren(name)).Flatten();
        }

        protected override IEnumerable<ITDParamRef<TDClass, PX>> GetTDClassChildren(IArgIdentifier<string, PX> name)
        {
            return TDRoot.Element.TDClasses.Where(cls => cls.Name == name.Name && cls.TypeParameters.Count() == name.Arguments.Count()).Select(cls => new TDClassRef<PX>(cls, TDRoot.PArguments.Transform<PX>(x => x).Push(cls.TypeParameters, name.Arguments)));
        }

        protected override IEnumerable<ITDParamRef<TDInterface, PX>> GetTDInterfaceChildren(IArgIdentifier<string, PX> name)
        {
            return TDRoot.Element.TDInterfaces.Where(cls => cls.Name == name.Name && cls.TypeParameters.Count() == name.Arguments.Count()).Select(cls => new TDInterfaceRef<PX>(cls, TDRoot.PArguments.Transform<PX>(x => x).Push(cls.TypeParameters, name.Arguments)));
        }

        protected override IEnumerable<ITDParamRef<TDNamespace, PX>> GetTDNamespaceChildren(IArgIdentifier<string, PX> name)
        {
            return TDRoot.Element.TDNamespaces.Where(ns => ns.Name == name.Name && ns.TypeParameters.Count() == name.Arguments.Count() && name.Arguments.Count() == 0).Select(ns => new TDNamespaceRef<PX>(ns, TDRoot.PArguments.Transform<PX>(x => x)));
        }

        protected override IEnumerable<IParamRef<INamespaceSpec, PX>> GetOtherChildren(IArgIdentifier<string, PX> name)
        {
            return Siblings.Select(s => s.FindChildren(name)).Flatten();
        }

        public override ILookupContext<Q, QX> TransformArgs<Q, QX>(Func<P, Q> transformer)
        {
            return TransformTDArgs<Q, QX>(transformer);
        }

        public override ITDLookupContext<Q, QX> TransformTDArgs<Q, QX>(Func<P, Q> transformer)
        {
            return new TDNamespaceLookupContext<Q, QX>(TDRoot.TransformArgs<Q>(transformer), Siblings.Select(s => s.TransformArgs<Q, QX>(transformer)));
        }
    }
    internal class TDNamespaceLookupContextChild<P, PX> : TDNamespaceLookupContext<P, PX> where P : PX, ISubstitutable<P> where PX : ITypeArgument, ISubstitutable<PX>, INamedReference
    {
        public ILookupContext<P, PX> Parent { get; }
        public TDNamespaceLookupContextChild(ITDParamRef<ATDContainer, P> root, IEnumerable<ILookupContext<P, PX>> siblings, ILookupContext<P, PX> parent) : base(root, siblings)
        {
            Parent = parent;
        }
        protected override IEnumerable<IParamRef<INamespaceSpec, P>> GetOtherChildren(IArgIdentifier<string, P> name)
        {
            return base.GetOtherChildren(name).Concat(Parent.FindChildren(name));
        }
        protected override IEnumerable<IParamRef<INamespaceSpec, PX>> GetOtherChildren(IArgIdentifier<string, PX> name)
        {
            return base.GetOtherChildren(name).Concat(Parent.FindChildren(name));
        }

        public override ITDLookupContext<Q, QX> TransformTDArgs<Q, QX>(Func<P, Q> transformer)
        {
            return new TDNamespaceLookupContextChild<Q, QX>(TDRoot.TransformArgs<Q>(transformer), Siblings.Select(s => s.TransformArgs<Q, QX>(transformer)), Parent.TransformArgs<Q, QX>(transformer));
        }

        public override Visibility MembersVisibleAt(INamespaceSpec ns)
        {
            Visibility ret = base.MembersVisibleAt(ns);
            Visibility parentVis = Parent.MembersVisibleAt(ns);
            if (parentVis < ret)
            {
                return parentVis;
            }
            return ret;
        }
    }

    public class TypeCheckLookup<P, PX> : ITypeCheckLookup<P, PX> where P : PX, ISubstitutable<P> where PX : ITypeArgument, ISubstitutable<PX>, INamedReference
    {
        public readonly Program Program;
        internal ITDChild Container { get; }
        private ITDLookupContext<P, PX> context;
        private Dictionary<string, ITypeParameterSpec> typeVariables;
        public ITypeEnvironment<IType> TypeEnvironment
        {
            get
            {
                return new TypeEnvironment<IType>(typeVariables.Values.ToDictionary<ITypeParameterSpec, ITypeParameterSpec, IType>(tps => tps, tps => new TypeVariable(tps)));
            }
        }
        public IParamRef<INamespaceSpec, P> ContextRoot
        {
            get
            {
                return context.Root;
            }
        }

        ITDChild ITypeCheckLookup<P, PX>.Container => Container;

        public TypeCheckLookup(Program program)
        {
            Program = program;
            Container = program.GlobalNS;
            typeVariables = new Dictionary<string, ITypeParameterSpec>();
            context = new TDNamespaceLookupContext<P, PX>(new TDNamespaceRef<P>(program.GlobalNS, new TypeEnvironment<P>()), program.Libraries.Select(lib => new LibRootLookupContext<P, PX>(lib)));
        }
        private TypeCheckLookup(ITDChild container, ITDLookupContext<P, PX> context, Dictionary<string, ITypeParameterSpec> typeEnv)
        {
            Container = container;
            this.context = context;
            this.typeVariables = typeEnv;
        }

        public bool HasChild(IQName<IArgIdentifier<string, P>> name)
        {
            var results = context.FindChildren(name).GetEnumerator();
            return results.MoveNext();
        }

        public IParamRef<INamespaceSpec, P> GetChild(IQName<IArgIdentifier<string, P>> name)
        {
            var results = context.FindChildren(name).GetEnumerator();
            if (!results.MoveNext())
            {
                throw new TypeCheckException("$0 not found!", name);
            }
            var ret = results.Current;
            while (results.MoveNext())
            {
                if (ret.Element == results.Current.Element)
                {
                    continue;
                }
                StringBuilder sb = new StringBuilder("$0 is an ambiguous reference. " + ret.Element.FullQualifiedName + " chosen. Other potential candidates are: " + results.Current.Element.FullQualifiedName);
                while (results.MoveNext())
                {
                    sb.Append(", ");
                    sb.Append(results.Current.Element.FullQualifiedName);
                }
                CompilerOutput.Warn(sb.ToString(), name);
                break;
            }
            return ret;
        }
        public IParamRef<INamespaceSpec, P> GetChild(IArgIdentifier<string, P> name)
        {
            var results = context.FindChildren(name).GetEnumerator();
            if (!results.MoveNext())
            {
                throw new TypeCheckException("$0 not found!", name);
            }
            var ret = results.Current;
            while (results.MoveNext())
            {
                if (ret.Element == results.Current.Element)
                {
                    continue;
                }
                StringBuilder sb = new StringBuilder("$0 is an ambiguous reference. " + ret.Element.FullQualifiedName + " chosen. Other potential candidates are: " + results.Current.Element.FullQualifiedName);
                while (results.MoveNext())
                {
                    sb.Append(", ");
                    sb.Append(results.Current.Element.FullQualifiedName);
                }
                CompilerOutput.Warn(sb.ToString(), name);
                break;
            }
            return ret;
        }

        public IParamRef<INamespaceSpec, PX> GetChild(IQName<IArgIdentifier<string, PX>> name)
        {
            var results = context.FindChildren(name).GetEnumerator();
            if (!results.MoveNext())
            {
                throw new TypeCheckException("$0 not found!", name);
            }
            var ret = results.Current;
            while (results.MoveNext())
            {
                if (ret.Element == results.Current.Element)
                {
                    continue;
                }
                StringBuilder sb = new StringBuilder("$0 is an ambiguous reference. " + ret.Element.FullQualifiedName + " chosen. Other potential candidates are: " + results.Current.Element.FullQualifiedName);
                while (results.MoveNext())
                {
                    sb.Append(", ");
                    sb.Append(results.Current.Element.FullQualifiedName);
                }
                CompilerOutput.Warn(sb.ToString(), name);
                break;
            }
            return ret;
        }
        public IParamRef<INamespaceSpec, PX> GetChild(IArgIdentifier<string, PX> name)
        {
            var results = context.FindChildren(name).GetEnumerator();
            if (!results.MoveNext())
            {
                throw new TypeCheckException("$0 not found!", name);
            }
            var ret = results.Current;
            while (results.MoveNext())
            {
                if (ret.Element == results.Current.Element)
                {
                    continue;
                }
                StringBuilder sb = new StringBuilder("$0 is an ambiguous reference. " + ret.Element.FullQualifiedName + " chosen. Other potential candidates are: " + results.Current.Element.FullQualifiedName);
                while (results.MoveNext())
                {
                    sb.Append(", ");
                    sb.Append(results.Current.Element.FullQualifiedName);
                }
                CompilerOutput.Warn(sb.ToString(), name);
                break;
            }
            return ret;
        }

        public void AddUsing(ILookupContext<P, PX> usecontext)
        {
            context.AddUsing(usecontext);
        }

        public ITypeCheckLookup<P, PX> PushVariables(IEnumerable<ITypeParameterSpec> typeParams)
        {
            Dictionary<string, ITypeParameterSpec> newTypeEnv = new Dictionary<string, ITypeParameterSpec>(typeVariables);
            foreach (ITypeParameterSpec tps in typeParams)
            {
                newTypeEnv[tps.Name] = tps;
            }
            return new TypeCheckLookup<P, PX>(Container, context, newTypeEnv);
        }

        public IOptional<ITypeParameterSpec> GetTypeVariable(string name)
        {
            if (typeVariables.ContainsKey(name))
            {
                return typeVariables[name].InjectOptional();
            }
            return Optional<ITypeParameterSpec>.Empty;
        }

        internal ITypeCheckLookup<P, PX> Push(ITDChild container, IQName<IArgIdentifier<string, P>> name, IEnumerable<ILookupContext<P, PX>> protecteds)
        {
            return new TypeCheckLookup<P, PX>(container, context.Push(name, protecteds), typeVariables);
        }
        internal ITypeCheckLookup<P, PX> Push(ITDChild container, IArgIdentifier<string, P> name, IEnumerable<ILookupContext<P, PX>> protecteds)
        {
            return new TypeCheckLookup<P, PX>(container, context.Push(name, protecteds), typeVariables);
        }

        public Visibility MembersVisibleAt(INamespaceSpec ns)
        {
            return context.MembersVisibleAt(ns);
        }

        ITypeCheckLookup<P, PX> ITypeCheckLookup<P, PX>.Push(ITDChild container, IQName<IArgIdentifier<string, P>> name, IEnumerable<ILookupContext<P, PX>> protecteds)
        {
            return Push(container, name, protecteds);
        }

        ITypeCheckLookup<P, PX> ITypeCheckLookup<P, PX>.Push(ITDChild container, IArgIdentifier<string, P> name, IEnumerable<ILookupContext<P, PX>> protecteds)
        {
            return Push(container, name, protecteds);
        }
    }

}
