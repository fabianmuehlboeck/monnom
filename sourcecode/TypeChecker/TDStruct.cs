using System;
using System.Collections.Generic;
using System.Text;
using Nom.Language;
using System.Linq;

namespace Nom.TypeChecker
{
    public class TDStruct : ITDStruct
    {
        private static UInt64 idcounter = 0;
        public TDStruct()
        {
            StructID = idcounter.ToString();
            idcounter++;
        }


        public int InitializerRegisterCount { get; set; }
        public int EndArgRegisterCount { get; set; }

        public ITypeParametersSpec ClosureTypeParameters { get; set; }

        public IEnumerable<IVariableReference> InitializerArgs { get; private set; }
        public void SetInitializerArgs(IEnumerable<IVariableReference> varrefs)
        {
            InitializerArgs = varrefs;
        }

        public void AddMethod(StructMethodDef smd)
        {
            //TODO: check for collisions
            methods.Add(smd);
        }
        private List<StructMethodDef> methods = new List<StructMethodDef>();
        public IEnumerable<StructMethodDef> Methods => methods.ToList();

        public void AddField(TDStructField sf)
        {
            //TODO: check for collisions
            fields.Add(sf);
        }
        private List<TDStructField> fields = new List<TDStructField>();
        public IEnumerable<TDStructField> Fields => fields.ToList();

        public string StructID { get; }
    }
}
