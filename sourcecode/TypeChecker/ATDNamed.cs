using System;
using System.Collections.Generic;
using System.Text;
using System.Linq;
using Nom.Language;

namespace Nom.TypeChecker
{
    internal abstract class ATDNamed<T> : ATDContainer, IInterfaceSpec where T : TDInterfaceDef
    {
        public ATDNamed(Program program, string name, IEnumerable<TDTypeArgDecl> arguments, IOptional<ITDChild> parentNamespace) : base(program, name, parentNamespace)
        {
            arguments = arguments.ToList();
            foreach(var tp in arguments)
            {
                tp.Parent = this;
            }
            this.parameters = new TypeParametersSpec(arguments);
        }

        private List<T> definitions = new List<T>();

        protected IEnumerable<T> Definitions
        {
            get
            {
                return definitions.ToList();
            }
        }

        private bool SingleDefinitionValuesSet = false;
        public void AddDefinition(T def)
        {
            lock (definitions)
            {
                definitions.Add(def);
                if (!SingleDefinitionValuesSet)
                {
                    CheckFirstDefinition(def);
                    SingleDefinitionValuesSet = true;
                }
                CheckDefinition(def);
            }
        }

        protected virtual void CheckDefinition(T def)
        {
            if (def.IsPartial != this.IsPartial)
            {
                CompilerOutput.RegisterException(new TypeCheckException("Not all definitions of $1 specify partial", def.Name));
            }
            if (def.IsShape != this.IsShape || (def.IsMaterial && this.IsShape))
            {
                CompilerOutput.RegisterException(new TypeCheckException("Not all definitions of $1 agree on whether it is a shape or material", def.Name));
            }
            if (def.Visibility.Visibility != this.Visibility)
            {
                CompilerOutput.RegisterException(new TypeCheckException("All definitions of $1 must specify the same visibility", def.Name));
            }
            if (def.TypeArguments.Count() != this.TypeParameters.Count())
            {
                throw new InternalException("Definition added to type with different number of type arguments!");
            }
            if (def.TypeArguments.Zip(this.TypeParameters, (l, r) => l.Name != r.Name).Any(x => x))
            {
                CompilerOutput.RegisterException(new TypeCheckException("Type argument names must match between all declarations of $1", def.Name));
            }
        }
        protected virtual void CheckFirstDefinition(T def)
        {
            this.IsPartial = def.IsPartial;
            this.IsShape = def.IsShape;
            this.Visibility = def.Visibility.Visibility;
        }

        public virtual void ConsolidateDefinition(T def)
        {
            foreach (MethodDeclDef md in def.DeclaredMethods)
            {
                if (Methods.Any(xmd => xmd.Name == md.Name && xmd.TypeParameters.Count() == md.TypeParameters.Count() && !xmd.Parameters.IsDisjoint(md.Parameters)))
                {
                    throw new TypeCheckException("Method $0 has non-disjoint overloadings", md.Identifier);
                }
                methods.Add(md);
            }
        }

        public abstract INamedType Instantiate(ITypeEnvironment<ITypeArgument> args);

        public Visibility Visibility
        {
            get;
            private set;
        } = Visibility.Private;

        public bool IsShape
        {
            get;
            private set;
        } = false;

        public bool IsExpando
        {
            get;
            private set;
        } = false;

        public bool IsPartial
        {
            get;
            private set;
        } = false;

        public IEnumerable<IParamRef<IInterfaceSpec, IType>> Implements
        {
            get
            {
                //TODO: filter duplicates, Prinicpal Instantiations?
                return Definitions.Select(d => d.Implements).Flatten();
            }
        }

        public virtual IEnumerable<IParamRef<IInterfaceSpec, IType>> ImmediateSuperTypes => Implements;

        protected List<MethodDeclDef> methods = new List<MethodDeclDef>();
        public virtual IEnumerable<MethodDeclDef> Methods => methods.ToList();

        protected IEnumerable<IMethodSpec> BestMethods
        {
            get
            {
                List<IMethodSpec> ret = new List<IMethodSpec>();
                foreach (var meth in Methods)
                {
                    ret.Add(meth);
                }
                foreach (var super in this.ImmediateSuperTypes)
                {
                    foreach (var meth in super.Element.Methods)
                    {
                        if (ret.Any(ms => ms.Overrides(meth, super.PArguments)))
                        {
                            continue;
                        }
                        ret.RemoveAll(ms => meth.Overrides(ms, super.PArguments)); //TODO: fix arguments
                        var methRef = new MethodReference(meth, super.PArguments);
                        ret.Add(methRef);
                    }
                }
                return ret;
            }
        }

        IEnumerable<IMethodSpec> IInterfaceSpec.Methods
        {
            get
            {
                return BestMethods;
            }
        }

        private TypeParametersSpec parameters = null;
        public override ITypeParametersSpec TypeParameters
        {
            get
            {
                return parameters;
            }
        }

        IEnumerable<INamespaceSpec> IInterfaceSpec.Children => Children;
        public override IEnumerable<ITDChild> PublicChildren => TDNamespaces.Concat<ITDChild>(TDInterfaces.Where(ifc => ifc.Visibility >= Visibility.Internal).Concat<ITDChild>(TDClasses.Where(ifc => ifc.Visibility >= Visibility.Internal))).ToList();

        public override IEnumerable<ITDChild> ProtectedChildren => TDNamespaces.Concat<ITDChild>(TDInterfaces.Where(ifc => ifc.Visibility >= Visibility.Internal).Concat<ITDChild>(TDClasses.Where(ifc => ifc.Visibility >= Visibility.Internal))).ToList();

 
    }
}
