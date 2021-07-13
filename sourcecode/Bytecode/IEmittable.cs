using System;
using System.Collections.Generic;
using System.Text;
using System.IO;

namespace Nom.Bytecode
{
    public interface IEmittable
    {
        void Emit(Stream s);
    }
}
