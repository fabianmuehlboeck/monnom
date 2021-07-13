using System;
using System.Collections.Generic;
using System.Text;
using Nom.Language;
using System.Linq;

namespace Nom.TypeChecker.StdLib
{
    internal class StdLibGlobalNamespace : AParameterized, INamespaceSpec
    {
        public static StdLibGlobalNamespace Instance = new StdLibGlobalNamespace();
        private StdLibGlobalNamespace() { }

        public IOptional<INamespaceSpec> ParentNamespace { get; } = Optional<INamespaceSpec>.Empty;
        protected override IOptional<IParameterizedSpec> ParamParent => ParentNamespace;
        public string Name => "";

        public override ITypeParametersSpec TypeParameters { get; } = new TypeParametersSpec(new List<ITypeParameterSpec>());

        public IEnumerable<INamespaceSpec> Namespaces
        {
            get
            {
                yield break;
            }
        }

        public IEnumerable<IInterfaceSpec> Interfaces
        {
            get
            {
                yield return IComparable.Instance;
                yield return IPair.Instance;
                yield return IEnumerableInterface.Instance;
                yield return IEnumeratorInterface.Instance;
                foreach(var fun in Fun.GetInstances())
                {
                    yield return fun;
                }
                yield break;
            }
        }

        public IEnumerable<IClassSpec> Classes
        {
            get
            {
                yield return Object.Instance;
                yield return Null.Instance;
                yield return Void.Instance;
                yield return Bool.Instance;
                yield return Int.Instance;
                yield return Float.Instance;
                yield return String.Instance;
                yield return Timer.Instance;
                yield return ArrayList.Instance;
                yield return Math.Instance;
                yield return Range.Instance;
                yield break;
            }
        }

        public IEnumerable<INamespaceSpec> Children => Namespaces.Concat(Interfaces).Concat(Classes);

        public IEnumerable<INamespaceSpec> PublicChildren => Namespaces.Concat((Interfaces.Concat(Classes)).Where(chld => chld.Visibility >= Visibility.Public));

        public IEnumerable<INamespaceSpec> ProtectedChildren => Namespaces.Concat((Interfaces.Concat(Classes)).Where(chld => chld.Visibility >= Visibility.Protected));

        public string FullQualifiedName => "";

        public Ret Visit<Arg, Ret>(INamespaceSpecVisitor<Arg, Ret> visitor, Arg arg = default(Arg))
        {
            return visitor.VisitNamespaceSpec(this, arg);
        }
    }
}
