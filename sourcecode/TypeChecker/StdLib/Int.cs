using System;
using System.Collections.Generic;
using System.Text;
using Nom.Language;

namespace Nom.TypeChecker.StdLib
{
    internal class Int : AStdClass
    {
        public static Int Instance = new Int();
        private Int() : base("Int", Object.Instance)
        {
        }
        private List<IMethodSpec> methods=null;
        public override IEnumerable<IMethodSpec> Methods
        {
            get
            {
                if(methods==null)
                {
                    methods = new List<IMethodSpec>();
                    methods.Add(new MethodSpec("ToString", this, new TypeParametersSpec(new List<ITypeParameterSpec>()), new ParametersSpec(new List<IParameterSpec>()), String.Instance.ClassType));
                    methods.Add(new MethodSpec("Compare", this, new TypeParametersSpec(new List<ITypeParameterSpec>()), new ParametersSpec(new List<IParameterSpec>() { new ParameterSpec("other", this.ClassType) }), Bool.Instance.ClassType));
                }
                return methods;
            }
        }
        public override IEnumerable<IParamRef<IInterfaceSpec, IType>> Implements
        {
            get
            {
                yield return new InterfaceRef<IType>(IComparable.Instance, new TypeEnvironment<IType>(IComparable.Instance.TypeParameters, new List<IType>() { this.ClassType }));
            }
        }
    }
}
