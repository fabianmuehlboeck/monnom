using System;
using System.Collections.Generic;
using System.Text;
using Nom.Language;

namespace Nom.TypeChecker.StdLib
{
    internal class IEnumeratorInterface : AStdInterface
    {
        internal readonly MethodDeclDef MoveNextMethod;
        internal readonly MethodDeclDef CurrentMethod;
        private IEnumeratorInterface() : base("IEnumerator")
        {
            List<TDTypeArgDecl> typeArgs = new List<TDTypeArgDecl>();
            typeArgs.Add(new TDTypeArgDecl("T", 0) { Parent = this });

            TypeParameters = new TypeParametersSpec(typeArgs);

            MoveNextMethod = new MethodDeclDef(new Parser.Identifier("MoveNext"), new TypeParametersSpec(new List<ITypeParameterSpec>()), new ParametersSpec(new List<ParameterSpec>()), Bool.Instance.ClassType, Visibility.Public, this);
            CurrentMethod = new MethodDeclDef(new Parser.Identifier("Current"), new TypeParametersSpec(new List<ITypeParameterSpec>()), new ParametersSpec(new List<ParameterSpec>()), new TypeVariable(typeArgs[0]), Visibility.Public, this);
        }

        public static IEnumeratorInterface Instance = new IEnumeratorInterface();
        public override IEnumerable<IMethodSpec> Methods
        {
            get
            {
                yield return MoveNextMethod;
                yield return CurrentMethod;
                yield break;
            }
        }

        public override bool IsShape => false;
        public override ITypeParametersSpec TypeParameters { get; }
    }
}
