using System;
using System.Collections.Generic;
using System.Text;
using System.Linq;

namespace Nom.Language
{
    public class InterfaceType : ANamedType
    {
        public InterfaceType(IInterfaceSpec iface, ITypeEnvironment<ITypeArgument> typeArgs) : base(typeArgs)
        {
            this.Element = iface;
        }
        public override IInterfaceSpec Element
        {
            get;
        }

        public override IEnumerable<IType> InheritsFrom
        {
            get
            {
                yield return this;
                foreach (var super in this.Element.Implements)
                {
                    foreach (var super_ihf in new InterfaceType(super.Element, super.PArguments.Transform<ITypeArgument>(x => x)).Substitute<Language.ITypeArgument>(this.Substitutions).InheritsFrom)
                    {
                        yield return super_ihf;
                    }
                }
            }
        }

        public override bool IsDisjoint(IType other)
        {
            NoargTypeVisitor<bool> tv = new NoargTypeVisitor<bool>((t) => t.IsDisjoint(this));
            tv.NamedTypeAction = t => false;
            return other.Visit(tv);
        }

        public override Ret Visit<Arg, Ret>(ITypeVisitor<Arg, Ret> visitor, Arg arg = default(Arg))
        {
            return visitor.VisitInterfaceType(this, arg);
        }

        public override INamedType Substitute(ITypeParameterSpec param, IType type)
        {
            return new InterfaceType(this.Element, Substitutions.Substitute(param, type));
        }

        public override INamedType MeetInstantiation(INamedType other)
        {
            if (!other.Element.Equals(this.Element))
            {
                throw new InternalException("Cannot create instantiation meet of two types referring to different interfaces");
            }
            return new InterfaceType(this.Element, Substitutions.Meet(other.Substitutions));
        }

        public override IType Join(IType other)
        {
            throw new NotImplementedException();
        }

        public override IType Meet(IType other)
        {
            throw new NotImplementedException();
        }

        public override INamedType Substitute<T>(ITypeEnvironment<T> env)
        {
            return new InterfaceType(this.Element, Substitutions.Substitute(env));
        }

        public override bool PrecisionRelated(IType other)
        {
            return other.Visit(new TypeVisitor<object, bool>()
            {
                DefaultAction = (x, o) => false,
                VisitDynamicType = (d, o) => true,
                VisitInterfaceType = (i, o) => i.Element == this.Element && this.Element.IsShape == false && Substitutions.Zip(i.Substitutions, (kvpl, kvpr) => kvpl.Value.PrecisionRelated(kvpr.Value)).All(x => x)
            });
        }

        public override bool LessOptimistic(IType other)
        {
            return other.Visit(new TypeVisitor<object, bool>()
            {
                DefaultAction = (x, o) => false,
                VisitDynamicType = (d, o) => true,
                VisitInterfaceType = (i, o) => i.Element == this.Element && this.Element.IsShape == false && Substitutions.Zip(i.Substitutions, (kvpl, kvpr) => kvpl.Value.LessOptimistic(kvpr.Value)).All(x => x)
            });
        }

        public override IParamRef<IInterfaceSpec, Q> TransformArgs<Q>(Func<ITypeArgument, Q> transformer)
        {
            return new InterfaceRef<Q>(Element, Substitutions.Transform(transformer));
        }
    }
}
