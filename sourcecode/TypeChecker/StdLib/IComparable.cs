using System;
using System.Collections.Generic;
using System.Text;
using Nom.Language;

namespace Nom.TypeChecker.StdLib
{
    class IComparable : AStdInterface
    {
        private MethodDeclDef CompareMethod;
        public static IComparable Instance { get; } = new IComparable();
        private IComparable() : base("IComparable")
        {
            var targ = new TDTypeArgDecl("Arg", 0) { Parent = this };
            TypeParameters = new TypeParametersSpec(new List<TDTypeArgDecl>(){ targ });
            CompareMethod = new MethodDeclDef(new Parser.Identifier("Compare"), new TypeParametersSpec(new List<ITypeParameterSpec>()), new ParametersSpec(new List<ParameterSpec>() { new ParameterSpec("other", new TypeVariable(targ)) }), Int.Instance.ClassType, Visibility.Public, this);
        }
        
        public override IEnumerable<IMethodSpec> Methods
        {
            get
            {
                yield return CompareMethod;
                yield break;
            }
        }

        public override ITypeParametersSpec TypeParameters { get; }
        public override bool IsShape => true;
    }
}
