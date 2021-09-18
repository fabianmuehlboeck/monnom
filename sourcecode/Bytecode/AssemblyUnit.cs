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
        //private Dictionary<TDClassDef, ClassRep> classReps = new Dictionary<TDClassDef, ClassRep>();
        //private Dictionary<TDInterfaceDef, InterfaceRep> interfaceReps = new Dictionary<TDInterfaceDef, InterfaceRep>();
        //private Dictionary<TDMethodDecl, MethodDeclRep> methodDeclReps = new Dictionary<TDMethodDecl, MethodDeclRep>();
        //private Dictionary<ITDMethodDef, MethodDefRep> methodDefReps = new Dictionary<ITDMethodDef, MethodDefRep>();
        //private Dictionary<TDStaticMethodDef, StaticMethodDefRep> staticMethodDefReps = new Dictionary<TDStaticMethodDef, StaticMethodDefRep>();
        //private Dictionary<TDConstructorDef, ConstructorDefRep> constructorDefReps = new Dictionary<TDConstructorDef, ConstructorDefRep>();
        //private Dictionary<TDInstanceDef, InstanceDefRep> instanceDefReps = new Dictionary<TDInstanceDef, InstanceDefRep>();
        //private Dictionary<TDFieldDecl, FieldRep> fieldDefReps = new Dictionary<TDFieldDecl, FieldRep>();

        //public ClassRep GetClassRep(TDClassDef tdcd)
        //{
        //    return classReps[tdcd];
        //}

        //public void AddClass(TDUserClassDef classDef)
        //{
        //    if (classReps.ContainsKey(classDef))
        //    {
        //        throw new InternalException("Cannot lock for compilation when already locked!");
        //    }
        //    classDef.UpdateRefs();

        //    CreateMethodRepVisitor methodRepVisitor = new CreateMethodRepVisitor(this);
        //    foreach (TDMethodDecl md in classDef.DeclaredMethods)
        //    {
        //        md.VisitInstanceMethod(methodRepVisitor, null);
        //    }
        //    foreach(TDStaticMethodDef sd in classDef.StaticMethods)
        //    {
        //        staticMethodDefReps.Add(sd, new StaticMethodDefRep());
        //    }
        //    foreach(TDConstructorDef cd in classDef.Constructors)
        //    {
        //        constructorDefReps.Add(cd, new ConstructorDefRep());
        //    }
        //    foreach(TDInstanceDef id in classDef.Instances)
        //    {
        //        instanceDefReps.Add(id, new InstanceDefRep());
        //    }
        //    foreach(TDFieldDecl fd in classDef.LocalInstanceFields)
        //    {
        //        fieldDefReps.Add(fd, new FieldRep());
        //    }
        //    ClassRep classRep = ClassRep.Create(classDef.Name, new List<TypeArgRep>(), ClassTypeRep.Create(classDef.Extends.Elem.ClassType, this), classDef.Implements.Select(iref=>ClassTypeRep.Create(iref.Elem.ClassType, this)), classDef.MethodDefs.Select(md => this.methodDefReps[md]), classDef.LocalInstanceFields.Select(f=>fieldDefReps[f]), classDef.StaticMethods.Select(sm=>staticMethodDefReps[sm]));
        //    classReps.Add(classDef, classRep);
        //        // classDef.VisitNamed(new CreateClassRepVisitor(), null);

        //    //foreach (TDStaticMethodDef smd in classDef.StaticMethods)
        //    //{
        //    //    smd.CompilationLock();
        //    //}

        //    //foreach (TDConstructorDef cd in classDef.Constructors)
        //    //{
        //    //    cd.CompilationLock();
        //    //}

        //    //foreach (TDInstanceDef id in classDef.Instances)
        //    //{
        //    //    id.CompilationLock();
        //    //}
        //    //TODO: make sure field duplication checks happen in TypeDefCheck
        //    //if (classDef.Extends != null)
        //    //{
        //    //    foreach (TDFieldDecl fd in instanceFields)
        //    //    {
        //    //        if (Extends.Elem.InstanceFields.Any(f => f.FieldName == fd.FieldName))
        //    //        {
        //    //            CompilerOutput.RegisterException(new TypeDefCheckException(fd.Locs, "Field " + fd.FieldName + " was already defined in class " + Extends.Elem.InstanceFields.First(f => f.FieldName == fd.FieldName).ContainingClass.Name.ToString()));
        //    //        }
        //    //    }
        //    //}
        //}

        //public void AddInterface(TDInterfaceDef interfaceDef)
        //{

        //}

        //private class CreateClassRepVisitor : ITDNamedTypeDefVisitor<object, ClassRep>
        //{
        //    public ClassRep VisitBaseClass(TDBaseClassDef cdef, object state)
        //    {
        //        return VisitClassDef(cdef, state);
        //    }

        //    public ClassRep VisitClassDef(TDClassDef cdef, object state)
        //    {
        //        return new ClassRep(cdef.Name, new TDRef<TDClassDef>(cdef.Name, cdef));
        //    }

        //    public ClassRep VisitInterfaceDef(TDInterfaceDef idef, object state)
        //    {
        //        throw new InternalException("Class visitor used on interface");
        //        //return new InterfaceRep(idef.Name, new TDRef<TDInterfaceDef>(idef.Name, idef));
        //    }

        //    public ClassRep VisitSpecialClass(TDSpecialClassDef cdef, object state)
        //    {
        //        throw new InternalException("Cannot compile special classes to Nom Bytecode");
        //        //return new SpecialClassRep(cdef.Name, new TDRef<TDClassDef>(cdef.Name, cdef), cdef.FieldDecls, cdef.SpecialDecls, cdef.SpecialCode, cdef.SpecialInitCode);
        //    }

        //    public ClassRep VisitUserClass(TDUserClassDef cdef, object state)
        //    {
        //        return VisitClassDef(cdef, state);
        //    }
        //}

        //private class CreateMethodRepVisitor : ITDInstanceMethodVisitor<object, IMethodRep>
        //{
        //    private readonly AssemblyUnit unit;
        //    public CreateMethodRepVisitor(AssemblyUnit unit)
        //    {
        //        this.unit = unit;
        //    }

        //    public Func<TDMethodDecl, object, IMethodRep> VisitMethodDecl => (tdmd, cls) =>
        //    {
        //        MethodDeclRep mdr = new MethodDeclRep(cls, tdmd.Visibility, tdmd.MethodName, tdmd.ArgTypes, tdmd.ReturnType, tdmd.Offset);
        //        unit.methodDeclReps.Add(tdmd, mdr);
        //        return mdr;
        //    };

        //    public Func<ATDMethodDef, object, IMethodRep> VisitMethodDef => throw new InternalException("Method definition does not have specific visit method.");

        //    public Func<TDMethodDef, object, IMethodRep> VisitMethodDefImpl => (tdmd, cls) =>
        //    {
        //        MethodDefRep mdr = new MethodDefRep(cls, tdmd.Visibility, tdmd.MethodName, new ArgumentRep(tdmd.ThisArgument.Type, tdmd.ThisArgument.Register.Variable), tdmd.Arguments.Select(a => new ArgumentRep(a.Type, a.Register.Variable)), tdmd.ReturnType, tdmd.Offset, tdmd);
        //        unit.methodDefReps.Add(tdmd, mdr);
        //        return mdr;
        //    };

        //    public Func<TDMethodRef, object, IMethodRep> VisitMethodRef => throw new NotImplementedException();
        //}

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
                interfaceInfos);
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
