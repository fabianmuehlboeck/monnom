using System;
using System.Collections.Generic;
using System.Text;
using Nom.Language;

namespace Nom.TypeChecker.StdLib
{
    internal class Timer : AStdClass
    {
        public static Timer Instance = new Timer();
        private Timer() : base("Timer", Object.Instance)
        {
            List<IMethodSpec> methods = new List<IMethodSpec>();
            methods.Add(new MethodSpec("PrintDifference", this, new TypeParametersSpec(new List<ITypeParameterSpec>()), new ParametersSpec(new List<IParameterSpec>()), StdLib.VoidType));
            Methods = methods;
            List<IConstructorSpec> constructors = new List<IConstructorSpec>();
            constructors.Add(new ConstructorSpec(this, new TypeParametersSpec(new List<ITypeParameterSpec>()), new ParametersSpec(new List<IParameterSpec>())));
            Constructors = constructors;
        }

        public override IEnumerable<IConstructorSpec> Constructors { get; }
        public override IEnumerable<IMethodSpec> Methods { get; }
    }
}
