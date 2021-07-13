using System;
using System.Collections.Generic;
using System.Text;
using System.Linq;
using Nom.Language;

namespace Nom.TypeChecker
{
    public class TDTypeArgDeclDef : ITypeParameterSpec
    {
        public TDTypeArgDeclDef(Parser.Identifier name, int index, IType upperBound=null, IType lowerBound=null)
        {
            Identifier = name;
            //Variance = variance;
            Index = index;
            this.UpperBound = upperBound ?? TopType.Instance;
            this.LowerBound = lowerBound ?? BotType.Instance;
        }
        public Parser.Identifier Identifier
        {
            get;
            private set;
        }

        public string Name => Identifier.Name;

        
        public IType UpperBound
        {
            get;
            private set;
        }
        
        public IType LowerBound
        {
            get;
            private set;
        }

        public void AdjustBounds(IType upperBound = null, IType lowerBound = null)
        {
            this.UpperBound = upperBound ?? this.UpperBound;
            this.LowerBound = lowerBound ?? this.LowerBound;
        }
        public int Index { get; }


        public override bool Equals(object obj)
        {
            ITypeParameterSpec other = obj as ITypeParameterSpec;
            if (other != null)
            {
                return other.Index == Index && other.Parent == Parent;
            }
            return false;
        }

        public override int GetHashCode()
        {
            var hashCode = 1108852512;
            hashCode = hashCode * -1521134295 + Index.GetHashCode();
            hashCode = hashCode * -1521134295 + EqualityComparer<IParameterizedSpec>.Default.GetHashCode(Parent);
            return hashCode;
        }

        private IParameterizedSpec parent;

        public IParameterizedSpec Parent
        {
            get
            {
                return parent;
            }
            set
            {
                if(parent!=null)
                {
                    throw new InternalException("Cannot set type parameter parent more than once!");
                }
                if(value==null)
                {
                    throw new InternalException("Cannot set type parameter parent to null!");
                }
                parent = value;
            }
        }
    }
}
