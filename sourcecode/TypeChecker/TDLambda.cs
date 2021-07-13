using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Nom.Language;

namespace Nom.TypeChecker
{
    public class TDLambda : ITDLambda
    {
        private static UInt64 idcounter = 0;
        public TDLambda(ITypeParametersSpec typeParameters, IParametersSpec parameters, IType returnType)
        {
            TypeParameters = typeParameters;
            Parameters = parameters;
            ReturnType = returnType;
            LambdaID = idcounter.ToString();
            idcounter++;
        }

        public ITypeParametersSpec ClosureTypeParameters { get; set; }
        public IParametersSpec ClosureParameters { get; set; }

        public ITypeParametersSpec TypeParameters { get; }
        public IParametersSpec Parameters { get; }
        public IType ReturnType { get; set; }
        public int RegisterCount { get; set; }
        public IEnumerable<IInstruction> Instructions { get; set; }

        private List<TDLambdaField> fields = new List<TDLambdaField>();
        public IEnumerable<TDLambdaField> Fields => fields.ToList();

        public string LambdaID { get; }

        public void AddFieldDef(TDLambdaField field)
        {
            fields.Add(field);
        }
    }
}
