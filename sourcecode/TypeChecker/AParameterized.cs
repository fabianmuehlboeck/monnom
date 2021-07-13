using System;
using System.Collections.Generic;
using System.Text;
using System.Linq;
using Nom.Language;
using Nom.Language.SpecExtensions;

namespace Nom.TypeChecker
{
    public abstract class AParameterized : IParameterizedSpec
    { 
        protected abstract IOptional<IParameterizedSpec> ParamParent { get; }
        public IOptional<IParameterizedSpec> ParameterizedParent => ParamParent;

        public abstract ITypeParametersSpec TypeParameters { get;  }

        public virtual int OverallTypeParameterCount => ParamParent.Extract(p => p.OverallTypeParameterCount) + TypeParameters.Count();

        public ITypeParametersSpec AllTypeParameters => new TypeParametersSpec(ParamParent.Extract<IEnumerable<ITypeParameterSpec>>(pp => pp.AllTypeParameters, new List<ITypeParameterSpec>()).Concat(TypeParameters));

        public virtual IParameterizedSpecRef<IParameterizedSpec> GetAsRef()
        {
            return new ParameterizedSpecRef<IParameterizedSpec>(this, this.AllTypeParameters.ToTypeEnvironment());
        }
    }
}
