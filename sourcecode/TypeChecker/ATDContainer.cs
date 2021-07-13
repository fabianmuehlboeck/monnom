using System;
using System.Collections.Generic;
using System.Text;
using System.Linq;
using Nom.Parser;
using Nom.Language;

namespace Nom.TypeChecker
{
    internal abstract class ATDContainer : AParameterized, INamespaceSpec, ITDChild, ITDInterfaceContainer
    {
        private Dictionary<string, Dictionary<int, ITDChild>> children = new Dictionary<string, Dictionary<int, ITDChild>>();
        protected ATDContainer(Program program, string name, IOptional<ITDChild> parentNamespace)
        {
            this.Name = name;
            this.ParentNamespace = parentNamespace;
            this.Program = program;
        }

        public string Name
        {
            get;
            private set;
        }

        private List<TDNamespace> tdnamespaces = new List<TDNamespace>();

        public IEnumerable<TDNamespace> TDNamespaces
        {
            get
            {
                return tdnamespaces.ToList();
            }
        }
        public IEnumerable<INamespace> Namespaces => TDNamespaces;
        IEnumerable<INamespaceSpec> INamespaceSpec.Namespaces => TDNamespaces;

        public void AddNamespace(TDNamespace ns)
        {
            if (children.ContainsKey(ns.Name))
            {
                children[ns.Name].Add(0, ns);
            }
            else
            {
                children.Add(ns.Name, new Dictionary<int, ITDChild>() { [0] = ns });
            }
            tdnamespaces.Add(ns);
        }

        private List<TDInterface> tdinterfaces = new List<TDInterface>();
        public IEnumerable<TDInterface> TDInterfaces
        {
            get
            {
                return tdinterfaces.ToList();
            }
        }

        public IEnumerable<IInterface> Interfaces => TDInterfaces;
        IEnumerable<IInterfaceSpec> INamespaceSpec.Interfaces => TDInterfaces;

        public void AddInterface(TDInterface iface)
        {
            if (children.ContainsKey(iface.Name))
            {
                children[iface.Name].Add(0, iface);
            }
            else
            {
                children.Add(iface.Name, new Dictionary<int, ITDChild>() { [0] = iface });
            }
            tdinterfaces.Add(iface);
        }

        private List<TDClass> tdclasses = new List<TDClass>();
        public IEnumerable<TDClass> TDClasses
        {
            get
            {
                return tdclasses.ToList();
            }
        }
        public IEnumerable<IClass> Classes => TDClasses;
        IEnumerable<IClassSpec> INamespaceSpec.Classes => TDClasses;

        public void AddClass(TDClass cls)
        {
            if (children.ContainsKey(cls.Name))
            {
                children[cls.Name].Add(0, cls);
            }
            else
            {
                children.Add(cls.Name, new Dictionary<int, ITDChild>() { [0] = cls });
            }
            tdclasses.Add(cls);
        }

        public bool HasChild(String name, int argcount)
        {
            return children.ContainsKey(name) && children[name].ContainsKey(argcount);
        }

        public bool HasChild<T>(IArgIdentifier<String, T> identifier) where T: INamedReference
        {
            return HasChild(identifier.Name, identifier.Arguments.Count());
        }
        public bool HasChild<T>(IArgIdentifier<Identifier, T> identifier) where T : INamedReference
        {
            return HasChild(identifier.Name.Name, identifier.Arguments.Count());
        }

        public INamespaceSpec this[String name, int argcount]
        {
            get
            {
                if (children.ContainsKey(name))
                {
                    if (children[name].ContainsKey(argcount))
                    {
                        return children[name][argcount];
                    }
                }
                throw new InternalException("Element " + name + "$" + argcount.ToString() + " does not exist!");
            }
        }

        public IOptional<INamespaceSpec> GetChild(String name, int argcount)
        {
            if(children.ContainsKey(name))
            {
                if(children[name].ContainsKey(argcount))
                {
                    return children[name][argcount].InjectOptional();
                }
            }
            return Optional<INamespaceSpec>.Empty;
        }

        public IOptional<INamespaceSpec> GetChild<T>(IArgIdentifier<String, T> identifier) where T : INamedReference
        {
            return GetChild(identifier.Name, identifier.Arguments.Count());
        }
        public IOptional<INamespaceSpec> GetChild<T>(IArgIdentifier<Identifier, T> identifier) where T : INamedReference
        {
            return GetChild(identifier.Name.Name, identifier.Arguments.Count());
        }


        public override string ToString()
        {
            return ParentNamespace.Extract((nss) => nss.ToString() + ".", "") + this.Name + "$0";
        }


        public Program Program { get; }

        public ILibrary Library => Program;

        public IEnumerable<ITDChild> Children => tdnamespaces.Concat<ITDChild>(tdinterfaces).Concat(tdclasses).ToList();
        IEnumerable<INamespaceSpec> INamespaceSpec.Children => Children;

        public IOptional<INamespaceSpec> ParentNamespace
        {
            get;
            private set;
        }

        protected override IOptional<IParameterizedSpec> ParamParent => ParentNamespace;
        public override ITypeParametersSpec TypeParameters => new TypeParametersSpec(new List<ITypeParameterSpec>());

        IEnumerable<INamespaceSpec> INamespaceSpec.PublicChildren => PublicChildren;

        IEnumerable<INamespaceSpec> INamespaceSpec.ProtectedChildren => ProtectedChildren;

        public string FullQualifiedName => ParentNamespace.Extract(pns => (pns.FullQualifiedName.Length>0)?(pns.FullQualifiedName + (Name.Length>0?("." + Name):"")):Name, Name)+(Name.Length>0?("_"+this.TypeParameters.Count().ToString()):"");

        public abstract IEnumerable<ITDChild> PublicChildren { get; }

        public abstract IEnumerable<ITDChild> ProtectedChildren { get; }


        public abstract Ret Visit<Arg, Ret>(INamespaceSpecVisitor<Arg, Ret> visitor, Arg arg = default(Arg));

        public abstract Ret Visit<Arg, Ret>(ITDChildVisitor<Arg, Ret> visitor, Arg arg = default(Arg));

        public TDInterface GetInterface(DeclIdentifier name)
        {
            if (name.Arguments.Count() == 0 && TDNamespaces.Any(ns => ns.Name == name.Name.Name))
            {
                throw new TypeCheckException("$1 is already a namespace, not an interface", name);
            }
            if (TDClasses.Any(cls => cls.Name == name.Name.Name&&cls.TypeParameters.Count()==name.Arguments.Count()))
            {
                throw new TypeCheckException("$1 is already a class, not an interface", name);
            }
            TDInterface ret = TDInterfaces.FirstOrDefault(iface => iface.Name == name.Name.Name && iface.TypeParameters.Count() == name.Arguments.Count());
            if(ret==null)
            {
                ret = new TDInterface(Program, name.Name.Name, name.Arguments.Select((arg, i) => new TDTypeArgDecl(arg.Name.Name, /*arg.Variance, */this.OverallTypeParameterCount + i)), this.InjectOptional());
                this.AddInterface(ret);
            }
            return ret;
        }

        public abstract TDLambda CreateLambda(ITypeParametersSpec typeParameters, IParametersSpec parameters, Language.IType returnType);
        public abstract TDStruct CreateStruct();

    }
}
