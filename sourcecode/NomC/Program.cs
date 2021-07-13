using System;
using System.IO;
using System.Collections.Generic;
using System.Linq;
using Nom.Parser;
using Nom.TypeChecker;
using Nom.Bytecode;

namespace NomC
{
    class Program
    {
        static void Main(string[] args)
        {
            string path = ".";
            string packagename = "Program";
            string mainclassname = "Main";
            string projectname = null;
            bool isLibrary = false;
            bool emitFlat = true;
            for(int i=0;i<args.Length;i++)
            {
                string argkey = "";
                if(args[i].StartsWith("--"))
                {
                    switch(args[i].Substring(2))
                    {
                        case "path":
                            argkey = "path";
                            break;
                        case "main":
                        case "mainClass":
                            argkey = "mainclass";
                            break;
                        case "library":
                            argkey = "library";
                            break;
                        case "project":
                            argkey = "project";
                            break;
                        case "flat":
                            argkey = "flat";
                            break;
                        case "archive":
                            argkey = "archive";
                            break;
                        default:
                            Usage("Invalid argument name: \"" + args[i].Substring(2) + "\"");
                            return;
                    }
                }
                else if(args[i].StartsWith("-"))
                {
                    switch (args[i].Substring(1))
                    {
                        case "p":
                            argkey = "path";
                            break;
                        case "m":
                            argkey = "mainclass";
                            break;
                        case "l":
                            argkey = "library";
                            break;
                        default:
                            Usage("Invalid argument name: \"" + args[i].Substring(1) + "\"");
                            return;
                    }
                }
                switch(argkey)
                {
                    case "":
                        packagename = args[i];
                        break;
                    case "path":
                        if (i + 1 >= args.Length)
                        {
                            throw new Exception("Path argument must be followed by path string");
                        }
                        path = args[i + 1];
                        i++;
                        break;
                    case "mainclass":
                        if (i + 1 >= args.Length)
                        {
                            throw new Exception("Main class argument must be followed by main class name");
                        }
                        mainclassname = args[i + 1];
                        i++;
                        break;
                    case "library":
                        isLibrary = true;
                        break;
                    case "project":
                        if(i+1>=args.Length)
                        {
                            throw new Exception("Project argument must be followed by project name");
                        }
                        projectname = args[i + 1];
                        i++;
                        break;
                    case "flat":
                        emitFlat = true;
                        break;
                    case "archive":
                        emitFlat = false;
                        break;
                }
            }
            DirectoryInfo di = new DirectoryInfo(path);
            Nom.Project.NomProject proj;
            if (projectname != null)
            {
                proj = new Nom.Project.NomProject(new FileInfo(di.FullName + "/" + projectname + ".mnp"));
                isLibrary |= proj.MainClassName == "";
                packagename = proj.Name;
            }
            else
            {
                proj = new Nom.Project.NomProject(packagename);
                proj.MainClassName = mainclassname;
                foreach (var fi in di.EnumerateFiles(".gn"))
                {
                    proj.Files.Add(fi.Name);
                }
            }
            List<Nom.Language.ILibrary> libraries = new List<Nom.Language.ILibrary>();
            if (proj.LibraryFiles.Any())
            {
                Nom.Project.NomProject libproj = new Nom.Project.NomProject(packagename + "_lib");
                libproj.Version = proj.Version;
                libproj.Files.AddRange(proj.LibraryFiles);
                Nom.Parser.Program libraryParseProg = ParseProgram(libproj.Files.Select(f => new FileInfo(di.FullName + "/" + f)));
                Nom.TypeChecker.Program librarytcprog = TypeCheckProgram(libraryParseProg, libproj.Name, new List<Nom.Language.ILibrary>());
                Nom.Bytecode.AssemblyUnit libraryau = GenerateBytecode(librarytcprog, libproj);
                if (emitFlat)
                {
                    libraryau.EmitToPath(di);
                }
                else
                {
                    libraryau.EmitArchive(new FileInfo(di.FullName + "/" + libproj.Name + ".mnar"));
                }
                libraries.Add(librarytcprog);
                proj.Dependencies.Add(new Nom.Project.NomDependency(libproj.Name, libproj.Version));
            }

            Nom.Parser.Program parseprog = ParseProgram(proj.Files.Select(f => new FileInfo(di.FullName + "/" + f)));
            Nom.TypeChecker.Program tcprog = TypeCheckProgram(parseprog, proj.Name, libraries);
            Nom.TypeChecker.TypeCheckLookup<Nom.Language.IType, Nom.Language.ITypeArgument> tcl = new TypeCheckLookup<Nom.Language.IType, Nom.Language.ITypeArgument>(tcprog);
            var mainClassTypeName = "";
            if (!isLibrary)
            {
                var mainClassType = tcl.GetChild(new RefQName(true, new List<Nom.IArgIdentifier<Identifier, Nom.Parser.IType>>() { new RefIdentifier(new Identifier(proj.MainClassName), new List<Nom.Parser.IType>()) }).Transform(arg => arg.Transform(n => n.Name, t => (Nom.Language.IType)Nom.Language.BotType.Instance))).Visit(new Nom.Language.ParamRefVisitor<object, Nom.Language.ClassType, Nom.Language.IType>(
                    (ns, arg) => throw new Exception(),
                    (ifc, arg) => throw new Exception(),
                    (cls, arg) => new Nom.Language.ClassType(cls.Element, cls.PArguments.Transform<Nom.Language.ITypeArgument>(x => x))));
                mainClassTypeName = mainClassType.Class.FullQualifiedName;
            }
            proj.MainClassName = mainClassTypeName;
            Nom.Bytecode.AssemblyUnit au = GenerateBytecode(tcprog, proj);
            if (emitFlat)
            {
                au.EmitToPath(di);
            }
            else
            {
                au.EmitArchive(new FileInfo(di.FullName + "/" + packagename + ".mnar"));
            }
            Console.WriteLine("Done!");
        }

        static Nom.Parser.Program ParseProgram(IEnumerable<FileInfo> files)
        {
            NomParser p = new NomParser();
            return p.Parse(files);
        }

        static Nom.TypeChecker.Program TypeCheckProgram(Nom.Parser.Program program, string libraryName, IEnumerable<Nom.Language.ILibrary> libraries)
        {
            TypeChecker tc = new TypeChecker();
            return tc.TypeCheck(program, libraries, libraryName);
        }


        static Nom.Bytecode.AssemblyUnit GenerateBytecode(Nom.TypeChecker.Program program, Nom.Project.NomProject project)
        {
            CodeGenerator cg = new CodeGenerator();
            return cg.GenerateBytecode(program, project);
        }

        static IEnumerable<Nom.Language.ILibrary> LoadDependencies(Nom.Project.NomProject project)
        {
            FileInfo exeFI = new FileInfo(System.Reflection.Assembly.GetEntryAssembly().Location);
            DirectoryInfo librariesDir = new DirectoryInfo(exeFI.Directory.FullName + "/libraries");
            var ret= new List<Nom.Language.ILibrary>();
            foreach(var dep in project.Dependencies)
            {
                FileInfo depManifest = new FileInfo(librariesDir.FullName + "/" + dep.QName + ".manifest");
                if(!depManifest.Exists)
                {
                    throw new TypeCheckException("Dependency not found: " + dep.QName);
                }
                Manifest manifest = new Manifest(depManifest);
                if(manifest.Version.CompareTo(dep.Version)==0)
                {
                    ret.Add(new CompiledLibrary(librariesDir, manifest));
                }
                else
                {
                    throw new TypeCheckException("Wrong version for dependency " + dep.QName+" - "+manifest.Version+" found, but "+dep.Version+" expected");
                }
            }
            return ret;
        }

        static void Usage(string message)
        {

        }
    }
}
