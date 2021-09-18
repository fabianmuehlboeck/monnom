using System;
using System.Collections.Generic;
using System.Text;

namespace Nom.Language
{
    public interface IType : ITypeArgument, IReference, ISubstitutable<IType>
    {
        bool IsDisjoint(IType other);

        bool IsSubtypeOf(IType other, bool optimistic = false);
        bool IsSupertypeOf(IType other, bool optimistic = false);
        
        Ret Visit<Arg, Ret>(ITypeVisitor<Arg, Ret> visitor, Arg arg = default(Arg));
        IType Meet(IType other);
        IType Join(IType other);

        bool IsEquivalent(IType other, bool optimistic = false);
        new IEnumerable<IType> InheritsFrom { get; }
        bool PrecisionRelated(IType other);
        bool LessOptimistic(IType other);
        bool PessimisticSubtype(IType other);
        bool OptimisticSubtype(IType other);
        bool SubstitutiveSubtype(IType other);
        IType ReplaceArgsWith(IEnumerable<IType> args);
    }
}
