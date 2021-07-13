using System;
using System.Collections.Generic;
using System.Text;
using System.Linq;
using Nom.Language;

namespace Nom.TypeChecker
{
    internal class StaticMethodDef : AParameterized, IStaticMethodDef
    {
        public StaticMethodDef(Parser.Identifier name, ITypeParametersSpec typeParameters, IParametersSpec parameters, IType returnType, Visibility visibility, INamespaceSpec container)
        {
            this.Identifier = name;
            TypeParameters = typeParameters;
            Parameters = parameters;
            ReturnType = returnType;
            Visibility = visibility;
            Container = container;
            foreach (var tp in typeParameters)
            {
                tp.Parent = this;
            }
        }
        public Parser.Identifier Identifier { get; }
        public string Name => Identifier.Name;

        public override ITypeParametersSpec TypeParameters { get; }

        public IParametersSpec Parameters { get; }

        public IType ReturnType { get; }

        public Visibility Visibility { get; }
        public IEnumerable<IInstruction> Instructions { get; set; }
        public int RegisterCount { get; set; }
        public INamespaceSpec Container { get; }
        protected override IOptional<IParameterizedSpec> ParamParent => Container.InjectOptional();

    }
}
