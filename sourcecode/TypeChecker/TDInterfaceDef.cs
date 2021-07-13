using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Nom.Parser;
using Nom.Language;

namespace Nom.TypeChecker
{
    internal class TDInterfaceDef
    {
        public TDInterfaceDef(Identifier name, IEnumerable<TDTypeArgDeclDef> typeArgs, bool isPartial, bool isShape, bool isMaterial, VisibilityNode visibility)
        {
            this.Name = name;
            this.TypeArguments = typeArgs;
            this.IsPartial = isPartial;
            this.IsMaterial = isMaterial;
            this.IsShape = isShape;
            this.Visibility = visibility??VisibilityNode.Internal;
        }

        public readonly Identifier Name;
        public readonly IEnumerable<TDTypeArgDeclDef> TypeArguments;
        public readonly VisibilityNode Visibility;
        public readonly bool IsPartial;
        public readonly bool IsShape;
        public readonly bool IsMaterial;

        private bool isInheritanceInitialized = false;
        public bool IsInheritanceInitialized
        {
            get
            {
                return isInheritanceInitialized;
            }
            set
            {
                //cannot be set back to false
                isInheritanceInitialized = value || isInheritanceInitialized;
            }
        }

        private List<IParamRef<IInterfaceSpec, Language.IType>> implements = new List<IParamRef<IInterfaceSpec, Language.IType>>();
        public IEnumerable<IParamRef<IInterfaceSpec, Language.IType>> Implements
        {
            get
            {
                return implements.ToList();
            }
        }

        public void AddImplements(IParamRef<IInterfaceSpec, Language.IType> it)
        {
            implements.Add(it);
        }

        public ISourceSpan Locs
        {
            get;
            private set;
        }

        private List<MethodDeclDef> declaredMethods = new List<MethodDeclDef>();
        public IEnumerable<MethodDeclDef> DeclaredMethods => declaredMethods;
        public void AddMethodDeclDef(MethodDeclDef mdd)
        {
            declaredMethods.Add(mdd);
        }
    }

}
