using System;
using System.Collections.Generic;
using System.Text;

namespace Nom
{
    public class DefaultReference : IReference
    {
        public static DefaultReference Unknown = new DefaultReference();
        public static DefaultReference Global = new DefaultReference("Global");
        public DefaultReference(String name="[Unknown]")
        {
            this.Name = name;
        }
        public readonly String Name;

        public void PrettyPrint(PrettyPrinter p)
        {
            throw new NotImplementedException();
        }
    }
}
