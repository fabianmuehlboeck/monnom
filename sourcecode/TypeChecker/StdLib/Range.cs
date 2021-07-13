using Nom.Language;
using System;
using System.Collections.Generic;
using System.Text;

namespace Nom.TypeChecker.StdLib
{
    internal class Range : AStdClass
    {
        public static Range Instance = new Range();
        private Range() : base("Range", Object.Instance)
        {
            List<TDTypeArgDecl> typeArgs = new List<TDTypeArgDecl>();

            TypeParameters = new TypeParametersSpec(typeArgs);

            GetEnumeratorMethod = new MethodSpec("GetEnumerator", this,
                new TypeParametersSpec(new List<ITypeParameterSpec>()),
                new ParametersSpec(new List<IParameterSpec>()),
                IEnumeratorInterface.Instance.Instantiate(new List<IType>() { StdLib.IntType }),
                Visibility.Public,
                true,
                false);

            FullConstructor = new StdLibConstructorSpec(this, new ParametersSpec(new List<IParameterSpec>()
            {
                new ParameterSpec("start", StdLib.IntType),
                new ParameterSpec("end", StdLib.IntType),
                new ParameterSpec("step", StdLib.IntType)
            }), Visibility.Public);
        }

        internal readonly MethodSpec GetEnumeratorMethod;
        internal readonly IConstructorSpec FullConstructor;

        public override IEnumerable<IConstructorSpec> Constructors
        {
            get
            {
                yield return FullConstructor;
                yield break;
            }
        }

        public override IEnumerable<IMethodSpec> Methods
        {
            get
            {
                yield return GetEnumeratorMethod;
                yield break;
            }
        }

        public override ITypeParametersSpec TypeParameters { get; }

        public override IEnumerable<IParamRef<IInterfaceSpec, IType>> Implements
        {
            get
            {
                yield return new InterfaceRef<IType>(IEnumerableInterface.Instance, new TypeEnvironment<IType>(IEnumerableInterface.Instance.TypeParameters, new List<IType>() { StdLib.IntType }));
            }
        }
    }
}
