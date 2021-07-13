using Nom.Language;
using System;
using System.Collections.Generic;
using System.Text;

namespace Nom.TypeChecker
{
    public class TDTypeArgDeclRef : ITypeParameterSpec
    {
        public readonly ITypeParameterSpec Reference;

        public ITypeParameterSpec Original => (Reference as TDTypeArgDeclRef)?.Original ?? Reference;

        public TDTypeArgDeclRef(ITypeParameterSpec reference, IType upperBound=null, IType lowerBound =null)
        {
            this.Reference = reference;
            this.upperBound = upperBound;
            this.lowerBound = lowerBound;
        }

        public override bool Equals(object obj)
        {
            return obj.Equals(Original);
        }
        public override int GetHashCode()
        {
            return Original.GetHashCode();
        }

        public int Index => Reference.Index;

        public string Name => Reference.Name;

        private IType upperBound = null;
        private IType lowerBound = null;
        public IType UpperBound => upperBound ?? Reference.UpperBound;
        public IType LowerBound => lowerBound ?? Reference.LowerBound;

        public IParameterizedSpec Parent
        {
            get
            {
                return Reference.Parent;
            }
            set
            {
                throw new InternalException("Cannot set type parameter parent through reference!");
            }
        }

        public void AdjustBounds(IType upperBound = null, IType lowerBound = null)
        {
            this.upperBound = upperBound;
            this.lowerBound = lowerBound;
        }
    }
}
