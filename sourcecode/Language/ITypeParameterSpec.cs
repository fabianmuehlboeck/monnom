using System;
using System.Collections.Generic;
using System.Text;

namespace Nom.Language
{
    public interface ITypeParameterSpec : ISpec
    {
        /// <summary>
        /// Can only be set once
        /// </summary>
        IParameterizedSpec Parent
        {
            get;
            set;
        }
        int Index
        {
            get;
        }
        String Name
        {
            get;
        }
        IType UpperBound
        {
            get;
        }
        IType LowerBound
        {
            get;
        }
        void AdjustBounds(IType upperBound = null, IType lowerBound = null);
    }
}
