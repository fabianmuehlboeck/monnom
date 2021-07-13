using System;
using System.Collections.Generic;
using System.Text;
using System.Linq;

using Nom.Language;

namespace Nom.TypeChecker.StdLib
{
    class StdLibConstructorSpec : AParameterized, IConstructorSpec
    {
        public StdLibConstructorSpec(IClassSpec container, IParametersSpec parameters, Visibility visibility)
        {
            Container = container;
            Parameters = parameters;
            Visibility = visibility;
        }
        public IClassSpec Container { get; }

        public IParametersSpec Parameters { get; }

        public IType ReturnType => new ClassType(Container, new TypeEnvironment<ITypeArgument>(Container.TypeParameters.Entries, Container.TypeParameters.Select(para => new TypeVariable(para))));
        protected override IOptional<IParameterizedSpec> ParamParent => Container.InjectOptional();

        public override ITypeParametersSpec TypeParameters => Container.TypeParameters;

        public Visibility Visibility { get; }

        INamespaceSpec IMember.Container => Container;
    }
}
