﻿using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using Nom.Language;
using System.Linq;
using System.Xml.Linq;
using System.Xml;
using Nom.Project;
using static Nom.Bytecode.IManifest;

namespace Nom.Bytecode
{
    internal static class XMLExtensionMethods
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
    public class Manifest : IManifest
    {
        public Manifest(IOptional<String> mainClass, string programName, Version version, IEnumerable<IManifest.LibraryDependency> dependencies, IEnumerable<IManifest.ClassInfo> clses, IEnumerable<IManifest.InterfaceInfo> ifaces, IEnumerable<IManifest.NativeLink> nativeLinks)
        {
            MainClass = mainClass;
            ProgramName = programName;
            Version = version;
            Dependencies = dependencies;
            Classes = clses;
            Interfaces = ifaces;
            NativeLinks = nativeLinks;
        }

        public Manifest(FileInfo file)
        {
            using (var reader = XmlReader.Create(file.OpenRead()))
            {
                XmlDocument doc = new XmlDocument();
                doc.Load(reader);
                reader.Close();
                var nomlib = doc.DocumentElement;
                ProgramName = nomlib.GetAttribute("name");
                Version = new Version(nomlib.GetAttribute("major").AsShort(1),
                nomlib.GetAttribute("minor").AsShort(0),
                nomlib.GetAttribute("revision").AsShort(0),
                nomlib.GetAttribute("build").AsShort(0));
                var manifestVersion = nomlib.GetAttribute("fversion");
                if(manifestVersion.Length>0)
                {
                    int versionno;
                    if(!int.TryParse(manifestVersion, out versionno))
                    {
                        throw new NomBytecodeException("Invalid Manifest Version");
                    }
                    if(versionno>1)
                    {
                        throw new NomBytecodeException("Manifest was created with newer version of MonNom compiler - update required!");
                    }
                }
                var deps = nomlib.SelectNodes("./dependencies/dependency");
                var clses = nomlib.SelectNodes("./classes/nomclass");
                var ifaces = nomlib.SelectNodes("./interfaces/nominterface");
                var maincls = nomlib.SelectNodes("./mainclass");
                if(maincls.Count>0)
                {
                    if(maincls.Count>1)
                    {
                        Nom.CompilerOutput.Warn("Multiple main classes defined!");
                    }
                    MainClass = ((XmlElement)maincls[0]).GetAttribute("name").InjectOptional();
                }
                else
                {
                    MainClass = Optional<String>.Empty;
                }
                var classes = new List<IManifest.ClassInfo>();
                foreach (var cls in clses)
                {
                    var clselem = (XmlElement)cls;
                    var qname = clselem.GetAttribute("qname");
                    var filename = clselem.GetAttribute("file");
                    classes.Add(new IManifest.ClassInfo(){ Name= qname, FileName= filename });
                }
                Classes = classes;
                var interfaces = new List<IManifest.InterfaceInfo>();
                foreach (var iface in ifaces)
                {
                    var ifaceelem = (XmlElement)iface;
                    var qname = ifaceelem.GetAttribute("qname");
                    var filename = ifaceelem.GetAttribute("file");
                    interfaces.Add(new IManifest.InterfaceInfo() { Name = qname, FileName = filename });
                }
                Interfaces = interfaces;
                var natlinks = nomlib.SelectNodes("./native/library");
                var nativelinks = new List<IManifest.NativeLink>();
                foreach (var natlink in natlinks)
                {
                    var natlinkelem = (XmlElement)natlink;
                    var natlinkname = natlinkelem.GetAttribute("name");
                    var binelems = natlinkelem.SelectNodes("./binary");
                    var nl = new IManifest.NativeLink() { Name = natlinkname };
                    foreach(var binelem in binelems)
                    {
                        var binaryelem=(XmlElement)binelem;
                        var binary=new IManifest.BinaryInfo() { Type=binaryelem.GetAttribute("type"), Path=binaryelem.GetAttribute("path"), Platform=binaryelem.GetAttribute("platform"), OS=binaryelem.GetAttribute("os"), Version=binaryelem.GetAttribute("version") };
                        nl.Binaries.Add(binary);
                    }
                    nativelinks.Add(nl);
                }
                NativeLinks = nativelinks;
                Dependencies = new List<IManifest.LibraryDependency>();
            }
        }

        public IOptional<String> MainClass
        {
            get;
        }

        public string ProgramName { get; }
        public Version Version { get; }
        public IEnumerable<IManifest.LibraryDependency> Dependencies { get; }


        public IEnumerable<IManifest.ClassInfo> Classes { get; }
        public IEnumerable<IManifest.InterfaceInfo> Interfaces { get; }

        public IEnumerable<IManifest.NativeLink> NativeLinks { get; }

        public void Emit(Func<string, Stream> opener)
        {
            using (Stream fs = opener(ProgramName))
            {
                using (StreamWriter sw = new StreamWriter(fs))
                {
                    sw.WriteLine("<?xml version=\"1.0\" encoding=\"ISO - 8859 - 1\" ?>");
                    sw.WriteLine("<nomlibrary name = \"" + ProgramName + "\" major=\"" + Version.Major.ToString() + "\" minor=\"" + Version.Minor.ToString() + "\" revision=\"" + Version.Revision.ToString() + "\" build=\"" + Version.Build.ToString() + "\" fversion=\"2\" >");
                    if (MainClass.HasElem)
                    {
                        sw.WriteLine("<mainclass name = \"" + MainClass.Elem + "\" />");
                    }
                    sw.WriteLine("<dependencies>");
                    foreach (var dep in Dependencies)
                    {
                        sw.WriteLine("<dependency major=\"" + dep.Version.Major.ToString() + "\" minor=\"" + dep.Version.Minor.ToString() + "\" revision=\"" + dep.Version.Revision.ToString() + "\" build=\"" + dep.Version.Build.ToString() + "\" name=\"" + dep.Name + "\"/>");
                    }
                    sw.WriteLine("</dependencies>");
                    sw.WriteLine("<classes>");
                    foreach (var cls in Classes)
                    {
                        sw.WriteLine("<nomclass qname=\"" + cls.Name + "\" file=\"" + cls.FileName + "\"/>");
                    }
                    sw.WriteLine("</classes>");
                    sw.WriteLine("<interfaces>");
                    foreach (var iface in Interfaces)
                    {
                        sw.WriteLine("<nominterface qname=\"" + iface.Name + "\" file=\"" + iface.FileName + "\"/>");
                    }
                    sw.WriteLine("</interfaces>");
                    if(NativeLinks.Count()>0)
                    {
                        sw.WriteLine("<native>");
                        foreach (var natlink in NativeLinks)
                        {
                            sw.WriteLine("<library name =\"" + natlink.Name + "\">");
                            foreach(var binary in natlink.Binaries)
                            {
                                sw.WriteLine("<binary type=\"" + binary.Type + "\" path=\""+binary.Path+"\" platform=\""+binary.Platform+"\" os=\""+binary.OS+"\" version=\""+binary.Version+"\"/>");
                            }
                            sw.WriteLine("</library>");
                        }
                        sw.WriteLine("</native>");
                    }
                    sw.WriteLine("</nomlibrary > ");
                }
            }
        }
    }
}
