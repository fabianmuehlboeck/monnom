using System;
using System.IO;
using System.Collections.Generic;
using Nom.Language;
using System.Linq;
using System.Diagnostics;
using System.Threading.Tasks;
using System.Collections.Concurrent;

namespace Nom
{
    namespace BenchmarkGenerator
    {
        class BenchmarkGenerator
        {
            static void Main(string[] args)
            {
                string path = ".";
                string packagename = "Program";
                string mainclassname = "Main";
                string projectname = "";
                string lambdaoptarg = " ";
                int runcount = 0;
                int optlevel = 3;
                int warmups = 0;
                bool byFile = false;
                bool highPriority = false;
                for (int i = 0; i < args.Length; i++)
                {
                    string argkey = "";
                    if (args[i].StartsWith("--"))
                    {
                        switch (args[i].Substring(2))
                        {
                            case "path":
                                argkey = "path";
                                break;
                            case "main":
                            case "mainClass":
                                argkey = "mainclass";
                                break;
                            case "run":
                            case "runs":
                                argkey = "runs";
                                break;
                            case "byfile":
                                argkey = "file";
                                break;
                            case "project":
                                argkey = "project";
                                break;
                            case "nolambdaopt":
                                argkey = "nolambdaopt";
                                break;
                            case "priority":
                                argkey = "priority";
                                break;
                            case "warmups":
                                argkey = "warmups";
                                break;
                        }
                    }
                    else if (args[i].StartsWith("-"))
                    {
                        switch (args[i].Substring(1))
                        {
                            case "p":
                                argkey = "path";
                                break;
                            case "m":
                                argkey = "mainclass";
                                break;
                            case "r":
                                argkey = "runs";
                                break;
                            case "f":
                                argkey = "file";
                                break;
                            case "o":
                                argkey = "opt";
                                break;
                            case "w":
                                argkey = "warmups";
                                break;
                        }
                    }
                    switch (argkey)
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
                        case "runs":
                            if (i + 1 < args.Length)
                            {
                                int arg;
                                if (Int32.TryParse(args[i + 1], out arg))
                                {
                                    i++;
                                    runcount = arg;
                                }
                                else
                                {
                                    runcount = args[i] == "--run" ? 1 : -1;
                                }
                            }
                            else
                            {
                                runcount = args[i] == "--run" ? 1 : -1;
                            }
                            break;
                        case "opt":
                            if (i + 1 < args.Length)
                            {
                                int arg;
                                if (Int32.TryParse(args[i + 1], out arg))
                                {
                                    i++;
                                    optlevel = arg;
                                }
                            }
                            break;
                        case "file":
                            byFile = true;
                            break;
                        case "project":
                            if (i + 1 < args.Length)
                            {
                                projectname = args[i + 1];
                                i++;
                            }
                            break;
                        case "warmups":
                            warmups = 1;
                            if(i+1<args.Length)
                            {
                                warmups = int.Parse(args[i + 1]);
                                i++;
                            }
                            break;
                        case "priority":
                            highPriority = true;
                            break;
                        case "nolambdaopt":
                            lambdaoptarg = " --nolambdaopt ";
                            break;
                    }
                }
                Nom.Project.NomProject proj;
                DirectoryInfo di = new DirectoryInfo(path);
                if (projectname.Length > 0)
                {
                    proj = new Nom.Project.NomProject(new FileInfo(di.FullName + "/" + projectname + ".mnp"));
                }
                else
                {
                    proj = new Project.NomProject(packagename);
                    proj.MainClassName = mainclassname;
                    proj.Files.AddRange(di.EnumerateFiles("*.mn").Select(f => f.Name));
                }

                List<Nom.Language.ILibrary> libraries = new List<Nom.Language.ILibrary>();
                List<Nom.Bytecode.IManifest> librariesManifests = new List<Bytecode.IManifest>();
                if (proj.LibraryFiles.Any())
                {
                    Nom.Project.NomProject libproj = new Nom.Project.NomProject(proj.Name + "_lib");
                    libproj.Version = proj.Version;
                    libproj.Files.AddRange(proj.LibraryFiles);
                    Nom.Parser.Program libraryParseProg = ParseProgram(libproj.Files.Select(f => new FileInfo(di.FullName + "/" + f)));
                    Nom.TypeChecker.Program librarytcprog = TypeCheckProgram(libraryParseProg, libproj.Name, new List<Nom.Language.ILibrary>() /*LoadDependencies(proj)*/);
                    Nom.Bytecode.AssemblyUnit libraryau = FullDevolution.GenerateBytecode(librarytcprog, libproj);
                    Bytecode.IManifest manifest = libraryau.EmitToPath(di);
                    libraries.Add(librarytcprog);
                    librariesManifests.Add(manifest);
                    proj.Dependencies.Add(new Nom.Project.NomDependency(libproj.Name, libproj.Version));
                }

                Nom.Parser.Program parseprog = ParseProgram(proj.Files.Select(f => new FileInfo(di.FullName + "/" + f)));
                Nom.TypeChecker.Program tcprog = TypeCheckProgram(parseprog, proj.Name, libraries);
                var dirs = FullDevolution.Instance.Run(parseprog, tcprog, di, libraries, librariesManifests, proj, byFile);
                //ProcessProgram(parseprog, di);
                int dircount = dirs.Count();
                for (int i = 0; i < (runcount < 0 ? Int32.MaxValue : runcount); i++)
                {
                    int dirid = 0;
                    foreach (var dir in dirs)
                    {
                        dirid++;
                        Console.WriteLine("Run " + (i + 1).ToString() + (runcount > 0 ? "/" + runcount.ToString() : "") + ": " + dir.Name + "(" + dirid.ToString() + "/" + dircount.ToString() + ")");
                        Process p = new Process();
                        ProcessStartInfo psi = p.StartInfo;
                        psi.FileName = "nom";
                        psi.Arguments = "-p \"" + dir.FullName + "\" "+(warmups>0?"-w"+warmups.ToString()+" ":"")+"-o" + (optlevel.ToString()) + lambdaoptarg + " " + proj.Name;
                        psi.WorkingDirectory = dir.FullName;
                        psi.UseShellExecute = false;
                        psi.CreateNoWindow = true;
                        psi.RedirectStandardOutput = true;
                        using (StreamWriter sw = new StreamWriter(new FileStream(dir.FullName +"/out" + (i + 1).ToString() + ".txt", FileMode.Create, FileAccess.Write)))
                        {
                            p.Start();
                            if (highPriority)
                            {
                                p.PriorityClass = ProcessPriorityClass.RealTime;
                            }
                            p.WaitForExit();

                            sw.Write(p.StandardOutput.ReadToEnd());
                            p.Dispose();
                        }
                    }
                }
                Console.WriteLine("Done!");
            }


            static Nom.Parser.Program ParseProgram(IEnumerable<FileInfo> files)
            {
                Nom.Parser.NomParser p = new Nom.Parser.NomParser();
                return p.Parse(files);
            }

            static Nom.TypeChecker.Program TypeCheckProgram(Nom.Parser.Program program, String packagename, IEnumerable<ILibrary> libraries)
            {
                Nom.TypeChecker.TypeChecker tc = new Nom.TypeChecker.TypeChecker();
                return tc.TypeCheck(program, libraries, packagename);
            }
        }

    }
}