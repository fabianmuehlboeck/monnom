using System;
using System.Collections.Generic;
using System.Linq;
using Nom.Language;
using ParserType = Nom.Parser.IType;
using ParserTypeArgs = Nom.Parser.IType;
using Nom.TypeChecker.TransformExtensions;
using Nom.Language.SpecExtensions;
using Nom.Parser;
using LType = Nom.Language.IType;
using LTypeArg = Nom.Language.ITypeArgument;

namespace Nom.TypeChecker
{
    public class TypeChecker
    {
        private class TypeCheckInfo<DefT, ContT, RefT>
        {
            public TypeCheckInfo(DefT definition, ContT container, RefT contextReference)
            {
                this.Definition = definition;
                this.Container = container;
                this.ContextReference = contextReference;
            }
            public readonly DefT Definition;
            public readonly ContT Container;
            public readonly RefT ContextReference;
            private List<Parser.Namespace> namespaces = null;
            private List<Parser.ClassDef> classes = null;
            private List<Parser.InterfaceDef> interfaces = null;
            public IEnumerable<Parser.Namespace> Namespaces
            {
                get
                {
                    return namespaces.ToList();
                }
            }
            public IEnumerable<Parser.ClassDef> Classes
            {
                get
                {
                    return classes.ToList();
                }
            }
            public IEnumerable<Parser.InterfaceDef> Interfaces
            {
                get
                {
                    return interfaces.ToList();
                }
            }
            public void InitializeExplorationTargets(IEnumerable<Parser.InterfaceDef> interfaces, IEnumerable<Parser.ClassDef> classes = null, IEnumerable<Parser.Namespace> namespaces = null)
            {
                if (this.interfaces == null)
                {
                    this.interfaces = interfaces.ToList();
                }
                if (this.classes == null)
                {
                    this.classes = classes?.ToList();
                }
                if (this.namespaces == null)
                {
                    this.namespaces = namespaces?.ToList();
                }
            }
            public void RemoveInterfaceTarget(Parser.InterfaceDef target)
            {
                interfaces.Remove(target);
            }
            public void RemoveClassTarget(Parser.ClassDef target)
            {
                classes.Remove(target);
            }
            public void RemoveNamespaceTarget(Parser.Namespace target)
            {
                namespaces.Remove(target);
            }
            public bool ExplorationTargetsCompleted
            {
                get
                {
                    return (namespaces?.Count ?? 0) + (classes?.Count ?? 0) + (interfaces?.Count ?? 0) == 0;
                }
            }
        }
        private class NamespaceTypeCheckInfo : TypeCheckInfo<TDNamespaceDef, TDNamespace, IQName<IArgIdentifier<String, Language.IType>>>
        {
            public NamespaceTypeCheckInfo(TDNamespaceDef definition, TDNamespace container, IQName<IArgIdentifier<String, Language.IType>> contextReference) : base(definition, container, contextReference)
            {
            }

        }
        private class InterfaceTypeCheckInfo : TypeCheckInfo<TDInterfaceDef, TDInterface, IArgIdentifier<String, Language.IType>>
        {
            public InterfaceTypeCheckInfo(TDInterfaceDef definition, TDInterface container, IArgIdentifier<String, Language.IType> contextReference) : base(definition, container, contextReference)
            {
            }
        }
        private class ClassTypeCheckInfo : TypeCheckInfo<TDClassDef, TDClass, IArgIdentifier<String, Language.IType>>
        {
            public ClassTypeCheckInfo(TDClassDef definition, TDClass container, IArgIdentifier<String, Language.IType> contextReference) : base(definition, container, contextReference)
            {
            }
        }
        private class TypeCheckBlock
        {
            private class TypeCheckPass
            {
                protected virtual void HandleNamespace(Parser.Namespace elem, NamespaceTypeCheckInfo tci, TypeCheckBlock tcb, ITypeCheckLookup<Language.IType, Language.ITypeArgument> context) { }
                protected virtual void HandleInterface(Parser.InterfaceDef elem, InterfaceTypeCheckInfo tci, TypeCheckBlock tcb, ITypeCheckLookup<Language.IType, Language.ITypeArgument> context) { }
                protected virtual void HandleClass(Parser.ClassDef elem, ClassTypeCheckInfo tci, TypeCheckBlock tcb, ITypeCheckLookup<Language.IType, Language.ITypeArgument> context) { }

                public void RunPass(TypeCheckBlock tcb)
                {
                    ITypeCheckLookup<Language.IType, Language.ITypeArgument> context = new TypeCheckLookup<Language.IType, Language.ITypeArgument>(tcb.program);
                    foreach (Parser.RefQName u in tcb.file.Usings)
                    {
                        var uref = context.GetChild(u.Transform(id => id.Transform(n => n.Name, t => t.TransformType(context))));
                        context.AddUsing(new LibLookupContext<Language.IType, Language.ITypeArgument>(uref));
                    }
                    CheckNamespace(tcb.file, tcb, context);
                }

                private void CheckNamespace(Parser.Namespace elem, TypeCheckBlock tcb, ITypeCheckLookup<Language.IType, Language.ITypeArgument> context)
                {
                    context = context.Push(tcb.Namespaces[elem].Container, tcb.Namespaces[elem].ContextReference, new List<ILookupContext<Language.IType, Language.ITypeArgument>>());
                    HandleNamespace(elem, tcb.Namespaces[elem], tcb, context);
                    foreach (Parser.Namespace chld in elem.Namespaces)
                    {
                        try
                        {
                            CheckNamespace(chld, tcb, context);
                        }
                        catch (ListableException e)
                        {
                            CompilerOutput.RegisterException(e);
                        }
                    }
                    foreach (Parser.InterfaceDef chld in elem.Interfaces)
                    {
                        try
                        {
                            ChecklInterface(chld, tcb, context);
                        }
                        catch (ListableException e)
                        {
                            CompilerOutput.RegisterException(e);
                        }
                    }
                    foreach (Parser.ClassDef chld in elem.Classes)
                    {
                        try
                        {
                            CheckClass(chld, tcb, context);
                        }
                        catch (ListableException e)
                        {
                            CompilerOutput.RegisterException(e);
                        }
                    }
                }
                private void ChecklInterface(Parser.InterfaceDef elem, TypeCheckBlock tcb, ITypeCheckLookup<LType, LTypeArg> context)
                {
                    InterfaceTypeCheckInfo tci = tcb.Interfaces[elem];
                    context = context.PushVariables(tci.Container.TypeParameters);
                    context = context.Push(tci.Container, tci.ContextReference, new TDInterfaceRef<LType>(tci.Container, context.ContextRoot.PArguments.Push<LType>(tci.Container.TypeParameters, tci.Container.TypeParameters.Select(para => new TypeVariable(para)))).AllImplementedInterfaces().Select(ihs => new ProtectedLookupContext<INamespaceSpec, LType, LTypeArg>(ihs)));

                    HandleInterface(elem, tci, tcb, context);
                    foreach (Parser.InterfaceDef chld in elem.Interfaces)
                    {
                        try
                        {
                            ChecklInterface(chld, tcb, context);
                        }
                        catch (ListableException e)
                        {
                            CompilerOutput.RegisterException(e);
                        }
                    }
                }
                private void CheckClass(Parser.ClassDef elem, TypeCheckBlock tcb, ITypeCheckLookup<LType, LTypeArg> context)
                {
                    ClassTypeCheckInfo tci = tcb.Classes[elem];
                    context = context.PushVariables(tci.Container.TypeParameters);
                    //TODO: add superclasses
                    context = context.Push(tci.Container, tci.ContextReference, new TDClassRef<LType>(tci.Container, context.ContextRoot.PArguments.Push<LType>(tci.Container.TypeParameters, tci.Container.TypeParameters.Select(para => new TypeVariable(para)))).AllImplementedInterfacesClasses().Select(ihs => new ProtectedLookupContext<INamespaceSpec, LType, LTypeArg>(ihs)));

                    HandleClass(elem, tci, tcb, context);
                    foreach (Parser.InterfaceDef chld in elem.Interfaces)
                    {
                        try
                        {
                            ChecklInterface(chld, tcb, context);
                        }
                        catch (ListableException e)
                        {
                            CompilerOutput.RegisterException(e);
                        }
                    }
                    foreach (Parser.ClassDef chld in elem.Classes)
                    {
                        try
                        {
                            CheckClass(chld, tcb, context);
                        }
                        catch (ListableException e)
                        {
                            CompilerOutput.RegisterException(e);
                        }
                    }
                }
            }

            private readonly Parser.CodeFile file;
            private readonly Program program;
            private readonly IEnumerable<ILibrary> libraries;
            public readonly Dictionary<Parser.MethodDef, MethodDef> MethodDefs = new Dictionary<Parser.MethodDef, MethodDef>();
            public readonly Dictionary<Parser.StaticMethodDef, StaticMethodDef> StaticMethodDefs = new Dictionary<Parser.StaticMethodDef, StaticMethodDef>();
            public readonly Dictionary<Parser.FieldDecl, FieldSpec> FieldDefs = new Dictionary<Parser.FieldDecl, FieldSpec>();
            public readonly Dictionary<Parser.Constructor, ConstructorDef> Constructors = new Dictionary<Parser.Constructor, ConstructorDef>();
            public readonly Dictionary<Parser.Namespace, NamespaceTypeCheckInfo> Namespaces = new Dictionary<Parser.Namespace, NamespaceTypeCheckInfo>();
            public readonly Dictionary<Parser.ClassDef, ClassTypeCheckInfo> Classes = new Dictionary<Parser.ClassDef, ClassTypeCheckInfo>();
            public readonly Dictionary<Parser.InterfaceDef, InterfaceTypeCheckInfo> Interfaces = new Dictionary<Parser.InterfaceDef, InterfaceTypeCheckInfo>();
            public TypeCheckBlock(Parser.CodeFile file, IEnumerable<ILibrary> libraries, Program program)
            {
                this.file = file;
                this.program = program;
                this.libraries = libraries;
                this.AddNamespace(file, null);
            }

            private void AddNamespace(Parser.Namespace ns, TDNamespace parent)
            {
                try
                {
                    if (parent == null)
                    {
                        AddNamespaceDef(ns, program.GlobalNS);
                    }
                    else
                    {
                        AddNamespaceDef(ns, parent.GetNamespace(ns.Name));
                    }
                }
                catch (ListableException e)
                {
                    CompilerOutput.RegisterException(e);
                }
            }

            private void AddNamespaceDef(Parser.Namespace ns, TDNamespace container)
            {
                TDNamespaceDef tdnd = new TDNamespaceDef(ns.Name.ElementName, ns.Locs);
                Namespaces.Add(ns, new NamespaceTypeCheckInfo(tdnd, container, ns.Name.Transform(arg => arg.Transform<string, LType>(n => n.Name, t => { throw new TypeCheckException("Namespaces cannot have type arguments (@1)", t.Name); }))));
                container.AddDefinition(tdnd);

                foreach (Parser.Namespace nns in ns.Namespaces)
                {
                    AddNamespace(nns, container);
                }
                foreach (Parser.ClassDef cdef in ns.Classes)
                {
                    AddClass(cdef, container);
                }
                foreach (Parser.InterfaceDef idef in ns.Interfaces)
                {
                    AddInterface(idef, container);
                }
            }

            private void AddClass(Parser.ClassDef cdef, ITDClassContainer parent)
            {
                try
                {
                    AddClassDef(cdef, parent.GetClass(cdef.Name));
                }
                catch (ListableException e)
                {
                    CompilerOutput.RegisterException(e);
                }
            }

            private void AddClassDef(Parser.ClassDef cdef, TDClass container)
            {
                cdef.Annotation = container;
                try
                {
                    List<TDTypeArgDeclDef> typeArgs = new List<TDTypeArgDeclDef>();
                    int startIndex = container.ParentNamespace.Extract(ns => ns.OverallTypeParameterCount, 0);
                    foreach (Parser.TypeArgDecl tad in cdef.TypeArguments)
                    {
                        typeArgs.Add(new TDTypeArgDeclDef(tad.Name, startIndex));
                        startIndex++;
                    }
                    TDClassDef tdcdef = new TDClassDef(cdef.Name.Name, typeArgs, cdef.IsPartial, cdef.IsShape, cdef.IsMaterial, cdef.Visibility, cdef.IsAbstract, cdef.IsFinal);
                    Classes.Add(cdef, new ClassTypeCheckInfo(tdcdef, container, new Parser.AArgIdentifier<string, LType>(container.Name, container.TypeParameters.Select(p => new TypeVariable(p)), cdef.Name.Locs)));
                    container.AddDefinition(tdcdef);
                    foreach (Parser.InterfaceDef iidef in cdef.Interfaces)
                    {
                        AddInterface(iidef, container);
                    }
                    foreach (Parser.ClassDef ccdef in cdef.Classes)
                    {
                        AddClass(ccdef, container);
                    }
                }
                catch (ListableException e)
                {
                    CompilerOutput.RegisterException(e);
                }

            }
            private void AddInterface(Parser.InterfaceDef idef, ITDInterfaceContainer parent)
            {
                try
                {
                    AddInterfaceDef(idef, parent.GetInterface(idef.Name));
                }
                catch (ListableException e)
                {
                    CompilerOutput.RegisterException(e);
                }
            }

            private void AddInterfaceDef(Parser.InterfaceDef idef, TDInterface container)
            {
                idef.Annotation = container;
                try
                {
                    List<TDTypeArgDeclDef> typeArgs = new List<TDTypeArgDeclDef>();
                    int startIndex = container.ParentNamespace.Extract(ns => ns.OverallTypeParameterCount, 0);
                    foreach (Parser.TypeArgDecl tad in idef.TypeArguments)
                    {
                        typeArgs.Add(new TDTypeArgDeclDef(tad.Name, startIndex));
                        startIndex++;
                    }
                    TDInterfaceDef tdidef = new TDInterfaceDef(idef.Name.Name, typeArgs, idef.IsPartial, idef.IsShape, idef.IsMaterial, idef.Visibility);
                    Interfaces.Add(idef, new InterfaceTypeCheckInfo(tdidef, container, new Parser.AArgIdentifier<string, LType>(container.Name, container.TypeParameters.Select(p => new TypeVariable(p)), idef.Name.Locs)));
                    container.AddDefinition(tdidef);
                    foreach (Parser.InterfaceDef iidef in idef.Interfaces)
                    {
                        AddInterface(iidef, container);
                    }
                }
                catch (ListableException e)
                {
                    CompilerOutput.RegisterException(e);
                }
            }

            //TODO: validate absence of circles, shape positions, and type variable constraints
            internal bool InitializeInheritance()
            {
                ITypeCheckLookup<LType, LTypeArg> context = new TypeCheckLookup<LType, LTypeArg>(program);
                foreach (Parser.RefQName u in file.Usings)
                {
                    var uref = context.GetChild(u.Transform(id => id.Transform(n => n.Name, t => t.TransformType(context))));
                    context.AddUsing(new LibLookupContext<LType, LTypeArg>(uref));
                }
                return InitializeInheritanceNamespace(file, context);
            }

            internal bool CheckInheritance()
            {
                new CheckInheritancePass().RunPass(this);
                return true;
            }

            internal bool InitializeInheritanceNamespace(Parser.Namespace ns, ITypeCheckLookup<LType, LTypeArg> context)
            {
                NamespaceTypeCheckInfo ntci = Namespaces[ns];
                ntci.InitializeExplorationTargets(ns.Interfaces, ns.Classes, ns.Namespaces);
                ITypeCheckLookup<LType, LTypeArg> ncontext = context.Push(ntci.Container, ntci.ContextReference, new List<ILookupContext<LType, LTypeArg>>());
                foreach (Parser.Namespace nns in ns.Namespaces)
                {
                    try
                    {
                        if (InitializeInheritanceNamespace(nns, ncontext))
                        {
                            ntci.RemoveNamespaceTarget(nns);
                        }
                    }
                    catch (ListableException e)
                    {
                        CompilerOutput.RegisterException(e);
                    }
                }
                foreach (Parser.InterfaceDef niface in ns.Interfaces)
                {
                    try
                    {
                        if (InitializeInheritanceInterface(niface, ncontext))
                        {
                            ntci.RemoveInterfaceTarget(niface);
                        }
                    }
                    catch (ListableException e)
                    {
                        CompilerOutput.RegisterException(e);
                    }
                }
                foreach (Parser.ClassDef ncls in ns.Classes)
                {
                    try
                    {
                        if (InitializeInheritanceClass(ncls, ncontext))
                        {
                            ntci.RemoveClassTarget(ncls);
                        }
                    }
                    catch (ListableException e)
                    {
                        CompilerOutput.RegisterException(e);
                    }
                }
                return ntci.ExplorationTargetsCompleted;
            }

            internal bool InitializeInheritanceInterface(Parser.InterfaceDef idef, ITypeCheckLookup<LType, LTypeArg> context)
            {
                InterfaceTypeCheckInfo itci = Interfaces[idef];
                TDInterfaceDef tdidef = itci.Definition;
                TDInterface iface = itci.Container;
                ITypeCheckLookup<Language.IType, Language.ITypeArgument> ihContext = context.PushVariables(iface.TypeParameters);
                if (!tdidef.IsInheritanceInitialized)
                {
                    foreach (Parser.InheritanceDecl ihd in idef.Implements)
                    {
                        try
                        {
                            IParamRef<INamespaceSpec, LType> ihspec = context.GetChild(ihd.Parent.Transform(id => id.Transform(n => n.Name, t => t.TransformType(ihContext))));

                            tdidef.AddImplements(ihspec.Visit(new ParamRefVisitor<object, IParamRef<Language.IInterfaceSpec, LType>, LType>(
                                (nss, o) =>
                                {
                                    throw new TypeCheckException("Class type should translate to class or interface type!", ihd.Parent);
                                },
                                (ifct, arg) =>
                                {
                                    ihd.Annotation = ifct;
                                    return ifct;
                                },
                                (clst, arg) =>
                                {
                                    throw new TypeCheckException("Interface can only inherit from other interfaces, not classes. %1 is a class.", ihd.Parent);
                                })));
                        }
                        catch (ListableException e)
                        {
                            CompilerOutput.RegisterException(e);
                        }
                    }
                    tdidef.IsInheritanceInitialized = true;
                }
                if (iface.IsInheritanceInitialized)
                {
                    bool done = true;

                    ITypeCheckLookup<LType, LTypeArg> ncontext = ihContext.Push(itci.Container, itci.ContextReference, new TDInterfaceRef<LType>(iface, context.ContextRoot.PArguments).AllImplementedInterfaces().Select(ihs => new ProtectedLookupContext<INamespaceSpec, LType, LTypeArg>(ihs)));
                    foreach (Parser.InterfaceDef niface in idef.Interfaces)
                    {
                        try
                        {
                            done = done && InitializeInheritanceInterface(niface, ncontext);
                        }
                        catch (ListableException e)
                        {
                            CompilerOutput.RegisterException(e);
                        }
                    }
                    return done;
                }
                return false;
            }
            internal bool InitializeInheritanceClass(Parser.ClassDef cdef, ITypeCheckLookup<LType, LTypeArg> context)
            {
                ClassTypeCheckInfo ctci = Classes[cdef];
                TDClassDef tdcdef = ctci.Definition;
                TDClass cls = ctci.Container;
                ITypeCheckLookup<Language.IType, Language.ITypeArgument> ihContext = context.PushVariables(cls.TypeParameters);
                if (!tdcdef.IsInheritanceInitialized)
                {
                    foreach (Parser.InheritanceDecl ihd in cdef.Implements)
                    {
                        try
                        {
                            IParamRef<INamespaceSpec, LType> ihspec = context.GetChild(ihd.Parent.Transform(id => id.Transform(n => n.Name, t => t.TransformType(ihContext))));
                            tdcdef.AddImplements(ihspec.Visit(new ParamRefVisitor<object, IParamRef<Language.IInterfaceSpec, LType>, LType>(
                                (nss, o) =>
                                {
                                    throw new TypeCheckException("Class type should translate to class or interface type!", ihd.Parent);
                                },
                                (ifct, arg) =>
                                {
                                    ihd.Annotation = ifct;
                                    return ifct;
                                },
                                (clst, arg) =>
                                {
                                    throw new TypeCheckException("Interface can only inherit from other interfaces, not classes. %1 is a class.", ihd.Parent);
                                })));
                        }
                        catch (ListableException e)
                        {
                            CompilerOutput.RegisterException(e);
                        }
                    }
                    cdef.SuperClass.ActionBind(sc =>
                    {
                        try
                        {
                            IParamRef<INamespaceSpec, LType> nsr = context.GetChild(sc.Parent.Transform(arg => arg.Transform(n => n.Name, t => t.TransformType(ihContext))));
                            nsr.Visit(new ParamRefVisitor<object, object, LType>(
                                (nsx, o) =>
                                {
                                    throw new InternalException("Class type should translate to class or interface type!", sc.Locs.AsSourceLocs());
                                },
                                (ifct, arg) =>
                                {
                                    throw new TypeCheckException("Classes can only extend other classes, not interfaces. $1 is an interface. Use the 'implements' keyword for interfaces. ", sc.Parent);
                                },
                                (clst, arg) =>
                                {
                                    sc.Annotation = clst;
                                    tdcdef.SuperClass = clst.InjectOptional();
                                    return null;
                                }));
                        }
                        catch (ListableException e)
                        {
                            CompilerOutput.RegisterException(e);
                        }
                    });
                    tdcdef.IsInheritanceInitialized = true;
                }
                if (cls.IsInheritanceInitialized)
                {
                    bool done = true;
                    ITypeCheckLookup<LType, LTypeArg> ncontext = ihContext.Push(ctci.Container, ctci.ContextReference, new TDClassRef<LType>(cls, context.ContextRoot.PArguments).AllImplementedInterfacesClasses().Concat<IParamRef<IInterfaceSpec, LType>>(cls.SuperClass.AsEnumerable()).Select(ihs => new ProtectedLookupContext<INamespaceSpec, LType, LTypeArg>(ihs)));
                    foreach (Parser.InterfaceDef niface in cdef.Interfaces)
                    {
                        try
                        {
                            done = done && InitializeInheritanceInterface(niface, ncontext);
                        }
                        catch (ListableException e)
                        {
                            CompilerOutput.RegisterException(e);
                        }
                    }
                    foreach (Parser.ClassDef ncls in cdef.Classes)
                    {
                        try
                        {
                            done = done && InitializeInheritanceClass(ncls, ncontext);
                        }
                        catch (ListableException e)
                        {
                            CompilerOutput.RegisterException(e);
                        }
                    }
                    return done;
                }
                return false;
            }

            private class InitializeMembersPass : TypeCheckPass
            {
                private static TypeParametersSpec GenerateParametersSpec(IEnumerable<TypeArgDecl> tads, int countStart, ITypeCheckLookup<Language.IType, Language.ITypeArgument> context)
                {
                    Dictionary<TypeArgDecl, TDTypeArgDeclDef> decls = new Dictionary<TypeArgDecl, TDTypeArgDeclDef>();
                    int i = 0;
                    foreach (var tad in tads)
                    {
                        decls.Add(tad, new TDTypeArgDeclDef(tad.Name, countStart + (i++)));
                    }
                    TypeParametersSpec tps = new TypeParametersSpec(decls.Values);
                    var ihcontext = context.PushVariables(tps);
                    foreach (var tad in tads)
                    {
                        decls[tad].AdjustBounds(tad.UpperBound?.TransformType(ihcontext), tad.LowerBound?.TransformType(ihcontext));
                    }
                    return tps;
                }
                public static InitializeMembersPass Instance = new InitializeMembersPass();
                protected override void HandleInterface(InterfaceDef elem, InterfaceTypeCheckInfo tci, TypeCheckBlock tcb, ITypeCheckLookup<Language.IType, Language.ITypeArgument> context)
                {
                    foreach (Parser.MethodDecl md in elem.Methods)
                    {
                        ParametersSpec ps = new ParametersSpec(md.Args.Select(arg => new ArgumentDeclDef(arg.Name, arg.Type.TransformType(context))));
                        TypeParametersSpec tps = GenerateParametersSpec(md.Name.Arguments, tci.Container.OverallTypeParameterCount, context); 
                        var mdecldef = new MethodDeclDef(md.Name.Name, tps, ps, md.Returns.TransformType(context), md.Visibility.Visibility, tci.Container);
                        tci.Definition.AddMethodDeclDef(mdecldef);
                        md.Annotation = mdecldef;
                    }
                }
                protected override void HandleClass(ClassDef elem, ClassTypeCheckInfo tci, TypeCheckBlock tcb, ITypeCheckLookup<Language.IType, Language.ITypeArgument> context)
                {
                    foreach (Parser.MethodDecl md in elem.Methods)
                    {
                        TypeParametersSpec tps = GenerateParametersSpec(md.Name.Arguments, tci.Container.OverallTypeParameterCount, context); 
                        var ihcontext = context.PushVariables(tps);
                        ParametersSpec ps = new ParametersSpec(md.Args.Select(arg => new ArgumentDeclDef(arg.Name, arg.Type.TransformType(ihcontext))));
                        var mdecldef = new MethodDeclDef(md.Name.Name, tps, ps, md.Returns.TransformType(ihcontext), md.Visibility.Visibility, tci.Container);
                        tci.Definition.AddMethodDeclDef(mdecldef);
                        md.Annotation = mdecldef;
                    }
                    foreach (Parser.MethodDef md in elem.ImplementedMethods)
                    {
                        TypeParametersSpec tps = GenerateParametersSpec(md.Name.Arguments, tci.Container.OverallTypeParameterCount, context);
                        var ihcontext = context.PushVariables(tps);
                        ParametersSpec ps = new ParametersSpec(md.Args.Select(arg => new ArgumentDeclDef(arg.Name, arg.Type.TransformType(ihcontext))));
                        tcb.MethodDefs[md] = new MethodDef(md.Name.Name, tps, ps, md.Returns.TransformType(ihcontext), md.Visibility.Visibility, tci.Container, md.IsFinal, md.IsVirtual, md.IsOverride);
                        tci.Definition.AddMethodDef(tcb.MethodDefs[md]);
                        md.Annotation = tcb.MethodDefs[md];
                    }
                    foreach (Parser.FieldDecl fd in elem.Fields)
                    {
                        FieldSpec fs = new FieldSpec(tci.Container, fd.Ident, fd.Type.TransformType(context), fd.Visibility.Visibility, fd.IsReadOnly, true);
                        tcb.FieldDefs[fd] = fs;
                        tci.Definition.AddFieldDef(fs);
                    }
                    foreach (Parser.StaticMethodDef smd in elem.StaticMethods)
                    {
                        TypeParametersSpec tps = GenerateParametersSpec(smd.Name.Arguments, tci.Container.OverallTypeParameterCount, context); 
                        var ihcontext = context.PushVariables(tps);
                        tps.Zip(smd.Name.Arguments, (tp, arg) => { tp.AdjustBounds(arg.UpperBound.TransformType(ihcontext), arg.LowerBound.TransformType(ihcontext)); return true; });
                        ParametersSpec ps = new ParametersSpec(smd.Args.Select(arg => new ArgumentDeclDef(arg.Name, arg.Type.TransformType(ihcontext))));
                        StaticMethodDef tdsmd = new StaticMethodDef(smd.Name.Name, tps, ps, smd.Returns.TransformType(ihcontext), smd.Visibility.Visibility, tci.Container);
                        tcb.StaticMethodDefs[smd] = tdsmd;
                        tci.Definition.AddStaticMethodDef(tdsmd);
                    }
                    foreach (Parser.Constructor cons in elem.Constructors)
                    {
                        TypeParametersSpec tps = GenerateParametersSpec(cons.TypeArguments, tci.Container.OverallTypeParameterCount, context);
                        var ihcontext = context.PushVariables(tps);
                        ParametersSpec ps = new ParametersSpec(cons.Args.Select(arg => new ArgumentDeclDef(arg.Name, arg.Type.TransformType(ihcontext))));
                        ConstructorDef cd = new ConstructorDef(tci.Container, tps, ps, new Language.ClassType(tci.Container, context.ContextRoot.PArguments.Transform<Language.ITypeArgument>(t => t)), cons.Visibility.Visibility);
                        tcb.Constructors[cons] = cd;
                        tci.Definition.AddConstructorDef(cd);
                    }
                }
            }

            internal void InitializeMembers()
            {
                InitializeMembersPass.Instance.RunPass(this);
            }

            private class ConsolidateDefinitionsPass : TypeCheckPass
            {
                public static ConsolidateDefinitionsPass Instance = new ConsolidateDefinitionsPass();
                private ConsolidateDefinitionsPass() { }

                protected override void HandleInterface(InterfaceDef elem, InterfaceTypeCheckInfo tci, TypeCheckBlock tcb, ITypeCheckLookup<LType, LTypeArg> context)
                {
                    tci.Container.ConsolidateDefinition(tci.Definition);
                }
                protected override void HandleClass(ClassDef elem, ClassTypeCheckInfo tci, TypeCheckBlock tcb, ITypeCheckLookup<LType, LTypeArg> context)
                {
                    tci.Container.ConsolidateDefinition(tci.Definition);
                }
            }

            internal void ConsolidateDefinitions()
            {
                ConsolidateDefinitionsPass.Instance.RunPass(this);
            }

            private class CheckInitializersPass : TypeCheckPass
            {
                public static CheckInitializersPass Instance = new CheckInitializersPass();
                private CheckInitializersPass()
                {

                }

                protected override void HandleClass(ClassDef elem, ClassTypeCheckInfo tci, TypeCheckBlock tcb, ITypeCheckLookup<LType, LTypeArg> context)
                {
                    var cls = tcb.Classes[elem].Container;
                    foreach (Parser.FieldDecl fd in elem.Fields)
                    {
                        if (fd.InitExpr != null)
                        {
                            throw new NotImplementedException(); //TODO: implement default field initializers
                        }
                        else
                        {
                            tcb.FieldDefs[fd].DefaultValueExpr = Optional<IExprTransformResult>.Empty;
                        }
                    }
                    foreach (Parser.StaticFieldDecl sfd in elem.StaticFields)
                    {
                        if (sfd.InitExpr != null)
                        {
                            throw new NotImplementedException(); //TODO: implement default static field initializers
                        }
                        else
                        {
                        }
                    }
                }
            }

            internal void CheckInitializers()
            {
                CheckInitializersPass.Instance.RunPass(this);
            }
            private class CheckInheritancePass : TypeCheckPass
            {
                protected override void HandleClass(ClassDef elem, ClassTypeCheckInfo tci, TypeCheckBlock tcb, ITypeCheckLookup<LType, LTypeArg> context)
                {
                    
                }

                protected override void HandleInterface(InterfaceDef elem, InterfaceTypeCheckInfo tci, TypeCheckBlock tcb, ITypeCheckLookup<LType, LTypeArg> context)
                {
                }
            }
            private class CheckBodiesPass : TypeCheckPass
            {
                public static CheckBodiesPass Instance = new CheckBodiesPass();
                private CheckBodiesPass()
                {

                }
                protected override void HandleClass(ClassDef elem, ClassTypeCheckInfo tci, TypeCheckBlock tcb, ITypeCheckLookup<LType, LTypeArg> context)
                {
                    foreach (Parser.Constructor cdef in elem.Constructors)
                    {
                        try
                        {
                            ConstructorDef cd = tcb.Constructors[cdef];
                            ConstructorTransformEnvironment cte = new ConstructorTransformEnvironment(tci.Container, cd.TypeParameters, cd.Parameters, context);
                            cte.EnableThis();

                            List<IStmtTransformResult> defaultInitializers = new List<IStmtTransformResult>();
                            foreach (FieldSpec fs in tci.Container.Fields)
                            {
                                if (fs.DefaultValueExpr.HasElem)
                                {
                                    defaultInitializers.Add(cte.GetFieldVariable(fs).GenerateWriteAccess(fs.DefaultValueExpr.Elem, cte));
                                }
                            }

                            IStmtTransformResult preResult = cdef.PreSuperStatements.Visit(CodeTransformer.Instance, cte);
                            IEnumerable<IExprTransformResult> superArgs = cdef.SuperCallArgs.Select(sca => sca.Visit(CodeTransformer.Instance, cte));
                            IOptional<ICallableReference> superCallRef = cte.GetSuperConstructor(superArgs.Select(sca => sca.Type));
                            if (!superCallRef.HasElem)
                            {
                                throw new TypeCheckException("Super-class does not have a matching constructor for given arguments (@1)", cdef.Locs);
                            }
                            // Note: the supercall is compiled before the field assignments because then we can enable the this-pointer;
                            // the assignments just use the this-pointer and write the values of existing registers to the fields, so the environment
                            // should stay unaffected by this - the emitted code will do the field assignments before the super call
                            // Currently, the supercall is also thrown away, because really only the arguments are important

                            IExprTransformResult scResult = superCallRef.Elem.GenerateCall(superArgs, cte);

                            List<IInstruction> fieldAssignments = new List<IInstruction>();

                            foreach (FieldSpec fs in tci.Container.Fields)
                            {
                                IVariableReference localVar = cte.GetFieldVariable(fs);
                                if (!localVar.IsInitialized)
                                {
                                    throw new TypeCheckException("All fields declared in class $0 must be declared before the super constructor call @1", elem.Name, cdef.PreSuperStatements.End.SpanTo(cdef.PastSuperStatements.Start));
                                }
                                IExprTransformResult fassetr = localVar.GenerateReadAccess(cte);
                                fieldAssignments.AddRange(fassetr);
                                fieldAssignments.Add(new WriteInstanceFieldInstruction(fs, cte.ThisRegister, fassetr.Register));
                            }

                            IStmtTransformResult postResult = cdef.PastSuperStatements.Visit(CodeTransformer.Instance, cte);
                            cd.PreInstructions = defaultInitializers.Flatten().Concat(preResult).Concat(fieldAssignments);
                            cd.PostInstructions = postResult;
                            cd.SuperConstructorArgs = superArgs.Select(sarg => sarg.Register);
                            cd.RegisterCount = cte.RegisterCount;
                            //TODO : make phi-node numbering less hacky
                            int index = 0;
                            foreach (PhiNode pn in cd.PreInstructions.Where(x => x is PhiNode))
                            {
                                pn.Index = index++;
                            }
                            foreach (PhiNode pn in cd.PostInstructions.Where(x => x is PhiNode))
                            {
                                pn.Index = index++;
                            }
                        }
                        catch (ListableException e)
                        {
                            CompilerOutput.RegisterException(e);
                        }
                    }
                    foreach (Parser.MethodDef mdef in elem.ImplementedMethods)
                    {
                        try
                        {
                            MethodDef md = tcb.MethodDefs[mdef];
                            InstanceMethodTransformEnvironment imte = new InstanceMethodTransformEnvironment(tci.Container, md.TypeParameters, md.Parameters, md.ReturnType, context);
                            IStmtTransformResult str = mdef.Code.Visit(CodeTransformer.Instance, imte);
                            if (!str.AlwaysGoesElsewhere)
                            {
                                if (StdLib.StdLib.VoidType.IsSubtypeOf(md.ReturnType, false))
                                {
                                    md.Instructions = str.Snoc(new ReturnVoidInstruction());
                                }
                                else
                                {
                                    throw new TypeCheckException("Not all paths in $1 return a value", mdef.Name);
                                }
                            }
                            else
                            {
                                md.Instructions = str;
                            }
                            //TODO : make phi-node numbering less hacky
                            int index = 0;
                            foreach (PhiNode pn in md.Instructions.Where(x => x is PhiNode))
                            {
                                pn.Index = index++;
                            }
                            md.RegisterCount = imte.RegisterCount;
                        }
                        catch (ListableException e)
                        {
                            CompilerOutput.RegisterException(e);
                        }
                    }
                    foreach (Parser.StaticMethodDef smd in elem.StaticMethods)
                    {
                        try
                        {
                            StaticMethodDef meth = tcb.StaticMethodDefs[smd];
                            StaticMethodTransformEnvironment smte = new StaticMethodTransformEnvironment(tci.Container, meth.TypeParameters, meth.Parameters, meth.ReturnType, context.PushVariables(meth.TypeParameters));
                            IStmtTransformResult str = smd.Code.Visit(CodeTransformer.Instance, smte);
                            if (!str.AlwaysGoesElsewhere)
                            {
                                if (StdLib.StdLib.VoidType.IsSubtypeOf(meth.ReturnType, false))
                                {
                                    meth.Instructions = str.Snoc(new ReturnVoidInstruction());
                                }
                                else
                                {
                                    throw new TypeCheckException("Not all paths in $1 return a value", smd.Name);
                                }
                            }
                            else
                            {
                                meth.Instructions = str;
                            }
                            //TODO : make phi-node numbering less hacky
                            int index = 0;
                            foreach (PhiNode pn in meth.Instructions.Where(x => x is PhiNode))
                            {
                                pn.Index = index++;
                            }
                            meth.RegisterCount = smte.RegisterCount;
                        }
                        catch (ListableException e)
                        {
                            CompilerOutput.RegisterException(e);
                        }
                    }
                }
            }

            internal void CheckBodies()
            {
                CheckBodiesPass.Instance.RunPass(this);
            }

        }

        public Nom.TypeChecker.Program TypeCheck(Nom.Parser.Program program, IEnumerable<ILibrary> libraries, string programName)
        {
            StdLib.Null.Instance.ToString();
            Program p = new Program(programName, libraries);
            List<TypeCheckBlock> blocks = new List<TypeCheckBlock>();
            foreach (Parser.CodeFile cf in program.CodeFiles)
            {
                blocks.Add(new TypeCheckBlock(cf, libraries, p));
            }
            if (CompilerOutput.HasErrors)
            {
                throw new CompileAbortException();
            }
            bool done = false;
            while (!done)
            {
                done = true;
                foreach (TypeCheckBlock b in blocks)
                {
                    done = b.InitializeInheritance() && done;
                }
            }
            if (CompilerOutput.HasErrors)
            {
                throw new CompileAbortException();
            }

            foreach (TypeCheckBlock b in blocks)
            {
                b.InitializeMembers();
            }
            if (CompilerOutput.HasErrors)
            {
                throw new CompileAbortException();
            }

            foreach (TypeCheckBlock b in blocks)
            {
                b.ConsolidateDefinitions();
            }
            if (CompilerOutput.HasErrors)
            {
                throw new CompileAbortException();
            }

            foreach(TypeCheckBlock b in blocks)
            {
                b.CheckInheritance();
            }
            if (CompilerOutput.HasErrors)
            {
                throw new CompileAbortException();
            }

            foreach (TypeCheckBlock b in blocks)
            {
                b.CheckInitializers();
            }
            if (CompilerOutput.HasErrors)
            {
                throw new CompileAbortException();
            }

            foreach (TypeCheckBlock b in blocks)
            {
                b.CheckBodies();
            }
            if (CompilerOutput.HasErrors)
            {
                throw new CompileAbortException();
            }

            return p;
        }
    }
}
