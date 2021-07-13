using System;
using System.Collections.Generic;
using System.Text;
using System.Linq;
using Nom.Parser;
using Nom.Language;

namespace Nom.TypeChecker
{
    internal class TDNamespace : ATDContainer, ITDNamespace
    {
        public new readonly IOptional<ITDNamespace> ParentNamespace;
        public TDNamespace(Program program, string name, IOptional<ITDNamespace> parentNamespace) : base(program, name, parentNamespace)
        {
            this.ParentNamespace = parentNamespace;
        }
        private List<TDNamespaceDef> definitions = new List<TDNamespaceDef>();

        public override IEnumerable<ITDChild> PublicChildren => Children;

        public override IEnumerable<ITDChild> ProtectedChildren => Children;

        public void AddDefinition(TDNamespaceDef def)
        {
            definitions.Add(def);
        }


        public override Ret Visit<Arg, Ret>(INamespaceSpecVisitor<Arg, Ret> visitor, Arg arg = default(Arg))
        {
            return visitor.VisitNamespaceSpec(this, arg);
        }

        public TDNamespace AddNamespaceDef(TDNamespaceDef def, DeclQName path)
        {
            if(path.IsEmpty)
            {
                AddDefinition(def);
                return this;
            }
            else
            {
                DeclIdentifier next = path.Current.Elem;
                TDNamespace ns;
                if (HasChild(next))
                {
                    INamespaceSpec child = GetChild(next).Elem;
                    ns = this.TDNamespaces.FirstOrDefault(tdn => tdn == child);
                    if(ns!=null)
                    {
                        return ns.AddNamespaceDef(def, path.Next.Elem);
                    }
                    throw new TypeCheckException(this.ToString().AppendSeparator(".")+next.ToString()+" is not a Namespace!", next);
                }
                ns = new TDNamespace(Program, next.Name.Name, this.InjectOptional());
                this.AddNamespace(ns);
                return ns.AddNamespaceDef(def, path.Next.Elem);
            }
        }

        public override Ret Visit<Arg,Ret>(ITDChildVisitor<Arg,Ret> visitor, Arg arg=default(Arg))
        {
            return visitor.VisitNamespace(this, arg);
        }

        public TDClass GetClass(DeclIdentifier name)
        {
            if (name.Arguments.Count() == 0 && TDNamespaces.Any(ns => ns.Name == name.Name.Name))
            {
                throw new TypeCheckException("$1 is already a namespace, not a class", name);
            }
            if (TDInterfaces.Any(iface => iface.Name == name.Name.Name && iface.TypeParameters.Count() == name.Arguments.Count()))
            {
                throw new TypeCheckException("$1 is already an interface, not a class", name);
            }
            TDClass ret = TDClasses.FirstOrDefault(cls => cls.Name == name.Name.Name && cls.TypeParameters.Count() == name.Arguments.Count());
            if(ret==null)
            {
                ret = new TDClass(Program, name.Name.Name, name.Arguments.Select((arg,i) => new TDTypeArgDecl(arg.Name.Name, this.OverallTypeParameterCount+i)), this.InjectOptional());
                this.AddClass(ret);
            }
            return ret;
        }
        public TDNamespace GetNamespace(DeclQName name)
        {
            if(name.IsEmpty)
            {
                return this;
            }
            return GetNamespace(name.Current.Elem).GetNamespace(name.Next.Elem);
        }
        public TDNamespace GetNamespace(DeclIdentifier name)
        {
            if(name.Arguments.Count()>0)
            {
                throw new TypeCheckException("Namespaces cannot have arguments (@1)", name);
            }
            if (TDClasses.Any(cls => cls.Name == name.Name.Name && cls.TypeParameters.Count() == name.Arguments.Count()))
            {
                throw new TypeCheckException("$1 is already a class, not a namespace", name);
            }
            if (TDInterfaces.Any(iface => iface.Name == name.Name.Name && iface.TypeParameters.Count() == name.Arguments.Count()))
            {
                throw new TypeCheckException("$1 is already an interface, not a namespace", name);
            }
            TDNamespace ret = TDNamespaces.FirstOrDefault(ns => ns.Name == name.Name.Name && ns.TypeParameters.Count() == 0);
            if(ret==null)
            {
                ret = new TDNamespace(Program, name.Name.Name, this.InjectOptional());
                this.AddNamespace(ret);
            }
            return ret;
        }

        public override TDLambda CreateLambda(ITypeParametersSpec typeParameters, IParametersSpec parameters, Language.IType returnType)
        {
            throw new InternalException("It should be impossible (for now) to create a lambda in a namespace");
        }
        public override TDStruct CreateStruct()
        {
            throw new InternalException("It should be impossible (for now) to create a struct in a namespace");
        }

    }
}
