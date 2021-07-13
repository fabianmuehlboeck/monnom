using System;
using System.Collections.Generic;
using System.Text;
using Nom.Language;
using System.Linq;
using Nom.Language.SpecExtensions;

namespace Nom.Bytecode
{
    public class NamespaceRep : TypeChecker.AParameterized, INamespaceSpec
    {
        public NamespaceRep(string name, IOptional<INamespaceSpec> parentNamespace)
        {
            ParentNamespace = parentNamespace;
            Name = name;
        }
        public IOptional<INamespaceSpec> ParentNamespace { get; }
        //public IOptional<IParameterizedSpecRef<IParameterizedSpec>> ParameterizedParent => ParentNamespace.Bind(ps=>ps.GetAsRef());
        //public int OverallTypeParameterCount => ParameterizedParent.Extract(p => p.Element.OverallTypeParameterCount) + TypeParameters.Count();

        protected override IOptional<IParameterizedSpec> ParamParent => ParentNamespace;
        public string Name { get; }

        private static ITypeParametersSpec emptyPS = new TypeChecker.TypeParametersSpec(new List<ITypeParameterSpec>());
        public override ITypeParametersSpec TypeParameters => emptyPS;

        private List<NamespaceRep> namespaces = new List<NamespaceRep>();
        public IEnumerable<INamespaceSpec> Namespaces => namespaces;
        public void AddNamespace(NamespaceRep ns)
        {
            namespaces.Add(ns);
        }

        private List<InterfaceRep> interfaces = new List<InterfaceRep>();
        public IEnumerable<IInterfaceSpec> Interfaces => interfaces;
        public void AddInterface(InterfaceRep ifc)
        {
            interfaces.Add(ifc);
        }

        private List<ClassRep> classes = new List<ClassRep>();
        public IEnumerable<IClassSpec> Classes => classes;
        public void AddClass(ClassRep cls)
        {
            classes.Add(cls);
        }

        public IEnumerable<INamespaceSpec> Children => Namespaces.Concat<INamespaceSpec>(Interfaces).Concat<INamespaceSpec>(Classes);

        public IEnumerable<INamespaceSpec> PublicChildren => Children;

        public IEnumerable<INamespaceSpec> ProtectedChildren => Children;

        public string FullQualifiedName => ParentNamespace.Extract(pns => pns.FullQualifiedName + ".", "") + Name;

        public Ret Visit<Arg, Ret>(INamespaceSpecVisitor<Arg, Ret> visitor, Arg arg = default(Arg))
        {
            return visitor.VisitNamespaceSpec(this, arg);
        }
        //public IParameterizedSpecRef<IParameterizedSpec> GetAsRef()
        //{
        //    return new ParameterizedSpecRef<IParameterizedSpec>(this, TypeParameters.ToTypeEnvironment());
        //}
    }
}
