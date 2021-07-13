using System;
using System.Collections.Generic;
using System.Text;
using Nom.Language;
using System.Linq;

namespace Nom.TypeChecker.StdLib
{
    internal class Math : AStdClass
    {
        public static readonly Math Instance = new Math();
        private Math() : base("Math", Object.Instance)
        {
            SinMethod = new StaticMethodSpec("Sin", this, new TypeParametersSpec(new List<ITypeParameterSpec>()), new ParametersSpec(new List<IParameterSpec>() { new ParameterSpec("x", StdLib.FloatType) }), StdLib.FloatType, Visibility.Public);
            CosMethod = new StaticMethodSpec("Cos", this, new TypeParametersSpec(new List<ITypeParameterSpec>()), new ParametersSpec(new List<IParameterSpec>() { new ParameterSpec("x", StdLib.FloatType) }), StdLib.FloatType, Visibility.Public);
            TanMethod = new StaticMethodSpec("Tan", this, new TypeParametersSpec(new List<ITypeParameterSpec>()), new ParametersSpec(new List<IParameterSpec>() { new ParameterSpec("x", StdLib.FloatType) }), StdLib.FloatType, Visibility.Public);
            FMaxMethod = new StaticMethodSpec("FMax", this, new TypeParametersSpec(new List<ITypeParameterSpec>()), new ParametersSpec(new List<IParameterSpec>() { new ParameterSpec("x", StdLib.FloatType), new ParameterSpec("u", StdLib.FloatType) }), StdLib.FloatType, Visibility.Public);
            FMinMethod = new StaticMethodSpec("FMin", this, new TypeParametersSpec(new List<ITypeParameterSpec>()), new ParametersSpec(new List<IParameterSpec>() { new ParameterSpec("x", StdLib.FloatType), new ParameterSpec("u", StdLib.FloatType) }), StdLib.FloatType, Visibility.Public);
            SqrtMethod = new StaticMethodSpec("Sqrt", this, new TypeParametersSpec(new List<ITypeParameterSpec>()), new ParametersSpec(new List<IParameterSpec>() { new ParameterSpec("x", StdLib.FloatType) }), StdLib.FloatType, Visibility.Public);
        }

        StaticMethodSpec SinMethod { get; }
        StaticMethodSpec CosMethod { get; }
        StaticMethodSpec TanMethod { get; }
        StaticMethodSpec FMaxMethod { get; }
        StaticMethodSpec FMinMethod { get; }
        StaticMethodSpec SqrtMethod { get; }

        public override IEnumerable<IStaticMethodSpec> StaticMethods
        {
            get
            {
                yield return SinMethod;
                yield return CosMethod;
                yield return TanMethod;
                yield return FMaxMethod;
                yield return FMinMethod;
                yield return SqrtMethod;
                yield break;
            }
        }
    }
}
