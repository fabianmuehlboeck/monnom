using System;
using System.Collections.Generic;
using System.Text;
using Nom.Language;

namespace Nom.TypeChecker
{
    internal class ArgumentDeclDef : IParameterSpec
    {
        public ArgumentDeclDef(Parser.Identifier name, IType type)
        {
            Identifier = name;
            Type = type;
        }
        public Parser.Identifier Identifier { get; }
        public string Name => Identifier.Name;

        public IType Type { get; }
    }
}
