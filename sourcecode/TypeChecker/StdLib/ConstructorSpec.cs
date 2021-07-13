using System;
using System.Collections.Generic;
using System.Text;
using System.Linq;
using Nom.Language;

namespace Nom.TypeChecker.StdLib
{
    internal class ConstructorSpec : AParameterized, IConstructorSpec
    {
        public ConstructorSpec(IClassSpec cls,  ITypeParametersSpec typeParams, IParametersSpec fparams)
        {
            Container = cls;
            Parameters = fparams;
            TypeParameters = typeParams;
        }

        public IClassSpec Container { get; }

        public IParametersSpec Parameters { get; }

        public IType ReturnType => new ClassType(Container, new TypeEnvironment<ITypeArgument>(TypeParameters, TypeParameters.Select(tp => new TypeVariable(tp))));

        protected override IOptional<IParameterizedSpec> ParamParent => Container.InjectOptional();

        public override ITypeParametersSpec TypeParameters { get; }


        public Visibility Visibility => Visibility.Public;

        INamespaceSpec IMember.Container => Container;
    }
}
