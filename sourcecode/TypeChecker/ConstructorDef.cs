using System;
using System.Collections.Generic;
using System.Text;
using System.Linq;
using Nom.Language;

namespace Nom.TypeChecker
{
    internal class ConstructorDef : AParameterized, IConstructorDef
    {
        public ConstructorDef(TDClass cls, ITypeParametersSpec typeParameters, IParametersSpec parameters, IType returnType, Visibility visibility)
        {
            TypeParameters = typeParameters;
            Parameters = parameters;
            ReturnType = returnType;
            Visibility = visibility;
            Container = cls;
            foreach (var tp in typeParameters)
            {
                tp.Parent = this;
            }
        }
        INamespaceSpec IMember.Container => Container;
        public override ITypeParametersSpec TypeParameters { get; }

        public IParametersSpec Parameters { get; }

        public IType ReturnType { get; }

        public Visibility Visibility { get; }
        public IEnumerable<IInstruction> PreInstructions { get; set; }
        public IEnumerable<IInstruction> PostInstructions { get; set; }
        public IEnumerable<IRegister> SuperConstructorArgs { get; set; }

        public IClassSpec Container { get; }

        protected override IOptional<IParameterizedSpec> ParamParent => Container.InjectOptional();
        public int RegisterCount { get; set; }


    }
}
