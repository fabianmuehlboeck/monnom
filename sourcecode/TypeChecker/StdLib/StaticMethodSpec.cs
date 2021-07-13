using System;
using System.Collections.Generic;
using System.Text;
using Nom.Language;
using System.Linq;

namespace Nom.TypeChecker.StdLib
{
    internal class StaticMethodSpec : AParameterized, IStaticMethodSpec
    {
        internal StaticMethodSpec(string name, INamespaceSpec container, ITypeParametersSpec typeParameters, IParametersSpec parameters, IType returnType, Visibility visibility = Visibility.Public)
        {
            Name = name;
            Container = container;
            Parameters = parameters;
            ReturnType = returnType;
            TypeParameters = typeParameters;
            Visibility = visibility;
        }

        public string Name { get; }

        public IParametersSpec Parameters { get; }

        public IType ReturnType { get; }

        public override ITypeParametersSpec TypeParameters { get; }

        protected override IOptional<IParameterizedSpec> ParamParent => Container.InjectOptional();

        public INamespaceSpec Container { get; }

        public Visibility Visibility { get; }
    }
}
