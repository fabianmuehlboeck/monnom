using System;
using System.Collections.Generic;
using System.Text;
using System.Linq;
using Nom.Language.SpecExtensions;

namespace Nom.Language
{
    public abstract class ANamedType : AType, INamedType
    {
        protected ANamedType(ITypeEnvironment<ITypeArgument> arguments)
        {
            this.Substitutions = arguments;
        }
        public abstract IInterfaceSpec Element
        {
            get;
        }

        public ITypeEnvironment<ITypeArgument> Substitutions
        {
            get;
            private set;
        }

        public override string ReferenceName => Element.Name + (Substitutions.Count()>0? "<" + String.Join(", ", Substitutions.Select(arg => arg.Value.ReferenceName)) + ">":"");

        public ITypeEnvironment<ITypeArgument> PArguments => Substitutions;

        public IEnumerable<ITypeArgument> Arguments => Element.AllTypeParameters.Select(tps => Substitutions[tps]);

        public IOptional<IParameterizedSpecRef<IParameterizedSpec>> ParameterizedParent => Element.ParentNamespace.Bind(pns => pns.Visit(new NamespaceSpecVisitor<object, IParameterizedSpecRef<IParameterizedSpec>>(
            (ns, o) => new ParameterizedSpecRef<INamespaceSpec>(ns, Substitutions.Restrict(ns)), 
            (iface, o) => new ParameterizedSpecRef<IInterfaceSpec>(iface, Substitutions.Restrict(iface)),
            (cls, o) => new ParameterizedSpecRef<IClassSpec>(cls, Substitutions.Restrict(cls)))));


        public virtual IOptional<INamedType> Instantiate(IInterfaceSpec iface)
        {
            if(iface.Equals(Element))
            {
                return (this).InjectOptional();
            }
            else
            {
                foreach(var super in Element.Implements)
                {
                    if(super.Element==iface)
                    {
                        return new InterfaceType(super.Element, super.PArguments.Transform<ITypeArgument>(x => x).Substitute<ITypeArgument>(this.Substitutions)).InjectOptional();
                    }
                    var superInst = Instantiate(super.Element).Join(nt => nt.Instantiate(iface));
                    if(superInst.HasElem)
                    {
                        return superInst; //TODO: actually look for "best" instantiation
                    }
                }
                return Optional<INamedType>.Empty;
            }
        }

        public override bool IsEquivalent(IType other, bool optimistic = false)
        {
            var visitor = new TypeVisitor<object, bool>();
            visitor.DefaultAction = (t, a) => t.IsEquivalent(this, optimistic);
            visitor.VisitTopType = (t, a) => false;
            visitor.VisitBotType = (t, a) => false;
            visitor.VisitTypeVariable = (t, a) => false;
            visitor.VisitMaybeType = (t, a) => false;
            visitor.VisitProbablyType = (t, a) => false;
            visitor.VisitDynamicType = (t, a) => optimistic;
            visitor.NamedTypeAction = (nt, a) =>
            {
                if (nt.Element != this.Element)
                {
                    return false;
                }
                foreach (var pair in nt.Arguments.Zip(Arguments, (left, right) => (left, right)))
                {
                    if (!pair.left.AsType.IsEquivalent(pair.right.AsType, optimistic))
                    {
                        return false;
                    }
                }
                return true;
            };
            return other.Visit<object, bool>(visitor);
        }

        public override bool IsSubtypeOf(IType other, bool optimistic = false)
        {
            var visitor = new TypeVisitor<object, bool>();
            visitor.DefaultAction = (t, a) => t.IsSupertypeOf(this, optimistic);
            visitor.VisitTopType = (t, a) => true;
            visitor.VisitBotType = (t, a) => false;
            visitor.NamedTypeAction = (nt, a) =>
            {
                IOptional<INamedType> superInstance = this.Instantiate(nt.Element);
                return superInstance.Extract(si => 
                {
                    foreach(KeyValuePair<ITypeParameterSpec, ITypeArgument> kvp in si.Substitutions)
                    {
                        if(!kvp.Value.AsType.IsEquivalent(nt.Substitutions[kvp.Key].AsType, optimistic))
                        {
                            return false;
                        }
                    }
                    return true;
                }
                , false);
            };
            return other.Visit<object, bool>(visitor);
        }
        
        public override bool IsSupertypeOf(IType other, bool optimistic = false)
        {
            var visitor = new TypeVisitor<object, bool>();
            visitor.DefaultAction = (t, a) => t.IsSubtypeOf(this, optimistic);
            visitor.VisitTopType = (t, a) => false;
            visitor.VisitBotType = (t, a) => true;
            return other.Visit<object, bool>(visitor);
        }
        
        public abstract INamedType MeetInstantiation(INamedType other);
        public new abstract INamedType Substitute(ITypeParameterSpec param, IType type);
        public new abstract INamedType Substitute<T>(ITypeEnvironment<T> env) where T : ITypeArgument, ISubstitutable<T>;

        public IParameterizedSpecRef<IInterfaceSpec> Substitute(ITypeEnvironment<ITypeArgument> substitutions)
        {
            return this.Substitute<ITypeArgument>(substitutions);
        }

        public abstract IParamRef<IInterfaceSpec, Q> TransformArgs<Q>(Func<ITypeArgument, Q> transformer) where Q : ITypeArgument, ISubstitutable<Q>;

        public Ret Visit<Arg, Ret>(IParamRefVisitor<Arg, Ret, ITypeArgument> visitor, Arg arg = default)
        {
            return Element.Visit(new NamespaceSpecVisitor<Arg, Ret>(
                (ns, aarg) => visitor.VisitNamespace((IParamRef<INamespaceSpec, ITypeArgument>)this, aarg),
                (iface, aarg) => visitor.VisitInterface((IParamRef<IInterfaceSpec, ITypeArgument>)this, aarg),
                (cls, aarg) => visitor.VisitClass(new ClassRef<ITypeArgument>(cls, Substitutions), aarg)), arg);
        }

        protected override IType SubstituteType(ITypeParameterSpec param, IType type)
        {
            return this.Substitute(param, type);
        }
        protected override IType SubstituteType<T>(ITypeEnvironment<T> env)
        {
            return this.Substitute(env);
        }
    }
}
