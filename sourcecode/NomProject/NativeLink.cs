using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Nom.Project
{
    public class NativeLink
    {
        public class Binary
        {
            public Binary(string type, string path, string platform, string os, string version)
            { 
                Type = type;
                Path = path;
                Platform = platform;
                OS = os;
                Version = version;
            }
            public string Type { get; set; }
            public string Path { get; set; }
            public string Platform { get; set; }
            public string OS { get; set; }
            public string Version { get; set; }
        }

        public string Name { get; set; }
        public List<Binary> Binaries { get; } = new List<Binary>();

        public NativeLink(string name, IEnumerable<Binary> binaries=null) {
            Name = name;
            if (binaries != null)
            {
                Binaries.AddRange(binaries);
            }
        }
    }
}
