using System;
using System.Collections.Generic;
using System.Text;
using Nom.Language;
using System.Linq;
using Nom.Language.SpecExtensions;

namespace Nom.TypeChecker.StdLib
{
    public abstract class AStdInterface : AParameterized, IInterfaceSpec
    {
        public AStdInterface(string name)
        {
            Name = name;
        }

        public abstract bool IsShape { get; }

        public bool IsExpando => false;

        public Visibility Visibility => Visibility.Public;

        public ILibrary Library => StdLib.Instance;

        public virtual IEnumerable<IParamRef<IInterfaceSpec, IType>> Implements
        {
            get
            {
                yield break;
            }
        }

        public virtual IEnumerable<IMethodSpec> Methods
        {
            get
            {
                yield break;
            }
        }

        public virtual IEnumerable<INamespaceSpec> Children
        {
            get
            {
                yield break;
            }
        }

        public IOptional<INamespaceSpec> ParentNamespace => StdLibGlobalNamespace.Instance.InjectOptional();

        public string Name { get; }

        public virtual IEnumerable<INamespaceSpec> Namespaces
        {
            get
            {
                yield break;
            }
        }

        public virtual IEnumerable<IInterfaceSpec> Interfaces
        {
            get
            {
                yield break;
            }
        }

        public virtual IEnumerable<IClassSpec> Classes
        {
            get
            {
                yield break;
            }
        }

        public IEnumerable<INamespaceSpec> PublicChildren => Namespaces.Concat<INamespaceSpec>(Interfaces.Concat<IInterfaceSpec>(Classes).Where(x => x.Visibility >= Visibility.Public));

        public IEnumerable<INamespaceSpec> ProtectedChildren => Namespaces.Concat<INamespaceSpec>(Interfaces.Concat<IInterfaceSpec>(Classes).Where(x => x.Visibility >= Visibility.Protected));

        public string FullQualifiedName => Name+"_"+TypeParameters.Count().ToString();

        protected override IOptional<IParameterizedSpec> ParamParent => ParentNamespace;

        public override ITypeParametersSpec TypeParameters => TypeParametersSpec.Empty;

        public INamedType Instantiate(ITypeEnvironment<ITypeArgument> args)
        {
            return new InterfaceType(this, args);
        }

        public INamedType Instantiate(IEnumerable<IType> args)
        {
            return new InterfaceType(this, new TypeEnvironment<ITypeArgument>(this.TypeParameters, args));
        }

        public Ret Visit<Arg, Ret>(INamespaceSpecVisitor<Arg, Ret> visitor, Arg arg = default)
        {
            return visitor.VisitInterfaceSpec(this, arg);
        }
    }
}
