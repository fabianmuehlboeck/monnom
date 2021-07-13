using System;
using System.Collections.Generic;
using System.Linq;

namespace Nom.Project
{
    class Program
    {
        private static Dictionary<String, ICommand> Commands = new Dictionary<string, ICommand>();
        static void Main(string[] args)
        {
            InitCommands();
            if (args.Length<1)
            {
                Usage("Need at least one argument!");
                return;
            }
            string command = args[0];
            if(Commands.ContainsKey(command))
            {
                try
                {
                    Commands[command].Run(args);
                }
                catch(CommandUsageException e)
                {
                    Usage(e.Message);
                    return;
                }
            }
            else
            {
                Usage("Unknown command: " + command);
                return;
            }
        }

        static void InitCommands()
        {
            Commands.Clear();
            Commands["create"] = CreateCommand.Instance;
            Commands["dep"] = DependencyCommand.Instance;
            Commands["adddep"] = DependencyCommand.Instance;
            Commands["addfile"] = FileCommand.Instance;
            Commands["setmain"] = MainCommand.Instance;
        }

        static void Usage(String msg)
        {
            Console.WriteLine(msg);
            Console.WriteLine();
            Console.WriteLine("Available Commands:");
            foreach(ICommand cmd in Commands.OrderBy(cp=>cp.Key).Select(cp=>cp.Value))
            {
                foreach(string line in cmd.GetUsage())
                {
                    Console.WriteLine(line);
                }
            }
        }
    }
}
