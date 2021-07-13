using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Nom.Language
{
    public abstract class AType : IType
    {
        //public IType CovariantPart => this;

        //public virtual IType ContravariantPart => this;

        public abstract string ReferenceName { get; }

        public IType AsType => this;

        public abstract IEnumerable<IType> InheritsFrom { get; }

        IEnumerable<ITypeArgument> ITypeArgument.InheritsFrom => InheritsFrom;

        public abstract bool IsDisjoint(IType other);

        public abstract bool IsSubtypeOf(IType other, bool optimistic = false);

        //public bool IsSubtypeOf(ITypeArgument other, bool optimistic = false)
        //{
        //    return CovariantPart.IsSubtypeOf(other.CovariantPart, optimistic) && other.ContravariantPart.IsSubtypeOf(ContravariantPart, optimistic);
        //}

        public bool IsSubtypeOf(ITypeArgument other, bool optimistic = false)
        {
            return this.AsType.IsSubtypeOf(other.AsType, optimistic);
        }

        public abstract bool IsSupertypeOf(IType other, bool optimistic = false);

        public abstract IType Join(IType other);

        public ITypeArgument Join(ITypeArgument other)
        {
            //if(other.CovariantPart==other.ContravariantPart)
            //{
            //    return Join(other.CovariantPart);
            //}
            //return new TypeArgument(Join(other.CovariantPart), Meet(other.ContravariantPart));
            return Join(other.AsType);
        }

        public abstract IType Meet(IType other);

        public ITypeArgument Meet(ITypeArgument other)
        {
            //if (other.CovariantPart == other.ContravariantPart)
            //{
            //    return Join(other.CovariantPart);
            //}
            //return new TypeArgument(Meet(other.CovariantPart), Join(other.ContravariantPart));
            return Meet(other.AsType);
        }

        public IType Substitute(ITypeParameterSpec param, IType type)
        {
            return SubstituteType(param, type);
        }
        protected abstract IType SubstituteType(ITypeParameterSpec param, IType type);
        public IType Substitute<T>(ITypeEnvironment<T> env) where T : ITypeArgument, ISubstitutable<T>
        {
            return SubstituteType(env);
        }
        protected abstract IType SubstituteType<T>(ITypeEnvironment<T> env) where T : ITypeArgument, ISubstitutable<T>;

        public abstract Ret Visit<Arg, Ret>(ITypeVisitor<Arg, Ret> visitor, Arg arg = default(Arg));

        ITypeArgument ISubstitutable<ITypeArgument>.Substitute(ITypeParameterSpec param, IType type)
        {
            return Substitute(param, type);
        }
        ITypeArgument ISubstitutable<ITypeArgument>.Substitute<T>(ITypeEnvironment<T> env)
        {
            return SubstituteType(env);
        }


        public abstract bool PrecisionRelated(IType other);

        public abstract bool LessOptimistic(IType other);

        public bool PessimisticSubtype(IType other)
        {
            return InheritsFrom.Any(t => t.LessOptimistic(other));
        }

        public bool OptimisticSubtype(IType other)
        {
            return InheritsFrom.Any(t => t.PrecisionRelated(other));
        }

        public bool SubstitutiveSubtype(IType other)
        {
            return InheritsFrom.Any(t => other.LessOptimistic(t));
        }

        public bool PrecisionRelated(ITypeArgument other)
        {
            return PrecisionRelated(other.AsType);
        }

        public bool LessOptimistic(ITypeArgument other)
        {
            return LessOptimistic(other.AsType);
        }

        public bool PessimisticSubtype(ITypeArgument other)
        {
            return PessimisticSubtype(other.AsType);
        }

        public bool OptimisticSubtype(ITypeArgument other)
        {
            return OptimisticSubtype(other.AsType);
        }

        public bool SubstitutiveSubtype(ITypeArgument other)
        {
            return SubstitutiveSubtype(other.AsType);
        }

        public void PrettyPrint(PrettyPrinter p) { throw new NotImplementedException(); }
    }
}
