using System;
using System.Collections.Generic;
using System.Text;
using Nom.Language;

namespace Nom.TypeChecker
{
    internal interface ITDClass : ITDChild, IClass, ITDClassContainer
    {
        /// <summary>
        /// Fields that are directly defined in this class
        /// </summary>
        IEnumerable<IFieldDecl> LocalFields { get; }
        bool IsInheritanceInitialized
        {
            get;
        }
        void AddDefinition(TDClassDef def);
        void AddClass(TDClass cls);
    }
}
