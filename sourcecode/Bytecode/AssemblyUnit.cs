using System;
using System.Collections.Generic;
using System.Linq;
using Nom.Language;
using System.IO;
using Nom.Project;
using System.IO.Compression;

namespace Nom.Bytecode
{
    public class AssemblyUnit : ILibrary
    {

        private NomProject project;

        public AssemblyUnit(NomProject project)
        {
            this.project = project;
        }

        public string Name => project.Name;

        public Version Version => project.Version;

        public INamespaceSpec GlobalNamespace { get; } = new NamespaceRep("", Optional<INamespaceSpec>.Empty);

        private List<BytecodeUnit> units = new List<BytecodeUnit>();
        public void AddUnit(BytecodeUnit bcu)
        {
            units.Add(bcu);
        }

        public IManifest Emit(Func<string, Stream> opener, bool ignoreManifestName)
        {
            List<IManifest.ClassInfo> classInfos = new List<IManifest.ClassInfo>();
            List<IManifest.InterfaceInfo> interfaceInfos = new List<IManifest.InterfaceInfo>();
            Func<string, Stream> ILopener = s => opener(s + ".mnil");
            Func<string, Stream> manifestOpener = s => opener(s + ".manifest");
            if(ignoreManifestName)
            {
                manifestOpener = s => opener("default.manifest");
            }
            foreach (BytecodeUnit bcu in units)
            {
                bcu.Emit(ILopener);
                foreach (var cls in bcu.Classes)
                {
                    classInfos.Add(new IManifest.ClassInfo() { Name = cls.FullQualifiedName, FileName = cls.FullQualifiedName + ".mnil" });
                }
                foreach (var iface in bcu.Interfaces)
                {
                    interfaceInfos.Add(new IManifest.InterfaceInfo() { Name = iface.FullQualifiedName, FileName = iface.FullQualifiedName + ".mnil" });
                }
            }
            Manifest manifest = new Manifest(project.MainClassName.Length == 0 ? Optional<String>.Empty : project.MainClassName.InjectOptional(), Name, Version,
                project.Dependencies.Select(nd => new IManifest.LibraryDependency() { Name = nd.QName, Version = nd.Version }),
                classInfos,
                interfaceInfos,
                project.NativeLinks.Select(nl=> new IManifest.NativeLink() { Name=nl.Name, Binaries=nl.Binaries.Select(b=>new IManifest.BinaryInfo() { Type=b.Type, Path=b.Path, Platform=b.Platform, OS=b.OS, Version=b.Version }).ToList()}));
            manifest.Emit(manifestOpener);
            return manifest;
        }

        public IManifest EmitToPath(DirectoryInfo path)
        {
            Func<string, Stream> opener = s => new FileInfo(path + "/" + s).Open(FileMode.Create,FileAccess.Write,FileShare.Read);
            return Emit(opener, false);
        }

        public IManifest EmitArchive(FileInfo fi)
        {
            if (fi.Exists)
            {
                fi.Delete();
            }
            using (var zip = ZipFile.Open(fi.FullName, ZipArchiveMode.Create))
            {
                Func<string, Stream> opener = s => zip.CreateEntry(s).Open();
                return Emit(opener, true);
            }
        }

        public IEnumerable<IParamRef<INamespaceSpec, P>> FindVarargsChildren<P>(IArgIdentifier<string, P> name) where P : ITypeArgument, ISubstitutable<P>
        {
            yield break;
        }
    }
}
