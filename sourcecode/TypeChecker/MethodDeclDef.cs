using System;
using System.Collections.Generic;
using System.Text;
using System.Linq;
using Nom.Language;

namespace Nom.TypeChecker
{
    internal class MethodDeclDef : AParameterized, IMethodSpec, IMethodDecl
    {
        public MethodDeclDef(Parser.Identifier name, ITypeParametersSpec typeParameters, IParametersSpec parameters, IType returnType, Visibility visibility, IInterfaceSpec container)
        {
            Identifier = name;
            TypeParameters = typeParameters;
            Parameters = parameters;
            ReturnType = returnType;
            Visibility = visibility;
            Container = container;
            foreach(var tp in typeParameters)
            {
                tp.Parent = this;
            }
        }
        INamespaceSpec IMember.Container => Container;
        public IInterfaceSpec Container { get; }
        public Parser.Identifier Identifier { get; }
        public String Name => Identifier.Name;
        public override ITypeParametersSpec TypeParameters { get; }
        public IParametersSpec Parameters { get; }
        public IType ReturnType { get; }
        public Visibility Visibility { get; }
        public virtual bool IsFinal => false;
        public virtual bool IsVirtual => true;

        protected override IOptional<IParameterizedSpec> ParamParent => Container.InjectOptional();


        public bool Overrides(IMethodSpec other, ITypeEnvironment<IType> substitutions)
        {
            bool ret = Name == other.Name && TypeParameters.Count() == other.TypeParameters.Count() && Parameters.Entries.Count() == other.Parameters.Entries.Count() && Visibility == other.Visibility;
            ret = ret && ReturnType.IsSubtypeOf(((ISubstitutable<IType>)other.ReturnType).Substitute(substitutions), true);
            foreach (var tp in Parameters.Entries.Zip(other.Parameters.Entries, (x, y) => new Tuple<IType, IType>(x.Type, y.Type)))
            {
                ret = ret && ((ISubstitutable<IType>)tp.Item2).Substitute(substitutions).IsSubtypeOf(tp.Item1, true);
            }
            //TODO: type parameter compatibility
            return ret;
        }

        public override string ToString()
        {
            return this.ToSignatureText();
        }
    }
}
