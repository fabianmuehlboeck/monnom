using System;
using System.Collections.Generic;
using System.Text;
using System.Linq;
using Nom.Parser;
using Nom.Language;

namespace Nom.TypeChecker
{
    internal class TDClass : ATDNamed<TDClassDef>, ITDClass
    {


        public TDClass(Program program, string name, IEnumerable<TDTypeArgDecl> arguments, IOptional<ITDChild> parentNamespace) : base(program, name, arguments, parentNamespace)
        {

        }


        public bool IsFinal
        {
            get;
            private set;
        }

        public bool IsAbstract
        {
            get;
            private set;
        }

        public IOptional<IParamRef<IClassSpec, Language.IType>> SuperClass
        {
            get;
            private set;
        } = new ClassRef<Language.IType>(StdLib.Object.Instance, new TypeEnvironment<Language.IType>()).InjectOptional();

 

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
                ret = new TDClass(Program, name.Name.Name, name.Arguments.Select((arg, i) => new TDTypeArgDecl(arg.Name.Name, this.OverallTypeParameterCount + i)), this.InjectOptional());
                this.AddClass(ret);
            }
            return ret;
        }

        public bool IsInheritanceInitialized => Definitions.All(d => d.IsInheritanceInitialized);

        private List<StaticMethodDef> staticMethods = new List<StaticMethodDef>();
        public IEnumerable<StaticMethodDef> StaticMethods => staticMethods.ToList();
        IEnumerable<IStaticMethodSpec> IClassSpec.StaticMethods => StaticMethods;
        IEnumerable<IStaticMethodDef> IClass.StaticMethods => StaticMethods;

        private List<FieldSpec> fields = new List<FieldSpec>();
        private List<FieldSpec> localFields = new List<FieldSpec>();
        public IEnumerable<IFieldDecl> LocalFields => localFields.ToList();
        public IEnumerable<FieldSpec> Fields => fields.ToList();
        IEnumerable<IFieldSpec> IClassSpec.Fields => Fields;
        IEnumerable<IFieldDecl> IClass.Fields => Fields;


        private List<ConstructorDef> constructors = new List<ConstructorDef>();
        public IEnumerable<ConstructorDef> Constructors => constructors.ToList();
        IEnumerable<IConstructorSpec> IClassSpec.Constructors => Constructors;
        IEnumerable<IConstructorDef> IClass.Constructors => Constructors;

        public IEnumerable<IInstanceSpec> Instances => throw new NotImplementedException();

        private List<TDLambda> lambdas = new List<TDLambda>();
        public IEnumerable<ITDLambda> Lambdas => lambdas.ToList();

        new private List<MethodDef> methods = new List<MethodDef>();
        public override IEnumerable<MethodDeclDef> Methods => methods.ToList();

        IEnumerable<IMethodDef> IClass.Methods => methods.ToList();

        public override IEnumerable<IParamRef<IInterfaceSpec, Language.IType>> ImmediateSuperTypes => SuperClass.AsEnumerable().Concat(base.ImmediateSuperTypes);

        IEnumerable<IStaticFieldDecl> IClass.StaticFields => throw new NotImplementedException();
        public IEnumerable<IStaticFieldSpec> StaticFields  { get { yield break; } } //TODO: implement static fields

        private List<TDStruct> structs = new List<TDStruct>();
        public IEnumerable<ITDStruct> Structs => structs.ToList();

        public override Ret Visit<Arg, Ret>(INamespaceSpecVisitor<Arg, Ret> visitor, Arg arg = default(Arg))
        {
            return visitor.VisitClassSpec(this, arg);
        }

        public override Ret Visit<Arg, Ret>(ITDChildVisitor<Arg, Ret> visitor, Arg arg = default(Arg))
        {
            return visitor.VisitClass(this, arg);
        }

        private bool HadSuperClassDefinition = false;
        protected override void CheckFirstDefinition(TDClassDef def)
        {
            base.CheckFirstDefinition(def);
            this.IsFinal = def.IsFinal;
            this.IsAbstract = def.IsAbstract;
        }
        protected override void CheckDefinition(TDClassDef def)
        {
            base.CheckDefinition(def);
            if(this.IsFinal!=def.IsFinal)
            {
                CompilerOutput.RegisterException(new TypeCheckException("Class $1: final specifications must be consistent over all definitions", def.Name));
            }
            if(this.IsAbstract!=def.IsAbstract)
            {
                CompilerOutput.RegisterException(new TypeCheckException("Class $1: abstract specifications must be consistent over all definitions", def.Name));
            }
        }
        
        public override void ConsolidateDefinition(TDClassDef def)
        {
            base.ConsolidateDefinition(def);
            if (def.SuperClass.HasElem)
            {
                if (HadSuperClassDefinition)
                {
                    if (!def.SuperClass.Elem.Equals(SuperClass.Elem))
                    {
                        CompilerOutput.RegisterException(new TypeCheckException("Class $1: superclass specifications must be consistent", def.Name));
                    }
                }
                else
                {
                    SuperClass = def.SuperClass;
                    HadSuperClassDefinition = true;
                }
            }
            foreach (StaticMethodDef smd in def.StaticMethodDefinitions)
            {
                if (!staticMethods.Contains(smd))
                {
                    //TODO: check method
                    staticMethods.Add(smd);
                }
            }
            foreach(FieldSpec fd in def.FieldDefinitions)
            {
                if(fields.Any(xfd=>xfd.Name==fd.Name)) //TODO: include superclass in field consolidation
                {
                    throw new TypeCheckException("Field $0 already exists", fd.Identifier);
                }
                fields.Add(fd);
                localFields.Add(fd);
            }
            foreach(ConstructorDef cd in def.Constructors)
            {
                if(constructors.Any(xcd=>!xcd.Parameters.IsDisjoint(cd.Parameters)))
                {
                    throw new TypeCheckException("Class $0 has non-disjoint constructors", def.Name);
                }
                constructors.Add(cd);
            }
            foreach(MethodDef md in def.MethodDefinitions)
            {
                if(Methods.Any(xmd => xmd.Name==md.Name && xmd.TypeParameters.Count() == md.TypeParameters.Count() && ! xmd.Parameters.IsDisjoint(md.Parameters)))
                {
                    throw new TypeCheckException("Method $0 has non-disjoint overloadings", md.Identifier);
                }
                methods.Add(md);
            }
        }

        public IParamRef<IClassSpec, Language.IType> MakeClassRef(ITypeEnvironment<Language.IType> env)
        {
            return new ClassRef<Language.IType>(this, env);
        }

        public override TDLambda CreateLambda(ITypeParametersSpec typeParameters, IParametersSpec parameters, Language.IType returnType)
        {
            TDLambda tdl = new TDLambda(typeParameters, parameters, returnType);
            this.lambdas.Add(tdl);
            return tdl;
        }

        public override TDStruct CreateStruct()
        {
            TDStruct tds = new TDStruct();
            this.structs.Add(tds);
            return tds;
        }
        public override INamedType Instantiate(ITypeEnvironment<Language.ITypeArgument> args)
        {
            return new Language.ClassType(this, args);
        }

    }
}
