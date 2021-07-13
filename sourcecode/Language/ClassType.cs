using System;
using System.Collections.Generic;
using System.Text;
using System.Linq;

namespace Nom.Language
{
    public class ClassType : ANamedType
    {
        private static ClassType nullType = null;
        public static ClassType NullType
        {
            set
            {
                if(value==null||nullType!=null)
                {
                    throw new InternalException("Standard null type must be set exactly once to a non-null reference");
                }
                nullType = value;
            }
            get
            {
                if(nullType==null)
                {
                    throw new InternalException("Null type requested by not instantiated!");
                }
                return nullType;
            }
        }

        public ClassType(IClassSpec cls, ITypeEnvironment<ITypeArgument> typeArgs) : base(typeArgs)
        {
            this.Class = cls;
        }
        public IClassSpec Class
        {
            get;
            private set;
        }

        public override IInterfaceSpec Element
        {
            get
            {
                return Class;
            }
        }

        public override IEnumerable<IType> InheritsFrom
        {
            get
            {
                yield return this;
                foreach (var super in Class.SuperClass.AsEnumerable())
                {
                    foreach (var super_ihf in new ClassType(super.Element, super.PArguments.Transform<ITypeArgument>(x => x)).Substitute<Language.ITypeArgument>(this.Substitutions).InheritsFrom)
                    {
                        yield return super_ihf;
                    }
                }
                foreach (var super in this.Element.Implements)
                {
                    foreach (var super_ihf in new InterfaceType(super.Element, super.PArguments.Transform<ITypeArgument>(x => x)).Substitute<Language.ITypeArgument>(this.Substitutions).InheritsFrom)
                    {
                        yield return super_ihf;
                    }
                }
            }
        }

        public override IOptional<INamedType> Instantiate(IInterfaceSpec iface)
        {
            return base.Instantiate(iface).Coalesce(() =>
            {
                if (Class.SuperClass.HasElem && Class.SuperClass.Elem.Element == iface)
                {
                    return iface.Instantiate(Class.SuperClass.Elem.PArguments.Transform<ITypeArgument>(x => x).Substitute<ITypeArgument>(this.Substitutions)).InjectOptional();
                }
                else if (Class.SuperClass.HasElem)
                {
                    return Instantiate(Class.SuperClass.Elem.Element).Join(nt => nt.Instantiate(iface));
                }
                return Optional<INamedType>.Empty;
            });
        }
        public override bool IsDisjoint(IType other)
        {
            NoargTypeVisitor<bool> tv = new NoargTypeVisitor<bool>((t) => t.IsDisjoint(this));
            tv.VisitInterfaceType = (t) => false;
            tv.VisitClassType = (t) => !t.IsSubtypeOf(this) || this.IsSubtypeOf(t);
            return other.Visit(tv);
        }

        public override INamedType Substitute(ITypeParameterSpec param, IType type)
        {
            return new ClassType(this.Class, Substitutions.Substitute(param, type));
        }

        public override Ret Visit<Arg, Ret>(ITypeVisitor<Arg, Ret> visitor, Arg arg = default(Arg))
        {
            return visitor.VisitClassType(this, arg);
        }

        public override INamedType MeetInstantiation(INamedType other)
        {
            if (!other.Element.Equals(this.Element))
            {
                throw new InternalException("Cannot create instantiation meet of two types referring to different classes");
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
            return new ClassType(this.Class, Substitutions.Substitute(env));
        }
        public override bool PrecisionRelated(IType other)
        {
            return other.Visit(new TypeVisitor<object, bool>()
            {
                DefaultAction = (x, o) => false,
                VisitDynamicType = (d, o) => true,
                VisitClassType = (c, o) => c.Class == this.Class && this.Class.IsShape == false && Substitutions.Zip(c.Substitutions, (kvpl, kvpr) => kvpl.Value.PrecisionRelated(kvpr.Value)).All(x => x)
            });
        }

        public override bool LessOptimistic(IType other)
        {
            return other.Visit(new TypeVisitor<object, bool>()
            {
                DefaultAction = (x, o) => false,
                VisitDynamicType = (d, o) => true,
                VisitClassType = (c, o) => c.Class == this.Class && this.Class.IsShape == false && Substitutions.Zip(c.Substitutions, (kvpl, kvpr) => kvpl.Value.LessOptimistic(kvpr.Value)).All(x => x)
            });
        }

        public override IParamRef<IInterfaceSpec, Q> TransformArgs<Q>(Func<ITypeArgument, Q> transformer)
        {
            return new ClassRef<Q>(Class, Substitutions.Transform(transformer));
        }
    }
}
