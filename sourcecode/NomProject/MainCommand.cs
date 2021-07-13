using System;
using System.Collections.Generic;
using System.Text;
using System.IO;

namespace Nom.Project
{
    class MainCommand : ICommand
    {
        public static readonly MainCommand Instance = new MainCommand();
        private MainCommand() { }

        public IEnumerable<string> GetUsage()
        {
            yield return "setmain [project-name] [main-class-name]";
            yield break;
        }

        public void Run(string[] args)
        {
            if(args.Length!=3)
            {
                throw new CommandUsageException("setmain needs exactly 2 arguments");
            }
            FileInfo projfi = new FileInfo(args[1] + ".mnp");
            if (!projfi.Exists)
            {
                throw new CommandUsageException("Project file not found: " + args[1] + ".mnp");
            }
            NomProject proj = new NomProject(projfi);
            proj.MainClassName = args[2];
            proj.Write(projfi);
        }
    }
}
