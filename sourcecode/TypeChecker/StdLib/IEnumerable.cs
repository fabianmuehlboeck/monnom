using System;
using System.Collections.Generic;
using System.Text;
using Nom.Language;

namespace Nom.TypeChecker.StdLib
{
    internal class IEnumerableInterface : AStdInterface
    {
        internal readonly MethodDeclDef GetEnumeratorMethod;
        private IEnumerableInterface() : base("IEnumerable")
        {
            List<TDTypeArgDecl> typeArgs = new List<TDTypeArgDecl>();
            typeArgs.Add(new TDTypeArgDecl("T", 0) { Parent = this });

            TypeParameters = new TypeParametersSpec(typeArgs);

            GetEnumeratorMethod = new MethodDeclDef(
                new Parser.Identifier("GetEnumerator"), 
                new TypeParametersSpec(new List<ITypeParameterSpec>()), 
                new ParametersSpec(new List<ParameterSpec>()), 
                IEnumeratorInterface.Instance.Instantiate(new List<IType>(){new TypeVariable(typeArgs[0])}),
                Visibility.Public, this);
        }

        public static IEnumerableInterface Instance = new IEnumerableInterface();
        public override IEnumerable<IMethodSpec> Methods
        {
            get
            {
                yield return GetEnumeratorMethod;
                yield break;
            }
        }

        public override bool IsShape => false;
        public override ITypeParametersSpec TypeParameters { get; }
    }
}
