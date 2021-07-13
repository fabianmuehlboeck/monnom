using System;
using System.Collections.Generic;
using System.Text;
using System.IO;

namespace Nom.Project
{
    class DependencyCommand : ICommand
    {
        public static readonly DependencyCommand Instance = new DependencyCommand();
        private DependencyCommand() { }

        public IEnumerable<string> GetUsage()
        {
            //yield return "dep add [dependency] {version}";
            yield return "adddep [project] [dependency] {version}";
        }

        public void Run(string[] args)
        {
            switch(args[0])
            {
                case "dep":
                    throw new NotImplementedException();
                case "adddep":
                    if(args.Length<3)
                    {
                        throw new CommandUsageException("adddep needs at least 3 arguments!");
                    }
                    FileInfo fi = new FileInfo(args[1] + ".mnp");
                    NomProject proj = new NomProject(fi);
                    proj.Dependencies.Add(new NomDependency(args[2], args.Length >= 4 ? new Version(args[3]) : new Version()));
                    proj.Write(fi);
                    break;
            }
        }
    }
}
