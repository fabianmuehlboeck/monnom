using System;
using System.Collections.Generic;
using System.Text;

namespace Nom.Project
{
    public interface ICommand
    {
        void Run(string[] args);
        IEnumerable<string> GetUsage();
    }
}
