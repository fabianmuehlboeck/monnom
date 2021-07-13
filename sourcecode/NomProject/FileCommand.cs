using System;
using System.Collections.Generic;
using System.Text;
using System.IO;

namespace Nom.Project
{
    class FileCommand : ICommand
    {
        public static readonly FileCommand Instance = new FileCommand();
        private FileCommand() { }

        public IEnumerable<string> GetUsage()
        {
            yield return "addfile [project] [file-pattern] ...";
        }

        public void Run(string[] args)
        {
            if(args.Length<=2)
            {
                throw new CommandUsageException("Too few arguments");
            }
            FileInfo projfi = new FileInfo(args[1]+".mnp");
            if(!projfi.Exists)
            {
                throw new CommandUsageException("Project file not found: " + args[1]+".mnp");
            }
            NomProject proj = new NomProject(projfi);
            DirectoryInfo di = new DirectoryInfo(Environment.CurrentDirectory);
            for(int i=2; i<args.Length;i++)
            {
                foreach(var fi in  di.GetFiles(args[i]))
                {
                    proj.Files.Add(fi.Name);
                }
            }
            proj.Write(projfi);
        }
    }
}
