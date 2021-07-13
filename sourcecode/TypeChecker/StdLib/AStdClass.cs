using System;
using System.Collections.Generic;
using System.Text;
using System.Linq;
using Nom.Language;
using Nom.Language.SpecExtensions;

namespace Nom.TypeChecker.StdLib
{
    public class AStdClass : AParameterized, IClassSpec
    {
        protected AStdClass(System.String name, IClassSpec superClass)
        {
            Name = name;
            SuperClass = new ClassRef<IType>(superClass, new TypeEnvironment<IType>()).InjectOptional();
            ClassType = new ClassType(this, new TypeEnvironment<ITypeArgument>());
        }

        public ClassType ClassType { get; } 

        public IOptional<IParamRef<IClassSpec, IType>> SuperClass { get; }

        public virtual bool IsFinal => true;

        public virtual bool IsExpando => false;

        public bool IsPartial => false;

        public virtual IEnumerable<IStaticMethodSpec> StaticMethods
        {
            get
            {
                yield break;
            }
        }

        public virtual IEnumerable<IFieldSpec> Fields
        {
            get
            {
                yield break;
            }
        }

        public virtual IEnumerable<IStaticFieldSpec> StaticFields
        {
            get
            {
                yield break;
            }
        }

        public virtual IEnumerable<IConstructorSpec> Constructors
        {
            get
            {
                yield break;
            }
        }

        public virtual IEnumerable<IInstanceSpec> Instances
        {
            get
            {
                yield break;
            }
        }

        public virtual bool IsShape => false;

        public virtual Visibility Visibility => Visibility.Public;

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

        public virtual IOptional<INamespaceSpec> ParentNamespace => StdLibGlobalNamespace.Instance.InjectOptional();

        protected override IOptional<IParameterizedSpec> ParamParent => ParentNamespace;
        public string Name { get; }

        public override ITypeParametersSpec TypeParameters => TypeParametersSpec.Empty;

        public IEnumerable<INamespaceSpec> Namespaces
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


        public Ret Visit<Arg, Ret>(INamespaceSpecVisitor<Arg, Ret> visitor, Arg arg = default(Arg))
        {
            return visitor.VisitClassSpec(this, arg);
        }

        public IParamRef<IClassSpec, IType> MakeClassRef(ITypeEnvironment<IType> env)
        {
            return new ClassRef<Language.IType>(this, env);
        }

        public INamedType Instantiate(ITypeEnvironment<ITypeArgument> args)
        {
            return new ClassType(this, args);
        }

    }
}
