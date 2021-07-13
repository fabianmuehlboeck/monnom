using System;
using System.Collections.Generic;
using System.Text;
using Nom.Language;
using System.Linq;

namespace Nom.TypeChecker
{
    public class StructMethodDef
    {
        public Parser.Identifier Identifier { get; }
        public String Name => Identifier.Name;

        public ITypeParametersSpec TypeParameters { get; }
        public IParametersSpec Parameters { get; }
        public IType ReturnType { get; }

        public ITDStruct Container { get; }

        public StructMethodDef(Parser.Identifier name, ITypeParametersSpec typeParameters, IParametersSpec parameters, Language.IType returnType, ITDStruct container)
        {
            Identifier = name;
            TypeParameters = typeParameters;
            Parameters = parameters;
            ReturnType = returnType;
            Container = container;
        }
        public int RegisterCount { get; set; }
        public IEnumerable<IInstruction> Instructions { get; set; }
    }
}
