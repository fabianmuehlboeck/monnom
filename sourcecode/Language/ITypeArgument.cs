using System;
using System.Collections.Generic;
using System.Text;

namespace Nom.Language
{
    public interface ITypeArgument : ISubstitutable<ITypeArgument>, INamedReference
    {
        IType AsType
        {
            get;
        }
        bool IsSubtypeOf(ITypeArgument other, bool optimistic = false);
        ITypeArgument Meet(ITypeArgument other);
        ITypeArgument Join(ITypeArgument other);

        IEnumerable<ITypeArgument> InheritsFrom { get; }
        bool PrecisionRelated(ITypeArgument other);
        bool LessOptimistic(ITypeArgument other);
        bool PessimisticSubtype(ITypeArgument other);
        bool OptimisticSubtype(ITypeArgument other);
        bool SubstitutiveSubtype(ITypeArgument other);
    }
}
