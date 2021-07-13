using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Nom.Parser;
using Nom.Language;
namespace Nom.TypeChecker
{
    public class Program : ILibrary
    {
        public readonly string LibraryName;
        private List<ILibrary> libraries = new List<ILibrary>() { StdLib.StdLib.Instance };

        public Program(string libraryName, IEnumerable<ILibrary> libraries)
        {
            this.LibraryName = libraryName;
            this.libraries.AddRange(libraries);
            GlobalNS = new TDNamespace(this, "", Optional<ITDNamespace>.Empty);
        }

        internal TDNamespace GlobalNS { get; }

        public string Name => LibraryName;

        public Version Version => null;
        public IEnumerable<ILibrary> Libraries => libraries;

        public void AddLibrary(ILibrary lib)
        {
            libraries.Add(lib);
        }

        public INamespace GlobalNamespace => GlobalNS;
        INamespaceSpec ILibrary.GlobalNamespace => GlobalNamespace;

        public IEnumerable<IParamRef<INamespaceSpec, P>> FindVarargsChildren<P>(IArgIdentifier<string, P> name) where P : Language.ITypeArgument, ISubstitutable<P>
        {
            yield break;
        }

    }
}
