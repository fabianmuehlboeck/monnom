using System;
using System.Collections.Generic;
using System.Text;
using Nom.Language;
using System.Linq;
using Nom.Parser;

namespace Nom.TypeChecker
{
    internal class MethodDef : MethodDeclDef, IMethodDef
    {
        public MethodDef(Identifier name, ITypeParametersSpec typeParameters, IParametersSpec parameters, Language.IType returnType, Visibility visibility, IClassSpec container, bool isFinal, bool isVirtual, bool isOverride) : base(name, typeParameters, parameters, returnType, visibility,container)
        {
            IsFinal = isFinal;
            IsVirtual = isVirtual;
            IsOverride = isOverride;
            Container = container;
        }

        public new IClassSpec Container { get; }

        public override bool IsFinal { get; }
        public override bool IsVirtual { get; }
        public bool IsOverride { get; }

        public int RegisterCount { get; set; }
        public IEnumerable<IInstruction> Instructions { get; set; }
    }
}
