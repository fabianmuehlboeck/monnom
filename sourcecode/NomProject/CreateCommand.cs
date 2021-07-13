using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.Linq;

namespace Nom.Project
{
    class CreateCommand : ICommand
    {
        public static readonly CreateCommand Instance = new CreateCommand();
        private CreateCommand() { }
        public IEnumerable<string> GetUsage()
        {
            yield return "create [name] {file-pattern ...}";
        }

        public void Run(string[] args)
        {
            if(args.Length<2)
            {
                throw new CommandUsageException("create command needs at least one argument");
            }
            NomProject proj = new NomProject(args[1]);
            DirectoryInfo dir = new DirectoryInfo(Environment.CurrentDirectory);
            for (int i=2;i<args.Length;i++)
            {
                var files = dir.GetFiles(args[i]);
                if(!files.Any())
                {
                    throw new CommandUsageException("file pattern matched no files: "+args[i]);
                }
                foreach (var fi in files)
                {
                    proj.Files.Add(fi.Name);
                }
            }
            proj.Write(new FileInfo(dir.FullName + "/" + args[1] + ".mnp"));
        }
    }
}
