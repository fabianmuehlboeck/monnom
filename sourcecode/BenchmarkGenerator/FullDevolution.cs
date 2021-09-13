using Microsoft.VisualBasic;
using System;
using System.Collections.Generic;
using System.Text;
using Nom.Parser;
using Nom.Language;
using Nom.Language.SpecExtensions;
using System.Linq;
using System.Reflection.Metadata;
using System.Security.Cryptography.X509Certificates;
using System.Diagnostics;
using Nom.TypeChecker;
using System.Data;
using System.IO;
using System.Runtime.InteropServices.ComTypes;

namespace Nom.BenchmarkGenerator
{
    class FullDevolution
    {
        private static ulong FreshVarCounter = 0;
        private static string GetFreshVarID()
        {
            return "__bm_fresh_" + (FreshVarCounter++).ToString();
        }
        public static FullDevolution Instance { get; } = new FullDevolution();
        private FullDevolution() { }
        private enum ClassDevolutionKind { NominalTyped, NominalUntyped, StructTyped, StructUntyped, LambdaTyped, LambdaUntyped, StructDictionary };
        private enum InterfaceDevolutionKind { Typed, Untyped, Removed };

        private interface IFullDevolutionVersion
        {
            bool IsRelevantForInheritance { get; }
            bool IsUntyped { get; }
            bool IsPresent { get; }
            IInterfaceSpec InterfaceObject { get; }
            Ret Visit<Arg, Ret>(IFullDevolutionVersionVisitor<Arg, Ret> visitor, Arg arg = default(Arg));
            void ProcessReplacements();
            void Reset();
        }
        private interface IFullDevolutionVersionVisitor<in Arg, out Ret>
        {
            Func<ClassDevolutionVersion, Arg, Ret> VisitClassDevolutionVersion { get; }
            Func<InterfaceDevolutionVersion, Arg, Ret> VisitInterfaceDevolutionVersion { get; }
        }
        private class ClassDevolutionVersion : IFullDevolutionVersion
        {
            public ClassDevolutionVersion(IClassSpec cls, ClassDevolutionKind kind)
            {
                ClassObject = cls;
                Kind = kind;
            }
            public IClassSpec ClassObject { get; }
            public IInterfaceSpec InterfaceObject => ClassObject;
            public ClassDevolutionKind Kind { get; }

            public bool IsRelevantForInheritance => Kind == ClassDevolutionKind.NominalTyped || Kind == ClassDevolutionKind.NominalUntyped;

            public bool IsUntyped => Kind == ClassDevolutionKind.NominalUntyped || Kind == ClassDevolutionKind.LambdaUntyped || Kind == ClassDevolutionKind.StructUntyped || Kind == ClassDevolutionKind.StructDictionary;

            public bool IsPresent => Kind == ClassDevolutionKind.NominalTyped || Kind == ClassDevolutionKind.NominalUntyped;

            public Ret Visit<Arg, Ret>(IFullDevolutionVersionVisitor<Arg, Ret> visitor, Arg arg = default)
            {
                return visitor.VisitClassDevolutionVersion(this, arg);
            }

            private List<ReplacementExpr> replacements = new List<ReplacementExpr>();
            private List<MethodDecl> methodDecls = new List<MethodDecl>();
            private List<MethodDef> methodDefs = new List<MethodDef>();
            private List<FieldDecl> fields = new List<FieldDecl>();
            private List<Constructor> constructors = new List<Constructor>();
            public void RegisterReplacement(ReplacementExpr e)
            {
                replacements.Add(e);
            }
            public void RegisterMethodDecl(MethodDecl md)
            {
                methodDecls.Add(md);
            }
            public void RegisterMethodDef(MethodDef md)
            {
                methodDefs.Add(md);
            }
            public void RegisterConstructor(Constructor cd)
            {
                constructors.Add(cd);
            }
            public void RegisterField(FieldDecl fd)
            {
                fields.Add(fd);
            }
            public override string ToString()
            {
                switch (Kind)
                {
                    case ClassDevolutionKind.NominalTyped:
                        return "C";
                    case ClassDevolutionKind.NominalUntyped:
                        return "D";
                    case ClassDevolutionKind.StructTyped:
                        return "S";
                    case ClassDevolutionKind.StructUntyped:
                        return "T";
                    case ClassDevolutionKind.LambdaTyped:
                        return "L";
                    case ClassDevolutionKind.LambdaUntyped:
                        return "M";
                    case ClassDevolutionKind.StructDictionary:
                        return "U";
                    default:
                        return "X";
                }
            }

            public void ProcessReplacements()
            {
                switch (Kind)
                {
                    case ClassDevolutionKind.NominalTyped:
                        break;
                    case ClassDevolutionKind.NominalUntyped:
                        break;
                    case ClassDevolutionKind.StructTyped:
                    case ClassDevolutionKind.StructUntyped:
                        {
                            if (replacements.Count == 0)
                            {
                                break;
                            }
                            List<StructFieldDecl> sfds = new List<StructFieldDecl>();
                            Dictionary<string, Parser.IExpr> initExprs = new Dictionary<string, IExpr>();
                            foreach (FieldDecl fd in fields)
                            {
                                if (fd.InitExpr != null)
                                {
                                    initExprs[fd.Ident.Name] = fd.InitExpr;
                                }
                            }
                            if (constructors.Count > 1 || (constructors.Count == 0 && initExprs.Count != fields.Count))
                            {
                                throw new Exception("Invalid constructor configuration for structification");
                            }
                            if (constructors.Count == 0)
                            {
                                constructors.Add(new Constructor(VisibilityNode.Public, new List<VarDecl>(), new Block(new List<IStmt>(), replacements[0].Locs), new List<IExpr>(), new Block(new List<IStmt>(), replacements[0].Locs), replacements[0].Locs));
                            }
                            if (constructors.Count != 1)
                            {
                                throw new Exception("Invalid constructor configuration for structification");
                            }
                            var constructor = constructors.Single();
                            Queue<ReplacementExpr> rqueue = new Queue<ReplacementExpr>(replacements);
                            List<ReplacementExpr> newReplacements = new List<ReplacementExpr>();
                            while (rqueue.Count > 0)
                            {
                                var re = rqueue.Dequeue();
                                var rinits = new Dictionary<string, IExpr>(initExprs);
                                var substitutions = (re.OriginalExpr as NewExpr).Annotation.Substitutions;
                                var argpairs = (re.OriginalExpr as NewExpr).Args.Zip(constructor.Args, (argexp, argdec) => (argexp, argdec)).ToList();
                                Dictionary<string, IExpr> argsubsts = new Dictionary<string, IExpr>();
                                Stack<(Identifier, IExpr)> letBindings = new Stack<(Identifier, IExpr)>();
                                foreach (var pair in argpairs)
                                {
                                    if (pair.argexp is IdentExpr)
                                    {
                                        argsubsts.Add(pair.argdec.Name.Name, pair.argexp);
                                    }
                                    else
                                    {
                                        Identifier freshIdent = new Identifier(pair.argdec.Name.Name + GetFreshVarID());
                                        argsubsts.Add(pair.argdec.Name.Name, new IdentExpr(freshIdent));
                                        letBindings.Push((freshIdent, pair.argexp));
                                    }
                                }
                                //var argsubsts = argpairs.ToDictionary(p => p.Item2.Name.Name, p => p.Item1);
                                foreach (AssignStmt stmt in constructor.PreSuperStatements)
                                {
                                    if (stmt.Expression is IdentExpr)
                                    {
                                        var substExp = stmt.Expression.Visit(SubstitutionVisitor.Instance, (argsubsts, substitutions));
                                        argsubsts.Add(stmt.Variable.Name, substExp);
                                        rinits[stmt.Variable.Name] = substExp;
                                    }
                                    else
                                    {
                                        Identifier freshIdent = new Identifier(stmt.Variable.Name + GetFreshVarID());
                                        letBindings.Push((freshIdent, stmt.Expression.Visit(SubstitutionVisitor.Instance, (argsubsts, substitutions))));
                                        argsubsts.Add(stmt.Variable.Name, new IdentExpr(freshIdent));
                                        rinits[stmt.Variable.Name] = new IdentExpr(freshIdent);
                                    }
                                }
                                try
                                {
                                    re.Expr = new StructExpr(fields.Select(fd => new StructFieldDecl(((ISubstitutable<Language.IType>)fd.Type.Annotation).Substitute(substitutions).ToParserType(fd.Type.Locs), fd.Ident, rinits[fd.Ident.Name])), methodDefs.Select(md => SubstitutionVisitor.Instance.VisitMethodDef(md, (new Dictionary<string, IExpr>(), substitutions))), new List<StructAssignment>(), re.Locs);
                                    while (letBindings.Count > 0)
                                    {
                                        var top = letBindings.Pop();
                                        re.Expr = new LetVarExpr(top.Item1, top.Item2, re.Expr, re.Locs);
                                    }
                                }
                                catch (UnreplacedReplacementExprException)
                                {
                                    newReplacements.Add(re);
                                }
                            }
                            if (newReplacements.Count > 0)
                            {
                                replacements = newReplacements;
                                throw new UnreplacedReplacementExprException();
                            }
                        }
                        break;
                    case ClassDevolutionKind.StructDictionary:
                        {
                            if(replacements.Count==0)
                            {
                                break;
                            }
                            List<StructFieldDecl> sfds = new List<StructFieldDecl>();
                            Dictionary<string, Parser.IExpr> initExprs = new Dictionary<string, IExpr>();
                            foreach (FieldDecl fd in fields)
                            {
                                if (fd.InitExpr != null)
                                {
                                    initExprs[fd.Ident.Name] = fd.InitExpr;
                                }
                            }
                            if (constructors.Count > 1 || (constructors.Count == 0 && initExprs.Count != fields.Count))
                            {
                                throw new Exception("Invalid constructor configuration for structification");
                            }
                            if (constructors.Count == 0)
                            {
                                constructors.Add(new Constructor(VisibilityNode.Public, new List<VarDecl>(), new Block(new List<IStmt>(), replacements[0].Locs), new List<IExpr>(), new Block(new List<IStmt>(), replacements[0].Locs), replacements[0].Locs));
                            }
                            if (constructors.Count != 1)
                            {
                                throw new Exception("Invalid constructor configuration for structification");
                            }
                            var constructor = constructors.Single();
                            Queue<ReplacementExpr> rqueue = new Queue<ReplacementExpr>(replacements);
                            List<ReplacementExpr> newReplacements = new List<ReplacementExpr>();
                            while (rqueue.Count > 0)
                            {
                                var re = rqueue.Dequeue();
                                var rinits = new Dictionary<string, IExpr>(initExprs);
                                var substitutions = (re.OriginalExpr as NewExpr).Annotation.Substitutions;
                                var argpairs = (re.OriginalExpr as NewExpr).Args.Zip(constructor.Args, (argexp, argdec) => (argexp, argdec)).ToList();
                                Dictionary<string, IExpr> argsubsts = new Dictionary<string, IExpr>();
                                Stack<(Identifier, IExpr)> letBindings = new Stack<(Identifier, IExpr)>();
                                foreach (var pair in argpairs)
                                {
                                    if (pair.argexp is IdentExpr)
                                    {
                                        argsubsts.Add(pair.argdec.Name.Name, pair.argexp);
                                    }
                                    else
                                    {
                                        Identifier freshIdent = new Identifier(pair.argdec.Name.Name + GetFreshVarID());
                                        argsubsts.Add(pair.argdec.Name.Name, new IdentExpr(freshIdent));
                                        letBindings.Push((freshIdent, pair.argexp));
                                    }
                                }
                                //var argsubsts = argpairs.ToDictionary(p => p.Item2.Name.Name, p => p.Item1);
                                foreach (AssignStmt stmt in constructor.PreSuperStatements)
                                {
                                    if (stmt.Expression is IdentExpr)
                                    {
                                        var substExp = stmt.Expression.Visit(SubstitutionVisitor.Instance, (argsubsts, substitutions));
                                        argsubsts.Add(stmt.Variable.Name, substExp);
                                        rinits[stmt.Variable.Name] = substExp;
                                    }
                                    else
                                    {
                                        Identifier freshIdent = new Identifier(stmt.Variable.Name + GetFreshVarID());
                                        letBindings.Push((freshIdent, stmt.Expression.Visit(SubstitutionVisitor.Instance, (argsubsts, substitutions))));
                                        argsubsts.Add(stmt.Variable.Name, new IdentExpr(freshIdent));
                                        rinits[stmt.Variable.Name] = new IdentExpr(freshIdent);
                                    }
                                }
                                var structAssignments = new List<StructAssignment>();
                                foreach (FieldDecl fd in fields)
                                {
                                    structAssignments.Add(new StructAssignment(fd.Ident, rinits[fd.Ident.Name]));
                                    //fields.Select(fd => new StructFieldDecl(((ISubstitutable<Language.IType>)fd.Type.Annotation).Substitute(substitutions).ToParserType(fd.Type.Locs), fd.Ident, rinits[fd.Ident.Name]))
                                }
                                foreach (MethodDef md in methodDefs)
                                {
                                    MethodDef mds = SubstitutionVisitor.Instance.VisitMethodDef(md, (argsubsts, substitutions));
                                    structAssignments.Add(new StructAssignment(mds.Name.Name, new LambdaExpr(mds.ArgDefs, mds.Code, mds.Returns, mds.Start)));
                                }
                                try
                                {
                                    re.Expr = new StructExpr(new List<StructFieldDecl>(), new List<MethodDef>(), structAssignments, re.Locs);
                                    while (letBindings.Count > 0)
                                    {
                                        var top = letBindings.Pop();
                                        re.Expr = new LetVarExpr(top.Item1, top.Item2, re.Expr, re.Locs);
                                    }
                                }
                                catch (UnreplacedReplacementExprException)
                                {
                                    newReplacements.Add(re);
                                }
                            }
                            if (newReplacements.Count > 0)
                            {
                                replacements = newReplacements;
                                throw new UnreplacedReplacementExprException();
                            }
                        }
                        break;
                    case ClassDevolutionKind.LambdaTyped:
                    case ClassDevolutionKind.LambdaUntyped:
                        {

                            var method = methodDefs.Single();

                            if (constructors.Count == 0 && fields.All(fd => fd.InitExpr != null))
                            {
                                constructors.Add(new Constructor(VisibilityNode.Public, new List<VarDecl>(), new Block(new List<IStmt>(), method.Locs), new List<IExpr>(), new Block(new List<IStmt>(), method.Locs), method.Locs));
                            }

                            if (constructors.Count != 1)
                            {
                                throw new Exception("Invalid constructor configuration for lambdaification");
                            }
                            var constructor = constructors.Single();
                            Dictionary<string, Parser.IExpr> initExprs = new Dictionary<string, IExpr>();
                            foreach (FieldDecl fd in fields)
                            {
                                if (fd.InitExpr != null)
                                {
                                    initExprs[fd.Ident.Name] = fd.InitExpr;
                                }
                            }
                            //foreach (AssignStmt stmt in constructor.PreSuperStatements)
                            //{
                            //    var argpairs = (re.OriginalExpr as NewExpr).Args.Zip(constructor.Args, (argexp, argdec) => (argexp, argdec)).ToList();
                            //    initExprs[stmt.Variable.Name] = stmt.Expression;
                            //}

                            foreach (ReplacementExpr re in replacements)
                            {
                                var rinits = new Dictionary<string, IExpr>(initExprs);
                                var argpairs = (re.OriginalExpr as NewExpr).Args.Zip(constructor.Args, (argexp, argdec) => (argexp, argdec)).ToList();
                                var substitutions = (re.OriginalExpr as NewExpr).Annotation.Substitutions;
                                foreach (AssignStmt stmt in constructor.PreSuperStatements)
                                {
                                    rinits[stmt.Variable.Name] = stmt.Expression.Visit(SubstitutionVisitor.Instance, (argpairs.ToDictionary(p => p.Item2.Name.Name, p => p.Item1), substitutions));
                                }
                                re.Expr = new LambdaExpr(method.ArgDefs, SubstitutionVisitor.Instance.VisitBlock(method.Code, (rinits, substitutions)).AsBlock(), method.Returns, re.Start);
                            }
                        }
                        break;
                }
            }

            public void Reset()
            {
                this.methodDecls.Clear();
                this.methodDefs.Clear();
                this.fields.Clear();
                this.constructors.Clear();
                this.replacements.Clear();
            }
        }

        private class UnreplacedReplacementExprException : Exception
        {

        }

        private class SubstitutionVisitor : ConversionVisitor<(IDictionary<string, IExpr>, ITypeEnvironment<Language.ITypeArgument>)>
        {
            private SubstitutionVisitor() { }
            public static SubstitutionVisitor Instance { get; } = new SubstitutionVisitor();

            public override Func<IdentExpr, (IDictionary<string, IExpr>, ITypeEnvironment<Language.ITypeArgument>), IExpr> VisitIdentExpr => (e, s) =>
            {
                if (e.Accessor == null && s.Item1.ContainsKey(e.Name.Name))
                {
                    return s.Item1[e.Name.Name];
                }
                return e;
            };

            public override Func<Parser.ClassType, (IDictionary<string, IExpr>, ITypeEnvironment<ITypeArgument>), Parser.IType> VisitClassType => (ct, s) =>
            {
                return ((ISubstitutable<Language.IType>)ct.Annotation).Substitute(s.Item2).ToParserType(ct.Locs);
            };
        }

        private class InterfaceDevolutionVersion : IFullDevolutionVersion
        {
            public InterfaceDevolutionVersion(IInterfaceSpec ifc, InterfaceDevolutionKind kind)
            {
                InterfaceObject = ifc;
                Kind = kind;
            }
            public IInterfaceSpec InterfaceObject { get; }
            public InterfaceDevolutionKind Kind { get; }

            public bool IsRelevantForInheritance => Kind != InterfaceDevolutionKind.Removed;

            public bool IsUntyped => Kind == InterfaceDevolutionKind.Untyped;

            public bool IsPresent => Kind != InterfaceDevolutionKind.Removed;

            public Ret Visit<Arg, Ret>(IFullDevolutionVersionVisitor<Arg, Ret> visitor, Arg arg = default)
            {
                return visitor.VisitInterfaceDevolutionVersion(this, arg);
            }

            public override string ToString()
            {
                switch (Kind)
                {
                    case InterfaceDevolutionKind.Typed:
                        return "I";
                    case InterfaceDevolutionKind.Untyped:
                        return "J";
                    case InterfaceDevolutionKind.Removed:
                        return "K";
                    default:
                        return "x";
                }
            }

            public void ProcessReplacements()
            {
            }

            public void Reset()
            {
            }
        }

        public IEnumerable<DirectoryInfo> Run(Parser.Program program, TypeChecker.Program tcprog, DirectoryInfo dir, IEnumerable<ILibrary> libraries, IEnumerable<Bytecode.IManifest> manifests, Project.NomProject project, bool byFile = false)
        {
            IEnumerable<IEnumerable<IFullDevolutionVersion>> configs;
            if (byFile)
            {
                configs = GenerateFileBasedConfigs(program, tcprog);
            }
            else
            {
                configs = GenerateAllConfigs(program, tcprog);
            }
            List<(Parser.Program, DirectoryInfo, string)> progs = new List<(Parser.Program, DirectoryInfo, string)>();
            foreach (var manifest in manifests)
            {
                foreach (var ifc in manifest.Interfaces)
                {
                    ifc.FileName = "../" + ifc.FileName;
                }
                foreach (var cls in manifest.Classes)
                {
                    cls.FileName = "../" + cls.FileName;
                }
            }
            foreach (var config in configs)
            {

                foreach (var version in config)
                {
                    version.Reset();
                }
                var configname = String.Concat(config.OrderBy(x => x.InterfaceObject.FullQualifiedName).Select(x => x.ToString()));
                var subdir = dir.CreateSubdirectory(".BM_" + configname);
                Console.Write("Configuration " + configname);
                Console.Write(": Generating...");
                var data = ProcessDevolutionConfiguration(config, tcprog);
                var nprog = FullDevolutionVisitor.Instance.VisitProgram(program, data);
                Queue<IFullDevolutionVersion> vpq = new Queue<IFullDevolutionVersion>(config);
                while (vpq.Count > 0)
                {
                    var dv = vpq.Dequeue();
                    try
                    {
                        dv.ProcessReplacements();
                    }
                    catch (UnreplacedReplacementExprException)
                    {
                        vpq.Enqueue(dv);
                    }
                }
                nprog.Output(subdir);
                foreach (var manifest in manifests)
                {
                    manifest.Emit(s => new FileInfo(subdir.FullName + "/" + manifest.ProgramName + ".manifest").OpenWrite());
                }
                progs.Add((nprog, subdir, configname));
                Console.WriteLine("DONE!");
            }

            foreach (var prog in progs)
            {
                var nprog = prog.Item1;
                var subdir = prog.Item2;
                var configname = prog.Item3;
                Console.Write("Configuration " + configname);
                Console.Write(": Type-Checking...");
                try
                {
                    Nom.TypeChecker.Program tcprog2 = TypeCheckProgram(nprog, project.Name, libraries);
                    Nom.TypeChecker.TypeCheckLookup<Nom.Language.IType, Nom.Language.ITypeArgument> tcl = new TypeCheckLookup<Nom.Language.IType, Nom.Language.ITypeArgument>(tcprog2);
                    var mainClassType = tcl.GetChild(new RefQName(true, new List<Nom.IArgIdentifier<Identifier, Nom.Parser.IType>>() { new RefIdentifier(new Identifier(project.MainClassName), new List<Nom.Parser.IType>()) }).Transform(arg => arg.Transform(n => n.Name, t => (Nom.Language.IType)Nom.Language.BotType.Instance))).Visit(new Nom.Language.ParamRefVisitor<object, Nom.Language.ClassType, Nom.Language.IType>(
                        (ns, arg) => throw new Exception(),
                        (ifc, arg) => throw new Exception(),
                        (cls, arg) => new Nom.Language.ClassType(cls.Element, cls.PArguments.Transform<Nom.Language.ITypeArgument>(x => x))));
                    //Bytecode.Manifest manifest = new Bytecode.Manifest(mainClassType.ReferenceName.InjectOptional(), packagename, new List<Bytecode.IManifest.LibraryDependency>(), new List<Bytecode.IManifest.ClassInfo>(), new List<Bytecode.IManifest.InterfaceInfo>());
                    //new Bytecode.Manifest(Nom.Optional<Nom.Language.ClassType>.Inject(mainClassType), packagename, new Dictionary<ulong, Nom.Language.ILibrary>());
                    Console.Write(" Generating Bytecode...");
                    Project.NomProject clonedProj = new Project.NomProject(project);
                    clonedProj.MainClassName = mainClassType.Class.FullQualifiedName;
                    Nom.Bytecode.AssemblyUnit au = GenerateBytecode(tcprog2, clonedProj);
                    au.EmitToPath(subdir);
                    //Console.Write(" Emitting Manifest...");
                    //manifest.Emit(subdir.FullName);
                    Console.WriteLine("DONE!");
                }
                catch (ListableException e)
                {
                    Console.WriteLine("FAILED: " + e.Message);
                }
            }
            return progs.Select(p => p.Item2);
        }

        static Nom.TypeChecker.Program TypeCheckProgram(Nom.Parser.Program program, string packagename, IEnumerable<ILibrary> libraries)
        {
            TypeChecker.TypeChecker tc = new TypeChecker.TypeChecker();
            return tc.TypeCheck(program, libraries, packagename);
        }


        static internal Nom.Bytecode.AssemblyUnit GenerateBytecode(Nom.TypeChecker.Program program, Project.NomProject project)
        {
            Bytecode.CodeGenerator cg = new Bytecode.CodeGenerator();
            return cg.GenerateBytecode(program, project);
        }

        private IEnumerable<IEnumerable<IFullDevolutionVersion>> GenerateFileBasedConfigs(Parser.Program program, TypeChecker.Program tcprog)
        {
            var interfaces = tcprog.GlobalNamespace.AllInterfaces();
            var classes = tcprog.GlobalNamespace.AllClasses();

            Dictionary<IClassSpec, List<ClassDevolutionKind>> classKinds = new Dictionary<IClassSpec, List<ClassDevolutionKind>>();
            Dictionary<IInterfaceSpec, List<InterfaceDevolutionKind>> interfaceKinds = new Dictionary<IInterfaceSpec, List<InterfaceDevolutionKind>>();
            IEnumerable<IEnumerable<IFullDevolutionVersion>> configs = new List<IEnumerable<IFullDevolutionVersion>>() { new List<IFullDevolutionVersion>() };

            foreach (var codefile in program.CodeFiles)
            {
                List<IClassSpec> cfclasses = new List<IClassSpec>();
                List<IInterfaceSpec> cfinterfaces = new List<IInterfaceSpec>();
                DevolutionFileVisitor.Instance.VisitCodeFile(codefile, (cfclasses, cfinterfaces));

                List<IEnumerable<IFullDevolutionVersion>> cfconfigs = new List<IEnumerable<IFullDevolutionVersion>>() { new List<IFullDevolutionVersion>() };

                if (cfclasses.Count > 0)
                {
                    List<IEnumerable<IFullDevolutionVersion>> cfcconfigs = new List<IEnumerable<IFullDevolutionVersion>>();
                    List<IClassSpec> structifiables = new List<IClassSpec>();
                    List<IClassSpec> lambdaifiables = new List<IClassSpec>();

                    List<IFullDevolutionVersion> typedNominals = new List<IFullDevolutionVersion>();
                    List<IFullDevolutionVersion> untypedNominals = new List<IFullDevolutionVersion>();
                    foreach (var cls in cfclasses)
                    {
                        typedNominals.Add(new ClassDevolutionVersion(cls, ClassDevolutionKind.NominalTyped));
                        untypedNominals.Add(new ClassDevolutionVersion(cls, ClassDevolutionKind.NominalUntyped));
                        if (cls.StaticFields.Count() == 0 && cls.StaticMethods.Count() == 0)
                        {
                            if (cls.Methods.Count() == 1 && cls.Methods.Single().Name.Length == 0)
                            {
                                lambdaifiables.Add(cls);
                            }
                            else
                            {
                                structifiables.Add(cls);
                            }
                        }
                    }
                    cfcconfigs.Add(typedNominals);
                    cfcconfigs.Add(untypedNominals);

                    if (structifiables.Count + lambdaifiables.Count > 0)
                    {
                        List<IFullDevolutionVersion> typedStructurals = new List<IFullDevolutionVersion>();
                        List<IFullDevolutionVersion> untypedStructurals = new List<IFullDevolutionVersion>();
                        //List<IFullDevolutionVersion> dictionaryStructurals = new List<IFullDevolutionVersion>();
                        foreach (var cls in cfclasses)
                        {
                            if (lambdaifiables.Contains(cls))
                            {
                                typedStructurals.Add(new ClassDevolutionVersion(cls, ClassDevolutionKind.LambdaTyped));
                                untypedStructurals.Add(new ClassDevolutionVersion(cls, ClassDevolutionKind.LambdaUntyped));
                            }
                            else if (structifiables.Contains(cls))
                            {
                                typedStructurals.Add(new ClassDevolutionVersion(cls, ClassDevolutionKind.StructTyped));
                                untypedStructurals.Add(new ClassDevolutionVersion(cls, ClassDevolutionKind.StructUntyped));
                                //dictionaryStructurals.Add(new ClassDevolutionVersion(cls, ClassDevolutionKind.StructDictionary));
                            }
                            else
                            {
                                typedStructurals.Add(new ClassDevolutionVersion(cls, ClassDevolutionKind.NominalTyped));
                                untypedStructurals.Add(new ClassDevolutionVersion(cls, ClassDevolutionKind.NominalUntyped));
                            }
                        }
                        cfcconfigs.Add(typedStructurals);
                        cfcconfigs.Add(untypedStructurals);
                        //cfcconfigs.Add(dictionaryStructurals);
                    }
                    cfconfigs = (cfconfigs.Select(cfc => cfcconfigs.Select(cfcc => cfc.Concat(cfcc)))).Flatten().ToList();
                }
                if (cfinterfaces.Count > 0)
                {
                    List<IEnumerable<IFullDevolutionVersion>> cficonfigs = new List<IEnumerable<IFullDevolutionVersion>>();
                    List<IFullDevolutionVersion> typedNominals = new List<IFullDevolutionVersion>();
                    List<IFullDevolutionVersion> untypedNominals = new List<IFullDevolutionVersion>();
                    List<IFullDevolutionVersion> removeds = new List<IFullDevolutionVersion>();
                    foreach (var ifc in cfinterfaces)
                    {
                        typedNominals.Add(new InterfaceDevolutionVersion(ifc, InterfaceDevolutionKind.Typed));
                        untypedNominals.Add(new InterfaceDevolutionVersion(ifc, InterfaceDevolutionKind.Untyped));
                        removeds.Add(new InterfaceDevolutionVersion(ifc, InterfaceDevolutionKind.Removed));
                    }
                    cficonfigs.Add(typedNominals);
                    cficonfigs.Add(untypedNominals);
                    cficonfigs.Add(removeds);
                    cfconfigs = (cfconfigs.Select(cfc => cficonfigs.Select(cfcc => cfc.Concat(cfcc)))).Flatten().ToList();
                }
                configs = (configs.Select(cc => cfconfigs.Select(cfc => cc.Concat(cfc)))).Flatten().ToList();
            }
            return configs;
        }

        private class DevolutionFileVisitor : CheckingVisitor<(IList<IClassSpec>, IList<IInterfaceSpec>)>
        {
            public static DevolutionFileVisitor Instance { get; } = new DevolutionFileVisitor();
            private DevolutionFileVisitor() { }

            public override Func<ClassDef, (IList<IClassSpec>, IList<IInterfaceSpec>), object> VisitClassDef => (cd, data) =>
            {
                data.Item1.Add(cd.Annotation as IClassSpec);
                return base.VisitClassDef(cd, data);
            };

            public override Func<InterfaceDef, (IList<IClassSpec>, IList<IInterfaceSpec>), object> VisitInterfaceDef => (idef, data) =>
            {
                data.Item2.Add(idef.Annotation);
                return base.VisitInterfaceDef(idef, data);
            };
        }

        private IEnumerable<IEnumerable<IFullDevolutionVersion>> GenerateAllConfigs(Parser.Program program, TypeChecker.Program tcprog)
        {
            var interfaces = tcprog.GlobalNamespace.AllInterfaces();
            var classes = tcprog.GlobalNamespace.AllClasses();


            List<IClassSpec> structifiables = new List<IClassSpec>();
            List<IClassSpec> lambdaifiables = new List<IClassSpec>();

            foreach (var cls in classes)
            {
                if (cls.StaticFields.Count() == 0 && cls.StaticMethods.Count() == 0 && cls.Classes.Count() == 0 && cls.Interfaces.Count() == 0)
                {
                    if (cls.Methods.Count() == 1 && cls.Methods.Single().Name.Length == 0)
                    {
                        lambdaifiables.Add(cls);
                    }
                    else
                    {
                        structifiables.Add(cls);
                    }
                }
            }

            IEnumerable<IEnumerable<IFullDevolutionVersion>> configs = new List<IEnumerable<IFullDevolutionVersion>>() { new List<IFullDevolutionVersion>() };

            foreach (var cls in classes)
            {
                List<IFullDevolutionVersion> clsVersions = new List<IFullDevolutionVersion>();
                clsVersions.Add(new ClassDevolutionVersion(cls, ClassDevolutionKind.NominalTyped));
                clsVersions.Add(new ClassDevolutionVersion(cls, ClassDevolutionKind.NominalUntyped));
                if (structifiables.Contains(cls))
                {
                    clsVersions.Add(new ClassDevolutionVersion(cls, ClassDevolutionKind.StructTyped));
                    clsVersions.Add(new ClassDevolutionVersion(cls, ClassDevolutionKind.StructUntyped));
                }
                if (lambdaifiables.Contains(cls))
                {
                    clsVersions.Add(new ClassDevolutionVersion(cls, ClassDevolutionKind.LambdaTyped));
                    clsVersions.Add(new ClassDevolutionVersion(cls, ClassDevolutionKind.LambdaUntyped));
                }
                configs = clsVersions.Select(clsv => configs.Select(v => v.Cons(clsv).ToList())).Flatten().ToList();
            }
            foreach (var ifc in interfaces)
            {
                configs = configs.Select(v => v.Cons(new InterfaceDevolutionVersion(ifc, InterfaceDevolutionKind.Typed)).ToList())
                    .Concat(
                    configs.Select(v => v.Cons(new InterfaceDevolutionVersion(ifc, InterfaceDevolutionKind.Untyped)).ToList())
                    ).Concat(
                    configs.Select(v => v.Cons(new InterfaceDevolutionVersion(ifc, InterfaceDevolutionKind.Removed)).ToList())).ToList();
            }

            return configs;
        }

        private FullDevolutionData ProcessDevolutionConfiguration(IEnumerable<IFullDevolutionVersion> config, TypeChecker.Program tcprog)
        {
            Dictionary<IMethodSpec, (IEnumerable<Language.IType>, Language.IType)> signatures = new Dictionary<IMethodSpec, (IEnumerable<Language.IType>, Language.IType)>();
            Dictionary<IInterfaceSpec, IFullDevolutionVersion> versions = new Dictionary<IInterfaceSpec, IFullDevolutionVersion>();

            foreach (var version in config)
            {
                versions[version.InterfaceObject] = version;
            }
            var all = tcprog.GlobalNamespace.AllInterfaces().Concat(tcprog.GlobalNamespace.AllClasses()).ToList();
            foreach (var ifc in all)
            {
                foreach (var method in ifc.Methods)
                {
                    if (!signatures.ContainsKey(method))
                    {
                        var linkedMethods = GetLinkedMethods(method, all, versions);
                        var roots = linkedMethods.Where(lm => lm.Item2 /*&& versions[lm.Item1.Container as IInterfaceSpec].IsRelevantForInheritance*/).Select(lm => lm.Item1);
                        if (roots.All(r => versions.ContainsKey(r.Container as IInterfaceSpec) && versions[r.Container as IInterfaceSpec].IsUntyped))
                        {
                            foreach (var m in linkedMethods)
                            {
                                signatures[m.Item1] = (m.Item1.Parameters.Entries.Select(t => new Language.DynamicType()), new Language.DynamicType());
                            }
                        }
                        else
                        {
                            foreach (var m in linkedMethods)
                            {
                                signatures[m.Item1] = (m.Item1.Parameters.Entries.Select(ps => ps.Type), m.Item1.ReturnType);
                            }
                        }
                    }
                }
            }
            return new FullDevolutionData(signatures, versions);
        }

        private IEnumerable<(IMethodSpec, bool)> GetLinkedMethods(IMethodSpec ms, IEnumerable<IInterfaceSpec> all, IDictionary<IInterfaceSpec, IFullDevolutionVersion> versions)
        {
            Queue<IMethodSpec> processQueue = new Queue<IMethodSpec>();
            List<IMethodSpec> results = new List<IMethodSpec>() { ms };
            List<IMethodSpec> roots = new List<IMethodSpec>() { };
            processQueue.Enqueue(ms);

            while (processQueue.Count > 0)
            {
                var item = processQueue.Dequeue();
                var container = item.Container as IInterfaceSpec;

                if (versions.ContainsKey(item.Container as IInterfaceSpec) && versions[item.Container as IInterfaceSpec].IsRelevantForInheritance)
                {
                    var parents = container.GetParamRef<IInterfaceSpec, Language.IType>().AllSuperNameds().Where(ifc => (!versions.ContainsKey(ifc.Element)) || versions[ifc.Element].IsRelevantForInheritance);
                    bool foundOverridden = false;
                    foreach (var parent in parents)
                    {
                        var substs = parent.Substitutions.Transform(x => x.AsType);
                        foreach (var method in parent.Element.Methods)
                        {
                            if (item.Overrides(method, substs))
                            {
                                foundOverridden = true;
                                if (!results.Contains(method))
                                {
                                    processQueue.Enqueue(method);
                                    results.Add(method);
                                }
                            }
                        }
                    }
                    if (!foundOverridden)
                    {
                        roots.Add(item);
                    }
                    var children = all.Where(ifc => versions.ContainsKey(ifc) && versions[ifc].IsRelevantForInheritance && ifc.GetParamRef<IInterfaceSpec, Language.IType>().AllSuperNameds().Any(sn => sn.Element == ifc)).Select(ifc => ifc.GetParamRef<IInterfaceSpec, Language.IType>().AllSuperNameds().Single(sn => sn.Element == ifc));
                    foreach (var child in children)
                    {
                        var substs = child.AllSuperNameds().Single(sn => sn.Element == container).Substitutions.Transform(x => x.AsType);
                        foreach (var method in child.Element.Methods)
                        {
                            if (method.Overrides(item, substs))
                            {
                                if (!results.Contains(method))
                                {
                                    processQueue.Enqueue(method);
                                    results.Add(method);
                                }
                            }
                        }
                    }
                }
                else
                {
                    roots.Add(item);
                }
            }
            return results.Select(r => (r, roots.Contains(r)));
        }


        private interface IFullDevolutionData
        {
            IDictionary<IMethodSpec, (IEnumerable<Language.IType>, Language.IType)> Signatures { get; }
            IDictionary<IInterfaceSpec, IFullDevolutionVersion> Versions { get; }
            void RegisterMethodDecl(MethodDecl md);
            void RegisterMethodDef(MethodDef md);
            void RegisterConstructor(Constructor cd);
            void RegisterField(FieldDecl fd);

            IFullDevolutionData CreateInContainerData(IInterfaceSpec container);
        }

        private class FullDevolutionData : IFullDevolutionData
        {
            public FullDevolutionData(IDictionary<IMethodSpec, (IEnumerable<Language.IType>, Language.IType)> signatures, IDictionary<IInterfaceSpec, IFullDevolutionVersion> versions)
            {
                Signatures = signatures;
                Versions = versions;
            }

            public IDictionary<IMethodSpec, (IEnumerable<Language.IType>, Language.IType)> Signatures { get; }
            public IDictionary<IInterfaceSpec, IFullDevolutionVersion> Versions { get; }

            public void RegisterConstructor(Constructor cd)
            {
                throw new InternalException("Cannot register members while no container exists!");
            }

            public void RegisterField(FieldDecl fd)
            {
                throw new InternalException("Cannot register members while no container exists!");
            }

            public void RegisterMethodDecl(MethodDecl md)
            {
                throw new InternalException("Cannot register members while no container exists!");
            }

            public void RegisterMethodDef(MethodDef md)
            {
                throw new InternalException("Cannot register members while no container exists!");
            }

            public IFullDevolutionData CreateInContainerData(IInterfaceSpec container)
            {
                return new FullDevolutionDataContainer(this, container);
            }

            private class FullDevolutionDataContainer : IFullDevolutionData
            {
                private IFullDevolutionData parent;
                private IInterfaceSpec container;
                public FullDevolutionDataContainer(IFullDevolutionData parent, IInterfaceSpec container)
                {
                    this.parent = parent;
                    this.container = container;
                }

                public IDictionary<IMethodSpec, (IEnumerable<Language.IType>, Language.IType)> Signatures => parent.Signatures;

                public IDictionary<IInterfaceSpec, IFullDevolutionVersion> Versions => parent.Versions;

                public IFullDevolutionData CreateInContainerData(IInterfaceSpec container)
                {
                    return new FullDevolutionDataContainer(this, container);
                }

                public void RegisterConstructor(Constructor cd)
                {
                    (Versions[container] as ClassDevolutionVersion).RegisterConstructor(cd);
                }

                public void RegisterField(FieldDecl fd)
                {
                    (Versions[container] as ClassDevolutionVersion).RegisterField(fd);
                }

                public void RegisterMethodDecl(MethodDecl md)
                {
                    (Versions[container] as ClassDevolutionVersion)?.RegisterMethodDecl(md);
                }

                public void RegisterMethodDef(MethodDef md)
                {
                    var kind = (Versions[container] as ClassDevolutionVersion).Kind;
                    (Versions[container] as ClassDevolutionVersion).RegisterMethodDef(md);
                }
            }
        }


        private class FullDevolutionVisitor : ConversionVisitor<IFullDevolutionData>
        {
            private class TypeArgumentConversionVisitor : Language.ITypeVisitor<IFullDevolutionData, Language.IType>
            {
                public TypeArgumentConversionVisitor(Language.IType replacement)
                {
                    Replacement = replacement;
                }
                public readonly Language.IType Replacement;
                public Func<BotType, IFullDevolutionData, Language.IType> VisitBotType => (t, data) => t;

                public Func<Language.ClassType, IFullDevolutionData, Language.IType> VisitClassType => (t, data) =>
                {
                    if (data.Versions.ContainsKey(t.Class))
                    {
                        if (!data.Versions[t.Class].IsPresent)
                        {
                            return Replacement;
                        }
                    }
                    List<Language.IType> args = new List<Language.IType>();
                    bool different = false;
                    foreach (var targ in t.Arguments)
                    {
                        var ntarg = targ.AsType.Visit(this, data);
                        if (ntarg != targ)
                        {
                            different = true;
                        }
                        args.Add(ntarg);
                    }
                    if (different)
                    {
                        return new Language.ClassType(t.Class, new TypeEnvironment<ITypeArgument>(t.Class.AllTypeParameters, args));
                    }
                    return t;
                };

                public Func<InterfaceType, IFullDevolutionData, Language.IType> VisitInterfaceType => (t, data) =>
                {
                    if (data.Versions.ContainsKey(t.Element))
                    {
                        if (!data.Versions[t.Element].IsPresent)
                        {
                            return Replacement;
                        }
                    }
                    List<Language.IType> args = new List<Language.IType>();
                    bool different = false;
                    foreach (var targ in t.Arguments)
                    {
                        var ntarg = targ.AsType.Visit(this, data);
                        if (ntarg != targ)
                        {
                            different = true;
                        }
                        args.Add(ntarg);
                    }
                    if (different)
                    {
                        return new Language.InterfaceType(t.Element, new TypeEnvironment<ITypeArgument>(t.Element.AllTypeParameters, args));
                    }
                    return t;
                };

                public Func<TopType, IFullDevolutionData, Language.IType> VisitTopType => (t, data) => t;

                public Func<Language.MaybeType, IFullDevolutionData, Language.IType> VisitMaybeType => (t, data) =>
                {
                    var pt = t.PotentialType.Visit(this, data);
                    if (pt != t.PotentialType)
                    {
                        return new Language.MaybeType(pt);
                    }
                    return pt;
                };

                public Func<Language.ProbablyType, IFullDevolutionData, Language.IType> VisitProbablyType => (t, data) =>
                {
                    var pt = t.PotentialType.Visit(this, data);
                    if (pt != t.PotentialType)
                    {
                        return new Language.ProbablyType(pt);
                    }
                    return pt;
                };

                public Func<TypeVariable, IFullDevolutionData, Language.IType> VisitTypeVariable => (t, data) => t;

                public Func<Language.DynamicType, IFullDevolutionData, Language.IType> VisitDynamicType => (t, data) => t;
            }

            public static FullDevolutionVisitor Instance { get; } = new FullDevolutionVisitor();
            protected FullDevolutionVisitor() { }
            //public override Func<IArgIdentifier<Identifier, Parser.IType>, IFullDevolutionData, RefIdentifier> VisitRefIdentifier => (ri, data) => ri as RefIdentifier ?? new RefIdentifier(ri.Name, ri.Arguments, ri.Name.Locs);
            //public override Func<IArgIdentifier<Identifier, TypeArgDecl>, IFullDevolutionData, DeclIdentifier> VisitDeclIdentifier => (di, data) => di as DeclIdentifier ?? new DeclIdentifier(di.Name, di.Arguments, di.Name.Locs);
            public override Func<IArgIdentifier<Identifier, Parser.IType>, IFullDevolutionData, RefIdentifier> VisitRefIdentifier => (ri, data) =>
            {
                bool different = false;
                List<Parser.IType> args = new List<Parser.IType>();
                TypeArgumentConversionVisitor tacv = new TypeArgumentConversionVisitor(new Language.DynamicType());
                foreach (var arg in ri.Arguments)
                {
                    var tp = arg.Annotation;
                    var ntp = tp.Visit(tacv, data);
                    if (tp != ntp)
                    {
                        different = true;
                        args.Add(ntp.ToParserType(arg.Locs));
                    }
                    else
                    {
                        args.Add(arg);
                    }
                }
                if (different)
                {
                    return new RefIdentifier(ri.Name, args, ri.Name.Locs);
                }
                return ri as RefIdentifier ?? new RefIdentifier(ri.Name, ri.Arguments, ri.Name.Locs);
            };
            public override Func<IArgIdentifier<Identifier, TypeArgDecl>, IFullDevolutionData, DeclIdentifier> VisitDeclIdentifier => (di, data) =>
            {
                //bool different = false;
                //List<Parser.TypeArgDecl> args = new List<Parser.TypeArgDecl>();
                //TypeArgumentConversionVisitor tacv = new TypeArgumentConversionVisitor(new Language.DynamicType());
                //foreach (var arg in di.Arguments)
                //{
                //    var tp = arg.Annotation.;
                //    var ntp = tp.Visit(tacv, data);
                //    if (tp != ntp)
                //    {
                //        different = true;
                //        args.Add(ntp.ToParserType(arg.Locs));
                //    }
                //    else
                //    {
                //        args.Add(arg);
                //    }
                //}
                //if (different)
                //{
                //    return new DeclIdentifier(di.Name, args, di.Name.Locs);
                //}
                return di as DeclIdentifier ?? new DeclIdentifier(di.Name, di.Arguments, di.Name.Locs);
            };

            public override Func<Parser.ClassType, IFullDevolutionData, Parser.IType> VisitClassType => (ct, data) =>
            {
                var ifc = (ct.Annotation as Nom.Language.INamedType)?.Element;
                if (ifc != null && data.Versions.ContainsKey(ifc) && !data.Versions[ifc].IsRelevantForInheritance)
                {
                    return new Parser.DynamicType(ct.Locs) { Annotation = new Language.DynamicType() };
                }
                return base.VisitClassType(ct, data);
            };
            public override Func<MethodDecl, IFullDevolutionData, MethodDecl> VisitMethodDecl => (md, data) =>
            {
                var sig = data.Signatures[md.Annotation];
                bool different = false;
                var args = VisitList(sig.Item1.Zip(md.Args, (tp, vd) => new VarDecl(vd.Name, tp.ToParserType(vd.Locs).Visit(FullDevolutionVisitor.Instance, data), vd.Locs)), data, VisitVarDecl, ref different);
                var returns = sig.Item2.ToParserType(md.Returns.Locs).Visit(FullDevolutionVisitor.Instance, data);
                var nmd = new MethodDecl(md.Visibility, md.IsCallTarget, md.Name, args, returns, md.Locs);
                data.RegisterMethodDecl(nmd);
                return nmd;
            };
            public override Func<MethodDef, IFullDevolutionData, MethodDef> VisitMethodDef => (md, data) =>
            {
                var sig = data.Signatures[md.Annotation];
                bool different = false;
                var args = VisitList(sig.Item1.Zip(md.Args, (tp, vd) => new VarDecl(vd.Name, tp.ToParserType(vd.Locs).Visit(FullDevolutionVisitor.Instance, data), vd.Locs)), data, VisitVarDecl, ref different);
                var returns = sig.Item2.ToParserType(md.Returns.Locs).Visit(FullDevolutionVisitor.Instance, data);
                var code = md.Code.Visit(this, data).AsBlock();
                var nmd = new MethodDef(md.IsFinal, md.IsVirtual, md.IsOverride, md.IsCallTarget, md.Visibility, md.Name, args, returns, code, md.Locs);
                data.RegisterMethodDef(nmd);
                return nmd;
            };

            public override Func<FieldDecl, IFullDevolutionData, FieldDecl> VisitFieldDecl => (md, data) =>
            {
                var nfd = base.VisitFieldDecl(md, data);
                data.RegisterField(nfd);
                return nfd;
            };


            public override Func<Constructor, IFullDevolutionData, Constructor> VisitConstructorDef => (cd, data) =>
            {
                var ncd = base.VisitConstructorDef(cd, data);
                data.RegisterConstructor(ncd);
                return ncd;
            };

            private static IEnumerable<ClassDevolutionKind> classRetainKinds = new ClassDevolutionKind[] { ClassDevolutionKind.NominalTyped, ClassDevolutionKind.NominalUntyped };
            public override Func<Namespace, IFullDevolutionData, Namespace> VisitNamespaceDef => (ns, data) =>
            {
                var ret = base.VisitNamespaceDef(ns, data);
                return new Namespace(ns.Name,
                    ret.Interfaces.Where(iface => (data.Versions[iface.Annotation] as InterfaceDevolutionVersion).Kind != InterfaceDevolutionKind.Removed).ToList(),
                    ret.Classes.Where(cls => classRetainKinds.Contains((data.Versions[cls.Annotation] as ClassDevolutionVersion).Kind)).ToList(),
                    ret.Namespaces,
                    ns.Locs);
            };

            public override Func<CodeFile, IFullDevolutionData, CodeFile> VisitCodeFile => (cf, data) =>
            {
                var ret = base.VisitCodeFile(cf, data);
                return new CodeFile(ret.FileName, ret.Usings, ret.Interfaces.Where(iface => (data.Versions[iface.Annotation] as InterfaceDevolutionVersion).Kind != InterfaceDevolutionKind.Removed).ToList(),
                    ret.Classes.Where(cls => classRetainKinds.Contains((data.Versions[cls.Annotation] as ClassDevolutionVersion).Kind)).ToList(),
                    ret.Namespaces);
            };

            public override Func<NewExpr, IFullDevolutionData, IExpr> VisitNewExpr => (e, data) =>
            {
                var ne = base.VisitNewExpr(e, data);
                if (data.Versions.ContainsKey(e.Annotation.Element))
                {
                    var version = data.Versions[e.Annotation.Element] as ClassDevolutionVersion;
                    switch (version.Kind)
                    {
                        case ClassDevolutionKind.NominalTyped:
                        case ClassDevolutionKind.NominalUntyped:
                            return ne;
                        case ClassDevolutionKind.StructTyped:
                        case ClassDevolutionKind.StructUntyped:
                        case ClassDevolutionKind.StructDictionary:
                        case ClassDevolutionKind.LambdaTyped:
                        case ClassDevolutionKind.LambdaUntyped:
                            var re = new ReplacementExpr(ne);
                            version.RegisterReplacement(re);
                            return re;
                        default:
                            throw new NotImplementedException();
                    }
                }
                bool different = false;
                var newRefQNameParts = e.NewCall.Type.Select(x =>
                {
                    var newArgs = x.Arguments.Select(a => a.Visit(this, data));
                    if (newArgs.Zip(x.Arguments, (a, b) => (a != b)).Any(x => x))
                    {
                        different = true;
                        return new AArgIdentifier<Identifier, Parser.IType>(x.Name, newArgs);
                    }
                    return x;
                }).ToList();
                if (different)
                {
                    ne = new NewExpr(new NewCall(new RefQName(false, newRefQNameParts), e.Locs), ((NewExpr)ne).Args, e.Locs);
                }
                return ne;
            };

            public override Func<ClassDef, IFullDevolutionData, ClassDef> VisitClassDef => (cd, data) =>
            {
                var version = data.Versions[cd.Annotation] as ClassDevolutionVersion;
                switch (version.Kind)
                {
                    case ClassDevolutionKind.NominalTyped:
                    case ClassDevolutionKind.StructTyped:
                    case ClassDevolutionKind.LambdaTyped:
                        var ret = base.VisitClassDef(cd, data.CreateInContainerData(cd.Annotation));
                        if (cd.Implements.Any(impl => data.Versions.ContainsKey(impl.Annotation.Element) && !data.Versions[impl.Annotation.Element].IsRelevantForInheritance))
                        {
                            ret = new ClassDef(ret.Name, ret.SuperClass, ret.Implements.Where(impl => !(data.Versions.ContainsKey(impl.Annotation.Element) && !data.Versions[impl.Annotation.Element].IsRelevantForInheritance)), ret.ImplementedMethods, ret.Fields, ret.Constructors, ret.StaticFields, ret.StaticMethods, ret.Instances, ret.IsFinal, ret.Visibility, ret.IsAbstract, ret.IsPartial, ret.IsShape, ret.IsMaterial, ret.Interfaces, ret.Classes, ret.Locs, ret.IsSpecial);
                        }
                        ret.Annotation = cd.Annotation;
                        return ret;
                    case ClassDevolutionKind.NominalUntyped:
                    case ClassDevolutionKind.StructUntyped:
                    case ClassDevolutionKind.LambdaUntyped:
                    case ClassDevolutionKind.StructDictionary:
                        ret = FullDevolutionVisitorUntyped.Instance.VisitClassDef(cd, data.CreateInContainerData(cd.Annotation));
                        if (cd.Implements.Any(impl => data.Versions.ContainsKey(impl.Annotation.Element) && !data.Versions[impl.Annotation.Element].IsRelevantForInheritance))
                        {
                            ret = new ClassDef(ret.Name, ret.SuperClass, ret.Implements.Where(impl => !(data.Versions.ContainsKey(impl.Annotation.Element) && !data.Versions[impl.Annotation.Element].IsRelevantForInheritance)), ret.ImplementedMethods, ret.Fields, ret.Constructors, ret.StaticFields, ret.StaticMethods, ret.Instances, ret.IsFinal, ret.Visibility, ret.IsAbstract, ret.IsPartial, ret.IsShape, ret.IsMaterial, ret.Interfaces, ret.Classes, ret.Locs, ret.IsSpecial);
                        }
                        ret.Annotation = cd.Annotation;
                        return ret;
                    default:
                        throw new NotImplementedException();
                }
            };

            protected Func<ClassDef, IFullDevolutionData, ClassDef> BaseVisitClassDef => base.VisitClassDef;

            public override Func<InterfaceDef, IFullDevolutionData, InterfaceDef> VisitInterfaceDef => (idef, data) =>
            {
                var version = data.Versions[idef.Annotation] as InterfaceDevolutionVersion;
                switch (version.Kind)
                {
                    case InterfaceDevolutionKind.Typed:
                        var nidef = base.VisitInterfaceDef(idef, data.CreateInContainerData(idef.Annotation));
                        nidef.Annotation = idef.Annotation;
                        return nidef;
                    case InterfaceDevolutionKind.Untyped:
                        nidef = FullDevolutionVisitorUntyped.Instance.VisitInterfaceDef(idef, data.CreateInContainerData(idef.Annotation));
                        nidef.Annotation = idef.Annotation;
                        return nidef;
                    case InterfaceDevolutionKind.Removed:
                        return idef;
                    default:
                        throw new NotImplementedException();
                }
            };

            protected Func<InterfaceDef, IFullDevolutionData, InterfaceDef> BaseVisitInterfaceDef => base.VisitInterfaceDef;
        }

        private class FullDevolutionVisitorUntyped : FullDevolutionVisitor
        {
            public static new FullDevolutionVisitorUntyped Instance { get; } = new FullDevolutionVisitorUntyped();
            private FullDevolutionVisitorUntyped() { }

            public override Func<Parser.ClassType, IFullDevolutionData, Parser.IType> VisitClassType =>
                (ct, data) =>
                {
                    var ifc = (ct.Annotation as Nom.Language.INamedType)?.Element;
                    if (ifc == null || !data.Versions.ContainsKey(ifc) || !data.Versions[ifc].IsRelevantForInheritance)
                    {
                        return new Parser.DynamicType(ct.Locs) { Annotation = new Language.DynamicType() };
                    }
                    return ct; //base.VisitClassType(ct, data);
                };
            //new Parser.DynamicType(ct.Locs) { Annotation = new Language.DynamicType() };

            public override Func<ClassDef, IFullDevolutionData, ClassDef> VisitClassDef => (cd, data) =>
            {
                var ret = BaseVisitClassDef(cd, data.CreateInContainerData(cd.Annotation));
                if (cd.Implements.Any(impl => data.Versions.ContainsKey(impl.Annotation.Element) && !data.Versions[impl.Annotation.Element].IsRelevantForInheritance))
                {
                    ret = new ClassDef(ret.Name, ret.SuperClass, ret.Implements.Where(impl => !(data.Versions.ContainsKey(impl.Annotation.Element) && !data.Versions[impl.Annotation.Element].IsRelevantForInheritance)), ret.ImplementedMethods, ret.Fields, ret.Constructors, ret.StaticFields, ret.StaticMethods, ret.Instances, ret.IsFinal, ret.Visibility, ret.IsAbstract, ret.IsPartial, ret.IsShape, ret.IsMaterial, ret.Interfaces, ret.Classes, ret.Locs, ret.IsSpecial);
                }
                ret.Annotation = cd.Annotation;
                return ret;
            };
            public override Func<InterfaceDef, IFullDevolutionData, InterfaceDef> VisitInterfaceDef => (idef, data) =>
            {
                var ret = BaseVisitInterfaceDef(idef, data.CreateInContainerData(idef.Annotation));
                ret.Annotation = idef.Annotation;
                return ret;
            };
        };

        private class ReplacementExpr : Parser.AExpr
        {
            public ReplacementExpr(IExpr original) : base(original.Locs)
            {
                OriginalExpr = original;
            }

            public IExpr Expr { get; set; }

            public IExpr OriginalExpr { get; }

            public override Parser.IType TypeAnnotation
            {
                get => Expr?.TypeAnnotation ?? OriginalExpr.TypeAnnotation;
                set => throw new Exception();
            }

            public override IEnumerable<Identifier> FreeVars => Expr?.FreeVars;

            public override void PrettyPrint(PrettyPrinter p)
            {
                Expr?.PrettyPrint(p);
            }

            public override R Visit<S, R>(IExprVisitor<S, R> visitor, S state)
            {
                if (Expr == null)
                {
                    throw new UnreplacedReplacementExprException();
                }
                return Expr.Visit(visitor, state);
            }
        }
    }
}
