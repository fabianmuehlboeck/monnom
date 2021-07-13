using System;
using System.Collections.Generic;
using System.Text;

namespace Nom.Parser
{
    public class Program
    {
        public Program(IEnumerable<CodeFile> codeFiles)
        {
            this.CodeFiles = codeFiles;
        }
        public readonly IEnumerable<CodeFile> CodeFiles;
    }
}
