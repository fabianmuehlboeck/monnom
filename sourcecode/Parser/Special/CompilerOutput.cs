using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Nom
{
    public class CompilerOutput
    {
        public static bool HasErrors
        {
            get;
            private set;
        } = false;
        public static bool HasPriorityErrors
        {
            get;
            private set;
        } = false;

        static CompilerOutput()
        {
        }

        public static void Warn(string warning)
        {
            Console.WriteLine(warning);
        }
        public static void Warn(string warning, params IReference[] refs)
        {
            Console.WriteLine(warning.FillReferences(refs));
        }
        public static void Warn(string warning, ISourcePos pos)
        {
            Console.WriteLine(pos.ToString() + ": " + warning);
        }
        public static void Warn(string warning, ISourceLocs locs)
        {
            Console.WriteLine(locs.ToString() + ": " + warning);
        }

        public static void Debug(string debugMsg)
        {
            Console.WriteLine("DEBUG: "+debugMsg);
        }

        public static void LightWarn(string warning)
        {
            Console.WriteLine(warning);
        }
        public static void LightWarn(string warning, ISourceLocs locs)
        {
            Console.WriteLine("Warning: "+locs.ToString() + ": " + warning);
        }

        public static void RegisterException(NomException e)
        {
            Console.WriteLine(e.Message);
            HasErrors = true;
        }

        public static void RegisterPriorityException(NomException e)
        {
            Console.WriteLine(e.Message);
            HasErrors = true;
            HasPriorityErrors = true;
        }
    }
}
