using Nom.Language;
using System;
using System.Collections.Generic;
using System.Text;
using System.Linq;

namespace Nom.TypeChecker.StdLib
{
    internal class ArrayList : AStdClass
    {
        public static ArrayList Instance = new ArrayList();
        private ArrayList() : base("ArrayList", Object.Instance)
        {
            List<TDTypeArgDecl> typeArgs = new List<TDTypeArgDecl>();
            typeArgs.Add(new TDTypeArgDecl("T", 0) { Parent = this });

            TypeParameters = new TypeParametersSpec(typeArgs);

            AddMethod = new MethodSpec("Add", this,
                new TypeParametersSpec(new List<ITypeParameterSpec>()),
                new ParametersSpec(new List<IParameterSpec>() { new ParameterSpec("elem", new TypeVariable(typeArgs[0])) }),
                StdLib.VoidType,
                Visibility.Public,
                true,
                false);

            ContainsMethod = new MethodSpec("Contains", this,
                new TypeParametersSpec(new List<ITypeParameterSpec>()),
                new ParametersSpec(new List<IParameterSpec>() { new ParameterSpec("elem", new TypeVariable(typeArgs[0])) }),
                StdLib.BoolType,
                Visibility.Public,
                true,
                false);

            GetMethod = new MethodSpec("Get", this,
                new TypeParametersSpec(new List<ITypeParameterSpec>()),
                new ParametersSpec(new List<IParameterSpec>() { new ParameterSpec("index", StdLib.IntType) }),
                 new TypeVariable(typeArgs[0]),
                Visibility.Public,
                true,
                false);

            SetMethod = new MethodSpec("Set", this,
                new TypeParametersSpec(new List<ITypeParameterSpec>()),
                new ParametersSpec(new List<IParameterSpec>() { new ParameterSpec("index",StdLib.IntType), new ParameterSpec("elem", new TypeVariable(typeArgs[0])) }),
                StdLib.VoidType,
                Visibility.Public,
                true,
                false);

            RemoveMethod = new MethodSpec("Remove", this,
                new TypeParametersSpec(new List<ITypeParameterSpec>()),
                new ParametersSpec(new List<IParameterSpec>() { new ParameterSpec("index", StdLib.IntType) }),
                StdLib.VoidType,
                Visibility.Public,
                true,
                false);

            LengthMethod = new MethodSpec("Length", this,
                new TypeParametersSpec(new List<ITypeParameterSpec>()),
                new ParametersSpec(new List<IParameterSpec>() { }),
                StdLib.IntType,
                Visibility.Public,
                true,
                false);

            InsertMethod = new MethodSpec("Insert", this,
                new TypeParametersSpec(new List<ITypeParameterSpec>()),
                new ParametersSpec(new List<IParameterSpec>() { new ParameterSpec("elem", new TypeVariable(typeArgs[0])), new ParameterSpec("index", StdLib.IntType) }),
                StdLib.VoidType,
                Visibility.Public,
                true,
                false);

            GetEnumeratorMethod = new MethodSpec("GetEnumerator", this,
                new TypeParametersSpec(new List<ITypeParameterSpec>()),
                new ParametersSpec(new List<IParameterSpec>()),
                IEnumeratorInterface.Instance.Instantiate(new List<IType>() { new TypeVariable(typeArgs[0]) }),
                Visibility.Public,
                true,
                false);

            EmptyConstructor = new StdLibConstructorSpec(this, new ParametersSpec(new List<IParameterSpec>()), Visibility.Public);
        }

        internal readonly MethodSpec AddMethod;
        internal readonly MethodSpec GetMethod;
        internal readonly MethodSpec SetMethod;
        internal readonly MethodSpec ContainsMethod;
        internal readonly MethodSpec RemoveMethod;
        internal readonly MethodSpec InsertMethod;
        internal readonly MethodSpec LengthMethod;
        internal readonly MethodSpec GetEnumeratorMethod;
        internal readonly IConstructorSpec EmptyConstructor;


        public override IEnumerable<IConstructorSpec> Constructors
        {
            get
            {
                yield return EmptyConstructor;
                yield break;
            }
        }

        public override IEnumerable<IMethodSpec> Methods
        {
            get
            {
                yield return GetEnumeratorMethod;
                yield return AddMethod;
                yield return GetMethod;
                yield return SetMethod;
                yield return ContainsMethod;
                yield return LengthMethod;
                yield return RemoveMethod;
                yield return InsertMethod;
                yield break;
            }
        }

        public override ITypeParametersSpec TypeParameters { get; }

        public override IEnumerable<IParamRef<IInterfaceSpec, IType>> Implements
        {
            get
            {
                yield return new InterfaceRef<IType>(IEnumerableInterface.Instance, new TypeEnvironment<IType>(IEnumerableInterface.Instance.TypeParameters, new List<IType>() { new TypeVariable(this.TypeParameters.First()) }));
            }
        }
    }
}
