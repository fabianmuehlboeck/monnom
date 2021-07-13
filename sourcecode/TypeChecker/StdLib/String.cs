using System;
using System.Collections.Generic;
using System.Text;
using Nom.Language;

namespace Nom.TypeChecker.StdLib
{
    internal class String : AStdClass
    {
        public static String Instance = new String();
        private String() : base("String", Object.Instance)
        {
        }

        private List<IMethodSpec> methods = null;
        public override IEnumerable<IMethodSpec> Methods
        {
            get
            {
                if(methods==null)
                {
                    methods = new List<IMethodSpec>();
                    methods.Add(new MethodSpec("Print", this, new TypeParametersSpec(new List<ITypeParameterSpec>()), new ParametersSpec(new List<IParameterSpec>()), StdLib.VoidType));
                    methods.Add(new MethodSpec("Concat", this, new TypeParametersSpec(new List<ITypeParameterSpec>()), new ParametersSpec(new List<IParameterSpec> { new ParameterSpec("other", ClassType) }), ClassType));
                }
                return methods;
            }
        }
    }
}
