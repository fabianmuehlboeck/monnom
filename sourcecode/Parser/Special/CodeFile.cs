using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;

namespace Nom.Parser
{
    public class CodeFile : Namespace
    {
        public IEnumerable<RefQName> Usings
        {
            get;
            private set;
        }

        public String FileName
        {
            get;
        }
        
        public CodeFile(string fileName, IEnumerable<RefQName> usings, IEnumerable<InterfaceDef> interfaces, IEnumerable<ClassDef> classes, IEnumerable<Namespace> namespaces) : base(new DeclQName(), interfaces, classes, namespaces, new GenSourceSpan())
        {
            this.Usings = usings;
            this.Interfaces = interfaces;
            this.Classes = classes;
            this.FileName = fileName;
        }

    }
}
