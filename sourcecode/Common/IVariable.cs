using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Nom
{
    public interface IVariable:IEquatable<IVariable>
    {
        String VarName
        {
            get;
        }

        String CAccessor
        {
            get;
        }
        
        IEnumerable<String> CDeclaration
        {
            get;
        }
    }
}
