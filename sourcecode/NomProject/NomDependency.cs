using System;
using System.Collections.Generic;
using System.Text;
using Nom;

namespace Nom.Project
{
    public class NomDependency
    {
        public NomDependency(String qname, Version version)
        {
            QName = qname;
            Version = version;
        }
        public string QName { get; }
        public Version Version { get; }
    }
}
