using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.Xml;

namespace Nom.Project
{
    internal static class ExtensionMethods
    {
        public static int AsInt(this string value, int defaultValue)
        {
            int ret = defaultValue;
            if (value.Length > 0)
            {
                if (!int.TryParse(value, out ret))
                {
                    throw new InvalidDataException();
                }
            }
            return ret;
        }
        public static short AsShort(this string value, short defaultValue)
        {
            short ret = defaultValue;
            if (value.Length > 0)
            {
                if (!short.TryParse(value, out ret))
                {
                    throw new InvalidDataException();
                }
            }
            return ret;
        }
    }
    public class NomProject
    {
        public string Name { get; set; }
        public string MainClassName { get; set; }
        public List<String> Files { get; } = new List<string>();
        public List<String> LibraryFiles { get; } = new List<string>();
        public List<NomDependency> Dependencies { get; } = new List<NomDependency>();
        public List<NativeLink> NativeLinks { get; } = new List<NativeLink>();
        public Version Version { get; set; }

        public NomProject(NomProject other)
        {
            Name = other.Name;
            MainClassName = other.MainClassName;
            Version = new Version(other.Version.Major, other.Version.Minor, other.Version.Revision, other.Version.Build);
            Files.AddRange(other.Files);
            LibraryFiles.AddRange(other.Files);
            Dependencies.AddRange(other.Dependencies);
            NativeLinks.AddRange(other.NativeLinks);
        }

        public NomProject(string name)
        {
            Name = name;
            MainClassName = "";
            Version = new Version();
        }
        public NomProject(FileInfo fi)
        {
            XmlDocument doc = new XmlDocument();
            using (var stream = fi.OpenRead())
            {
                using (var reader = XmlReader.Create(stream))
                {
                    doc.Load(reader);
                    reader.Close();
                }
                stream.Close();
            }
            XmlElement nomproj = doc.DocumentElement;
            Name = nomproj.GetAttribute("name");
            MainClassName = nomproj.GetAttribute("mainclass");

            Version = new Version(nomproj.GetAttribute("major").AsShort(1),
                nomproj.GetAttribute("minor").AsShort(0),
                nomproj.GetAttribute("revision").AsShort(0),
                nomproj.GetAttribute("build").AsShort(0));

            var deps = nomproj.SelectNodes("./dependencies/dependency");

            foreach (var dep in deps)
            {
                var depelem = (XmlElement)dep;
                var qname = depelem.GetAttribute("qname");
                var version = new Version(depelem.GetAttribute("version"));
                Dependencies.Add(new NomDependency(qname, version));
            }

            var files = nomproj.SelectNodes("./files/file");
            foreach (var file in files)
            {
                var fileelem = (XmlElement)file;
                var filename = fileelem.GetAttribute("name");
                Files.Add(filename);
            }

            var libfiles = nomproj.SelectNodes("./libraryfiles/file");
            foreach (var file in libfiles)
            {
                var fileelem = (XmlElement)file;
                var filename = fileelem.GetAttribute("name");
                LibraryFiles.Add(filename);
            }

            var nativelinks = nomproj.SelectNodes("./native/library");
            foreach (var natlink in nativelinks)
            {
                var natlinkelem = (XmlElement)natlink;
                var natlinkname = natlinkelem.GetAttribute("name");
                var binnodes = natlinkelem.SelectNodes("./binary");
                List<NativeLink.Binary> binaries=new List<NativeLink.Binary>();
                foreach (var binnode in binnodes)
                {
                    var binelem=(XmlElement)binnode;
                    binaries.Add(new NativeLink.Binary(binelem.GetAttribute("type"), binelem.GetAttribute("path"), binelem.GetAttribute("platform"), binelem.GetAttribute("os"), binelem.GetAttribute("version")));
                }
                NativeLinks.Add(new NativeLink(natlinkname, binaries));
            }
        }

        public void Write(FileInfo fi)
        {
            XmlDocument doc = new XmlDocument();
            XmlElement nomproj = doc.CreateElement("project");
            doc.AppendChild(nomproj);
            nomproj.SetAttribute("name", Name);
            if (MainClassName.Length > 0)
            {
                nomproj.SetAttribute("mainclass", MainClassName);
                nomproj.SetAttribute("major", Version.Major.ToString());
                nomproj.SetAttribute("minor", Version.Minor.ToString());
                nomproj.SetAttribute("revision", Version.Revision.ToString());
                nomproj.SetAttribute("build", Version.Build.ToString());
            }

            if (Dependencies.Count > 0)
            {
                XmlElement deps = doc.CreateElement("dependencies");
                foreach (NomDependency dep in Dependencies)
                {
                    XmlElement depelem = doc.CreateElement("dependency");
                    depelem.SetAttribute("qname", dep.QName);
                    depelem.SetAttribute("major", dep.Version.Major.ToString());
                    depelem.SetAttribute("minor", dep.Version.Minor.ToString());
                    depelem.SetAttribute("revision", dep.Version.Revision.ToString());
                    depelem.SetAttribute("build", dep.Version.Build.ToString());
                    deps.AppendChild(depelem);
                }
                nomproj.AppendChild(deps);
            }
            if (Files.Count > 0)
            {
                XmlElement files = doc.CreateElement("files");
                foreach (var file in Files)
                {
                    XmlElement fileelem = doc.CreateElement("file");
                    fileelem.SetAttribute("name", file);
                    files.AppendChild(fileelem);
                }
                nomproj.AppendChild(files);
            }
            if (LibraryFiles.Count > 0)
            {
                XmlElement files = doc.CreateElement("libraryfiles");
                foreach (var file in LibraryFiles)
                {
                    XmlElement fileelem = doc.CreateElement("file");
                    fileelem.SetAttribute("name", file);
                    files.AppendChild(fileelem);
                }
                nomproj.AppendChild(files);
            }

            if (NativeLinks.Count > 0) {
                XmlElement nativeelem = doc.CreateElement("native");
                foreach (NativeLink nl in NativeLinks)
                {
                    XmlElement libelem = doc.CreateElement("library");
                    libelem.SetAttribute("name", nl.Name);
                    foreach(var binary in nl.Binaries)
                    {
                        XmlElement binelem = doc.CreateElement("binary");
                        binelem.SetAttribute("type", binary.Type);
                        binelem.SetAttribute("path", binary.Path);
                        binelem.SetAttribute("platform", binary.Platform);
                        binelem.SetAttribute("os", binary.OS);
                        binelem.SetAttribute("version", binary.Version);
                        libelem.AppendChild(binelem);
                    }
                    nativeelem.AppendChild(libelem);
                }
                nomproj.AppendChild(nativeelem);
            }

            if (fi.Exists)
            {
                fi.Delete();
            }
            using(var writer=XmlWriter.Create(fi.OpenWrite()))
            {
                doc.WriteTo(writer);
                writer.Close();
            }
        }
    }
}
