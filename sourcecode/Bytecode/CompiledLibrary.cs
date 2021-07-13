using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.Linq;
using System.Xml;
using Nom.Language;

namespace Nom.Bytecode
{
    public class CompiledLibrary : ILibrary
    {
        private class ProtoNamespace
        {
            public ProtoNamespace(string name, int argcount)
            {
                Name = name;
                ArgCount = argcount;
            }
            public readonly string Name;
            public readonly int ArgCount;
            public List<ProtoNamespace> Children = new List<ProtoNamespace>();
            private InterfaceRep assignedInterface=null;
            private ClassRep assignedClass=null;
            private NamespaceRep assignedNamespace=null;

            public InterfaceRep AssignedInterface
            {
                set
                {
                    if(assignedInterface!=null||assignedClass!=null||assignedNamespace!=null)
                    {
                        throw new NomBytecodeException("Naming collision!");
                    }
                    assignedInterface = value;
                }
                get
                {
                    return assignedInterface;
                }
            }
            public ClassRep AssignedClass
            {
                set
                {
                    if (assignedInterface != null || assignedClass != null || assignedNamespace != null)
                    {
                        throw new NomBytecodeException("Naming collision!");
                    }
                    assignedClass = value;
                }
                get
                {
                    return assignedClass;
                }
            }
            public NamespaceRep AssignedNamespace
            {
                set
                {
                    if (assignedInterface != null || assignedClass != null || assignedNamespace != null)
                    {
                        throw new NomBytecodeException("Naming collision!");
                    }
                    assignedNamespace = value;
                }
                get
                {
                    return assignedNamespace;
                }
            }

            public void AddToParentNS(NamespaceRep ns)
            {
                if (assignedInterface != null)
                {
                    ns.AddInterface(assignedInterface); 
                    foreach (var child in Children)
                    {
                        child.AddToParentInterface(assignedInterface);
                    }
                }
                else if(assignedClass!=null)
                {
                    ns.AddClass(assignedClass);
                    foreach (var child in Children)
                    {
                        child.AddToParentClass(assignedClass);
                    }
                }
                else
                {
                    if(assignedNamespace==null)
                    {
                        if(ArgCount>0)
                        {
                            throw new NomBytecodeException("Missing class or interface: " + Name + "<" + ArgCount + ">");
                        }
                        assignedNamespace = new NamespaceRep(Name, ns.InjectOptional());
                    }
                    ns.AddNamespace(assignedNamespace);
                    foreach(var child in Children)
                    {
                        child.AddToParentNS(assignedNamespace);
                    }
                }
            }
            public void AddToParentInterface(InterfaceRep ir)
            {
                if(assignedInterface!=null)
                {
                    ir.AddInterface(assignedInterface);
                }
                else
                {
                    throw new NomBytecodeException("Missing interface: " + Name + "<" + ArgCount + ">");
                }
            }
            public void AddToParentClass(ClassRep cr)
            {
                if(assignedInterface!=null)
                {
                    cr.AddInterface(assignedInterface);
                    foreach (var child in Children)
                    {
                        child.AddToParentInterface(assignedInterface);
                    }
                }
                else if(assignedClass!=null)
                {
                    cr.AddClass(assignedClass);
                    foreach (var child in Children)
                    {
                        child.AddToParentClass(assignedClass);
                    }
                }
                else
                {
                    throw new NomBytecodeException("Missing class or interface: " + Name + "<" + ArgCount + ">");
                }
            }
            public void AssignRoot()
            {
                foreach(var child in Children)
                {
                    child.AddToParentNS(AssignedNamespace);
                }
            }
        }
        public CompiledLibrary(DirectoryInfo basePath, IManifest manifest)
        {
            if(manifest.Dependencies.Count()>0)
            {
                throw new NotImplementedException();//needs some top-level version compatibility resolution
            }
            var globalNamespace = new NamespaceRep("", Optional<INamespaceSpec>.Empty);
            GlobalNamespace = globalNamespace;
            Name = manifest.ProgramName;
            Version = manifest.Version;
            List<ClassRep> classes = new List<ClassRep>();
            List<InterfaceRep> interfaces = new List<InterfaceRep>();
            foreach (var ifc in manifest.Interfaces)
            {
                FileInfo ilfile = new FileInfo(basePath.FullName + "/" + ifc.FileName);
                BytecodeReader bcr = new BytecodeReader();
                bcr.ReadBytecodeFile(ilfile);
                classes.AddRange(bcr.Classes);
                interfaces.AddRange(bcr.Interfaces);
            }
            foreach(var cls in manifest.Classes)
            {
                FileInfo ilfile = new FileInfo(basePath.FullName + "/" + cls.FileName);
                BytecodeReader bcr = new BytecodeReader();
                bcr.ReadBytecodeFile(ilfile);
                classes.AddRange(bcr.Classes);
                interfaces.AddRange(bcr.Interfaces);
            }
            var protoRoot = new ProtoNamespace("", 0);
            protoRoot.AssignedNamespace = globalNamespace;
            foreach(InterfaceRep ir in interfaces)
            {
                var qname = ir.NameConstant.Constant.Value;
                var qnameParts = qname.Split(".");
                var currentNS = protoRoot;
                for(int i=0;i<qnameParts.Length;i++)
                {
                    var splitpos = qnameParts[i].LastIndexOf("_");
                    if(splitpos<0)
                    {
                        throw new NomBytecodeException("Invalid class/interface name!");
                    }
                    var name = qnameParts[i].Substring(0, splitpos);
                    var argcount = int.Parse(qnameParts[i].Substring(splitpos + 1));
                    if(!currentNS.Children.Any(c=>c.Name== name && c.ArgCount==argcount))
                    {
                        currentNS.Children.Add(new ProtoNamespace(Name, argcount));
                    }
                    currentNS = currentNS.Children.Single(c => c.Name == name && c.ArgCount == argcount);
                }
                currentNS.AssignedInterface = ir;
            }
            foreach (ClassRep cr in classes)
            {
                var qname = cr.NameConstant.Constant.Value;
                var qnameParts = qname.Split(".");
                var currentNS = protoRoot;
                for (int i = 0; i < qnameParts.Length; i++)
                {
                    var splitpos = qnameParts[i].LastIndexOf("_");
                    if (splitpos < 0)
                    {
                        throw new NomBytecodeException("Invalid class/interface name!");
                    }
                    var name = qnameParts[i].Substring(0, splitpos);
                    var argcount = int.Parse(qnameParts[i].Substring(splitpos + 1));
                    if (!currentNS.Children.Any(c => c.Name == name && c.ArgCount == argcount))
                    {
                        currentNS.Children.Add(new ProtoNamespace(Name, argcount));
                    }
                    currentNS = currentNS.Children.Single(c => c.Name == name && c.ArgCount == argcount);
                }
                currentNS.AssignedClass = cr;
            }
            protoRoot.AssignRoot();
        }
        public string Name { get; }

        public Version Version { get; }

        public INamespaceSpec GlobalNamespace { get; }

        public IEnumerable<IParamRef<INamespaceSpec, P>> FindVarargsChildren<P>(IArgIdentifier<string, P> name) where P : ITypeArgument, ISubstitutable<P>
        {
            yield break;
        }
    }
}
