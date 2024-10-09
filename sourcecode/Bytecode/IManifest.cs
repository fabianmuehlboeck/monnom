using System;
using System.Collections.Generic;
using System.Text;
using System.IO;

namespace Nom.Bytecode
{
    public interface IManifest
    {
        public class ClassInfo
        {
            public String Name;
            public String FileName;
        }
        public class InterfaceInfo
        {
            public String Name;
            public String FileName;
        }
        public class LibraryDependency
        {
            public String Name;
            public Version Version;
        }
        public class BinaryInfo
        {
            public String Type;
            public String Path;
            public String Platform;
            public String OS;
            public String Version;
        }
        public class NativeLink
        {
            public String Name;
            public List<BinaryInfo> Binaries = new List<BinaryInfo>();
        }
        IOptional<String> MainClass { get; }
        String ProgramName { get; }
        IEnumerable<LibraryDependency> Dependencies { get; }
        IEnumerable<ClassInfo> Classes { get; }
        IEnumerable<InterfaceInfo> Interfaces { get; }
        IEnumerable<NativeLink> NativeLinks { get; }
        Version Version { get; }
        void Emit(Func<string, Stream> opener);
    }
}
