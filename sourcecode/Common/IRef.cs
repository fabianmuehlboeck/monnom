using System;
using System.Collections.Generic;
using System.Text;

namespace Nom
{
    public interface IRef<out T>
    {
        T Element
        {
            get;
        }
    }
}
