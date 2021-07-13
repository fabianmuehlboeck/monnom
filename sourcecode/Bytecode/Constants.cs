 using System;
using System.Collections.Generic;
using System.Text;

namespace Nom.Bytecode
{
    public class Constants
    {
        private static ulong nextConstantID = 1;
        private static Dictionary<String, ulong> stringConstants = new Dictionary<string, ulong>();
        public static ulong GetStringConstant(String s)
        {
            if(stringConstants.ContainsKey(s))
            {
                return stringConstants[s];
            }
            ulong cid = nextConstantID++;
            stringConstants[s] = cid;
            return cid;
        }

        public static ulong GetClassTypeConstant(ClassTypeRep ctr)
        {
            //TODO: actually register this
            return nextConstantID++;
        }

        public static ulong GetClassTypeListConstant(IEnumerable<ClassTypeRep> ctrs)
        {
            //TODO: actually register this
            return nextConstantID++;
        }
    }
}
