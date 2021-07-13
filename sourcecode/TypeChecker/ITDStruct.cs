using System;
using System.Collections.Generic;
using System.Text;
using Nom.Language;

namespace Nom.TypeChecker
{
    public interface ITDStruct
    {
        ITypeParametersSpec ClosureTypeParameters { get; set; }
        IEnumerable<StructMethodDef> Methods { get; }
        IEnumerable<TDStructField> Fields { get; }
        IEnumerable<IVariableReference> InitializerArgs
        {
            get;
        }
        int InitializerRegisterCount { get; }
        int EndArgRegisterCount { get; }
        string StructID { get; }
    }
}
