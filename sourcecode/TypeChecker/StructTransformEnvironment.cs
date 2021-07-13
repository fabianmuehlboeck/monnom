using System;
using System.Collections.Generic;
using System.Text;
using System.Linq;
using Nom.Language;
using Nom.Parser;

namespace Nom.TypeChecker
{
    public class StructTypeParameter : ITypeParameterSpec
    {
        public StructTypeParameter(int index, ITypeParameterSpec original, StructTypeCheckLookup lookup)
        {
            Parent = original.Parent;
            Index = index;
            Original = original;
            Name = original.Name;
        }
        public ITypeParameterSpec Original;
        public IParameterizedSpec Parent { get; set; }

        public int Index { get; }

        public string Name { get; }

        public Language.IType UpperBound { get; private set; }

        public Language.IType LowerBound { get; private set; }

        public void AdjustBounds(Language.IType upperBound = null, Language.IType lowerBound = null)
        {
            UpperBound = upperBound;
            LowerBound = lowerBound;
        }
    }
    public class StructTypeCheckLookup : ITypeCheckLookup<Language.IType, Language.ITypeArgument>
    {
        private class StructTypeParameterBoundVisitor : Language.ITypeVisitor<StructTypeCheckLookup, Language.IType>
        {
            private StructTypeParameterBoundVisitor() { }
            public static StructTypeParameterBoundVisitor Instance { get; } = new StructTypeParameterBoundVisitor();
            public Func<BotType, StructTypeCheckLookup, Language.IType> VisitBotType => (tp, ctx) => tp;

            public Func<Language.ClassType, StructTypeCheckLookup, Language.IType> VisitClassType => (tp, ctx) => new Language.ClassType(tp.Class,
                new TypeEnvironment<Language.ITypeArgument>(tp.Class.AllTypeParameters, tp.Arguments.Select(ta => ta.AsType.Visit(this, ctx))));

            public Func<InterfaceType, StructTypeCheckLookup, Language.IType> VisitInterfaceType => (tp, ctx) => new Language.InterfaceType(tp.Element,
                new TypeEnvironment<Language.ITypeArgument>(tp.Element.AllTypeParameters, tp.Arguments.Select(ta => ta.AsType.Visit(this, ctx))));

            public Func<TopType, StructTypeCheckLookup, Language.IType> VisitTopType => (tp, ctx) => tp;

            public Func<Language.MaybeType, StructTypeCheckLookup, Language.IType> VisitMaybeType => (tp, ctx) => new Language.MaybeType(tp.PotentialType.Visit(this, ctx));

            public Func<Language.ProbablyType, StructTypeCheckLookup, Language.IType> VisitProbablyType => (tp, ctx) => new Language.ProbablyType(tp.PotentialType.Visit(this, ctx));

            public Func<TypeVariable, StructTypeCheckLookup, Language.IType> VisitTypeVariable => (tp, ctx) => new Language.TypeVariable(ctx.RegisterCapturedVariable(tp.ParameterSpec));

            public Func<Language.DynamicType, StructTypeCheckLookup, Language.IType> VisitDynamicType => (tp, ctx) => tp;
        }
        private ITypeCheckLookup<Language.IType, Language.ITypeArgument> Parent;
        private List<StructTypeParameter> capturedTypeParameters = new List<StructTypeParameter>();
        public StructTypeCheckLookup(ITypeCheckLookup<Language.IType, Language.ITypeArgument> parent)
        {
            Parent = parent;
        }

        public IParamRef<INamespaceSpec, Language.IType> ContextRoot => Parent.ContextRoot;
        public IEnumerable<ITypeParameterSpec> StructTypeParameters => capturedTypeParameters.OrderBy(ctp => ctp.Index);
        public IEnumerable<Language.IType> ConstructorTypeArguments => capturedTypeParameters.OrderBy(ctp => ctp.Index).Select(ctp => new Language.TypeVariable(ctp.Original));

        public ITypeParameterSpec RegisterCapturedVariable(ITypeParameterSpec tps)
        {
            if (!capturedTypeParameters.Any(stps => stps.Original == tps))
            {
                StructTypeParameter stp = new StructTypeParameter(capturedTypeParameters.Count, tps, this);
                capturedTypeParameters.Add(stp);
                stp.AdjustBounds(tps.UpperBound?.Visit(StructTypeParameterBoundVisitor.Instance, this), tps.LowerBound?.Visit(StructTypeParameterBoundVisitor.Instance, this));
            }
            return capturedTypeParameters.Single(stps => stps.Original == tps);
        }

        public ITypeEnvironment<Language.IType> TypeEnvironment
        {
            get
            {
                return new TypeEnvironment<Language.IType>(Parent.TypeEnvironment.ToDictionary(kvp =>
                {
                    return RegisterCapturedVariable(kvp.Key);
                }, kvp => kvp.Value.Visit(StructTypeParameterBoundVisitor.Instance, this)));
            }
        }

        ITDChild ITypeCheckLookup<Language.IType, ITypeArgument>.Container => Parent.Container;

        public void AddUsing(ILookupContext<Language.IType, ITypeArgument> usecontext)
        {
            Parent.AddUsing(usecontext);
        }

        public IParamRef<INamespaceSpec, Language.IType> GetChild(IArgIdentifier<string, Language.IType> name)
        {
            return Parent.GetChild(name);
        }

        public IParamRef<INamespaceSpec, ITypeArgument> GetChild(IArgIdentifier<string, ITypeArgument> name)
        {
            return Parent.GetChild(name);
        }

        public IParamRef<INamespaceSpec, Language.IType> GetChild(IQName<IArgIdentifier<string, Language.IType>> name)
        {
            return Parent.GetChild(name);
        }

        public IParamRef<INamespaceSpec, ITypeArgument> GetChild(IQName<IArgIdentifier<string, ITypeArgument>> name)
        {
            return Parent.GetChild(name);
        }

        public IOptional<ITypeParameterSpec> GetTypeVariable(string name)
        {
            var ptv = Parent.GetTypeVariable(name);
            return ptv.Bind(tps => RegisterCapturedVariable(tps));
        }

        public bool HasChild(IQName<IArgIdentifier<string, Language.IType>> name)
        {
            return Parent.HasChild(name);
        }

        public Visibility MembersVisibleAt(INamespaceSpec ns)
        {
            return Parent.MembersVisibleAt(ns);
        }

        public ITypeCheckLookup<Language.IType, ITypeArgument> PushVariables(IEnumerable<ITypeParameterSpec> typeParams)
        {
            return new StructTypeCheckLookupChild(this, Parent, typeParams);
        }

        ITypeCheckLookup<Language.IType, ITypeArgument> ITypeCheckLookup<Language.IType, ITypeArgument>.Push(ITDChild container, IQName<IArgIdentifier<string, Language.IType>> name, IEnumerable<ILookupContext<Language.IType, ITypeArgument>> protecteds)
        {
            return new StructTypeCheckLookupChild(this, Parent.Push(container, name, protecteds), new List<ITypeParameterSpec>());
        }

        ITypeCheckLookup<Language.IType, ITypeArgument> ITypeCheckLookup<Language.IType, ITypeArgument>.Push(ITDChild container, IArgIdentifier<string, Language.IType> name, IEnumerable<ILookupContext<Language.IType, ITypeArgument>> protecteds)
        {
            return new StructTypeCheckLookupChild(this, Parent.Push(container, name, protecteds), new List<ITypeParameterSpec>());
        }

        private class StructTypeCheckLookupChild : ITypeCheckLookup<Language.IType, Language.ITypeArgument>
        {
            private readonly StructTypeCheckLookup Base;
            private readonly ITypeCheckLookup<Language.IType, Language.ITypeArgument> Wrapped;
            private Dictionary<string, ITypeParameterSpec> typeVariables = new Dictionary<string, ITypeParameterSpec>();
            public StructTypeCheckLookupChild(StructTypeCheckLookup baseLookup, ITypeCheckLookup<Language.IType, Language.ITypeArgument> wrapped, IEnumerable<ITypeParameterSpec> tpss)
            {
                Base = baseLookup;
                Wrapped = wrapped;
                foreach (var tps in tpss)
                {
                    typeVariables[tps.Name] = tps;
                }
            }

            public IParamRef<INamespaceSpec, Language.IType> ContextRoot => Wrapped.ContextRoot;

            public ITypeEnvironment<Language.IType> TypeEnvironment => new TypeEnvironment<Language.IType>(Base.TypeEnvironment.Concat(typeVariables.Select(kvp => new KeyValuePair<Language.ITypeParameterSpec, Language.IType>(kvp.Value, new Language.TypeVariable(kvp.Value)))).ToDictionary(kvp => kvp.Key, kvp => kvp.Value));

            ITDChild ITypeCheckLookup<Language.IType, ITypeArgument>.Container => Wrapped.Container;

            public void AddUsing(ILookupContext<Language.IType, ITypeArgument> usecontext)
            {
                Wrapped.AddUsing(usecontext);
            }

            public IParamRef<INamespaceSpec, Language.IType> GetChild(IArgIdentifier<string, Language.IType> name)
            {
                return Wrapped.GetChild(name);
            }

            public IParamRef<INamespaceSpec, ITypeArgument> GetChild(IArgIdentifier<string, ITypeArgument> name)
            {
                return Wrapped.GetChild(name);
            }

            public IParamRef<INamespaceSpec, Language.IType> GetChild(IQName<IArgIdentifier<string, Language.IType>> name)
            {
                return Wrapped.GetChild(name);
            }

            public IParamRef<INamespaceSpec, ITypeArgument> GetChild(IQName<IArgIdentifier<string, ITypeArgument>> name)
            {
                return Wrapped.GetChild(name);
            }

            public IOptional<ITypeParameterSpec> GetTypeVariable(string name)
            {
                if(typeVariables.ContainsKey(name))
                {
                    return typeVariables[name].InjectOptional();
                }
                return Base.GetTypeVariable(name);
            }

            public bool HasChild(IQName<IArgIdentifier<string, Language.IType>> name)
            {
                return Wrapped.HasChild(name);
            }

            public Visibility MembersVisibleAt(INamespaceSpec ns)
            {
                return Wrapped.MembersVisibleAt(ns);
            }

            public ITypeCheckLookup<Language.IType, ITypeArgument> PushVariables(IEnumerable<ITypeParameterSpec> typeParams)
            {
                return new StructTypeCheckLookupChild(Base, Wrapped, typeVariables.Values.Concat(typeParams));
            }

            ITypeCheckLookup<Language.IType, ITypeArgument> ITypeCheckLookup<Language.IType, ITypeArgument>.Push(ITDChild container, IQName<IArgIdentifier<string, Language.IType>> name, IEnumerable<ILookupContext<Language.IType, ITypeArgument>> protecteds)
            {
                return new StructTypeCheckLookupChild(Base, Wrapped.Push(container, name, protecteds), typeVariables.Values);
            }

            ITypeCheckLookup<Language.IType, ITypeArgument> ITypeCheckLookup<Language.IType, ITypeArgument>.Push(ITDChild container, IArgIdentifier<string, Language.IType> name, IEnumerable<ILookupContext<Language.IType, ITypeArgument>> protecteds)
            {
                return new StructTypeCheckLookupChild(Base, Wrapped.Push(container, name, protecteds), typeVariables.Values);
            }
        }
    }
    public class StructTransformEnvironment : ICodeTransformEnvironment
    {
        public StructTransformEnvironment(ICodeTransformEnvironment parentEnv, StructInitializerTransformEnvironment initializerEnvironment, TDStruct tdstruct)
        {
            InitializerEnvironment = initializerEnvironment;
            ParentEnv = parentEnv;
            StructContext = new StructTypeCheckLookup(parentEnv.Context);
            structdef = tdstruct;
        }

        private ICodeTransformEnvironment ParentEnv;
        private StructInitializerTransformEnvironment InitializerEnvironment { get; }
        private TDStruct structdef;
        private Dictionary<String, CaptureField> fields = new Dictionary<String, CaptureField>();
        public IOptional<IVariableReference> this[Identifier ident]
        {
            get
            {
                if (fields.ContainsKey(ident.Name))
                {
                    return new StructFieldReference(ident, fields[ident.Name]).InjectOptional();
                }
                if (structdef.Fields.Any(fd => fd.Name == ident.Name))
                {
                    return new StructFieldReference(ident, structdef.Fields.Single(fd => fd.Name == ident.Name)).InjectOptional();
                }
                //TODO: methods
                var plv = InitializerEnvironment.GetLocalVariable(ident.Name);
                if (plv.HasElem)
                {
                    CaptureField fs = new CaptureField(ident, plv.Elem.Type, structdef, new LocalVariableRef(ident, plv.Elem), InitializerEnvironment);
                    structdef.AddField(fs);
                    fields.Add(ident.Name, fs);
                    return new StructFieldReference(ident, fs).InjectOptional();
                }
                return ParentEnv[ident];
            }
        }

        private Language.IType bestReturnType = BotType.Instance;
        public Language.IType BestReturnType
        {
            get => ExpectedReturnType.Extract(ert => ert.IsSubtypeOf(bestReturnType, false) ? ert : bestReturnType, bestReturnType);
            set { bestReturnType = bestReturnType.Join(value); }
        }

        private class CaptureField : TDStructField
        {
            public CaptureField(Identifier ident, Language.IType type, ITDStruct tdstruct, IVariableReference source, ICodeTransformEnvironment env) : base(ident, type, tdstruct, true, source.GenerateReadAccess(env))
            {
                Source = source;
            }
            public IVariableReference Source { get; }
        }

        private class StructFieldReference : IInstanceFieldReference
        {
            private class CheatRegister : IRegister
            {
                private CheatRegister() { }
                public static CheatRegister Instance { get; } = new CheatRegister();
                public int Index => 0;
            }
            public IRegister SelfRegister { get; }
            public StructFieldReference(Identifier ident, TDStructField sf)
            {
                Identifier = ident;
                StructField = sf;
                SelfRegister = CheatRegister.Instance;
            }
            public TDStructField StructField { get; }
            public IInstanceFieldReference OptimisticVersion => this;

            public Identifier Identifier { get; }

            public bool IsReadOnly => StructField.IsReadOnly;

            public bool IsInitialized => true;

            public Language.IType Type => StructField.Type;

            public IExprTransformResult GenerateReadAccess(ICodeTransformEnvironment env)
            {
                var instruction = new ReadStructFieldInstruction(StructField, env.CreateRegister());
                return new ExprTransformResult(Type, instruction.Register, instruction.Singleton());
            }

            public IStmtTransformResult GenerateWriteAccess(IExprTransformResult value, ICodeTransformEnvironment env)
            {
                if (IsReadOnly)
                {
                    throw new TypeCheckException("Capture variable/field $0 is read-only!", Identifier);
                }
                var instruction = new WriteExpandoFieldInstruction(StructField.Name, SelfRegister, value.Register);
                return new StmtTransformResult(value.Snoc(instruction), env);
            }
        }

        public IOptional<Language.IType> ExpectedReturnType { get; }

        public StructTypeCheckLookup StructContext { get; }
        public ITypeCheckLookup<Language.IType, Language.ITypeArgument> Context => StructContext;

        public IVariableReference AddLocalVariable(Identifier ident, Language.IType type)
        {
            throw new InternalException("this shouldn't happen");
        }

        public IOptional<ICallableReference> AsCallable(IExprTransformResult value, IEnumerable<Language.IType> argTypes)
        {
            var meth = GetMethod(value, new AArgIdentifier<Identifier, Language.IType>(new Identifier("", value.Locs), new List<Language.IType>()), argTypes);
            if (!meth.HasElem)
            {
                throw new InternalException("(Optimistic) callable should have fitting invoke method");
            }
            return meth;
        }

        public IRegister CreateRegister()
        {
            throw new InternalException("shouldn't happen");
        }

        public IOptional<ICallableReference> GetConstructor(IParamRef<IClassSpec, Language.IType> cls, IEnumerable<Language.IType> argTypes)
        {
            return ParentEnv.GetConstructor(cls, argTypes);
        }

        public ILocalVariable GetCurrentVersion(ILocalVariable var)
        {
            throw new InternalException("shouldn't happen");
        }

        public IOptional<IVariableReference> GetFieldAccessVariable(IExprTransformResult value, Identifier ident)
        {
            return ParentEnv.GetFieldAccessVariable(value, ident);
        }

        public IOptional<ICallableReference> GetLocalCallable(IArgIdentifier<Identifier, Language.IType> callableName, IEnumerable<Language.IType> argTypes)
        {
            throw new NotImplementedException(); //TODO: need to make parent refer back to this environment when generating this pointer method access
        }

        public IOptional<ILocalVariable> GetLocalVariable(string name)
        {
            return ParentEnv.GetLocalVariable(name);
        }

        public IOptional<ICallableReference> GetMethod(IExprTransformResult value, IArgIdentifier<Identifier, Language.IType> method, IEnumerable<Language.IType> argTypes)
        {
            return ParentEnv.GetMethod(value, method, argTypes);
        }

        public IOptional<IVariableReference> GetStaticFieldAccessVariable(IParamRef<INamespaceSpec, Language.IType> ns, Identifier ident)
        {
            return ParentEnv.GetStaticFieldAccessVariable(ns, ident);
        }

        public IOptional<ICallableReference> GetStaticMethod(IParamRef<INamespaceSpec, Language.IType> ns, IArgIdentifier<Identifier, Language.IType> method, IEnumerable<Language.IType> argTypes)
        {
            return ParentEnv.GetStaticMethod(ns, method, argTypes);
        }

        public void MergeIn(ICodeTransformEnvironment env)
        {
            throw new InternalException("this shouldn't happen");
        }

        internal IEnumerable<IExprTransformResult> GetClosureAccessors()
        {
            foreach (CaptureField cf in fields.Values)
            {
                yield return cf.Source.GenerateReadAccess(ParentEnv);
            }
        }

        public PhiNode GetBreakPHI(int levels)
        {
            throw new TypeCheckException("No loop to break out of!");
        }

        public PhiNode GetContinuePHI(int levels)
        {
            throw new NotImplementedException("No loop to continue!");
        }

        public ILocalVariable GetTopmostVersion(ILocalVariable var)
        {
            return GetCurrentVersion(var);
        }

        public bool DescendedFrom(ICodeTransformEnvironment ancestor)
        {
            return ancestor == this || ParentEnv.DescendedFrom(ancestor);
        }
        public event Action<ICodeTransformEnvironment, ILocalVariable, IRegister, IRegister> VariableRegisterChanged;
        public void RaiseVariableRegisterChanged(ICodeTransformEnvironment e, ILocalVariable v, IRegister cr, IRegister nr)
        {
            if (VariableRegisterChanged != null)
            {
                VariableRegisterChanged(e, v, cr, nr);
            }
        }
        public void RaiseVariableRegisterChanged(ILocalVariable v, IRegister cr, IRegister nr)
        {
            if (VariableRegisterChanged != null)
            {
                VariableRegisterChanged(this, v, cr, nr);
            }
        }

    }

    public class StructInitializerTransformEnvironment : ICodeTransformEnvironment
    {
        private LocalVariable thisVar;
        private LocalVariable disVar;
        public IEnumerable<IVariableReference> CapturedVariables => captureVars.Select(cv => cv.Source);
        private List<CaptureVariable> captureVars = new List<CaptureVariable>();
        private class CaptureVariable : ILocalVariable
        {
            public readonly IVariableReference Source;
            public CaptureVariable(string name, Language.IType type, IRegister register, IVariableReference source)
            {
                Name = name;
                Type = type;
                this.register = register;
                Source = source;
            }
            public string Name { get; }

            public Language.IType Type { get; }

            public bool IsInitialized => true;

            public bool IsReadOnly => true;

            private IRegister register;

            public IRegister CurrentRegister { get => register; set => throw new InternalException("Shouldn't be able to write to struct initializer capture variables"); }

            public event Action<ILocalVariable, IRegister, IRegister> RegisterChange;

            public bool Matches(ILocalVariable other)
            {
                return this == other;
            }
        }
        private class Register : IRegister
        {
            public Register(int index)
            {
                Index = index;
            }

            public int Index { get; set; }
        }
        private List<Register> registers = new List<Register>();

        public event Action<ICodeTransformEnvironment, ILocalVariable, IRegister, IRegister> VariableRegisterChanged;
        public void RaiseVariableRegisterChanged(ICodeTransformEnvironment e, ILocalVariable v, IRegister cr, IRegister nr)
        {
            if (VariableRegisterChanged != null)
            {
                VariableRegisterChanged(e, v, cr, nr);
            }
        }
        public void RaiseVariableRegisterChanged(ILocalVariable v, IRegister cr, IRegister nr)
        {
            if (VariableRegisterChanged != null)
            {
                VariableRegisterChanged(this, v, cr, nr);
            }
        }

        int lastArgReg = 0;

        public IRegister InsertArgRegister()
        {
            lastArgReg++;
            Register reg = new Register(lastArgReg);
            for (int i = lastArgReg; i < registers.Count; i++)
            {
                registers[i].Index++;
            }
            registers.Insert(lastArgReg, reg);
            return reg;
        }

        public IRegister CreateRegister()
        {
            Register reg = new Register(registers.Count);
            registers.Add(reg);
            return reg;
        }

        public int RegisterCount
        {
            get
            {
                return registers.Count();
            }
        }
        public StructInitializerTransformEnvironment(ICodeTransformEnvironment parent)
        {
            var selfReg = CreateRegister();
            thisVar = new LocalVariable("this", new Language.DynamicType(), selfReg, true);
            disVar = new LocalVariable("dis", new Language.DynamicType(), selfReg, true);
            ParentEnv = parent;
        }
        public IOptional<IVariableReference> this[Identifier ident]
        {
            get
            {
                if (ident.Name == "this")
                {
                    return new LocalVariableRef(ident, thisVar).InjectOptional();
                }
                else if (ident.Name == "dis")
                {
                    return new LocalVariableRef(ident, disVar).InjectOptional();
                }
                else if (captureVars.Any(cv => cv.Name == ident.Name))
                {
                    return new LocalVariableRef(ident, captureVars.Single(cv => cv.Name == ident.Name)).InjectOptional();
                }
                else
                {
                    return ParentEnv[ident].Bind(varref =>
                    {
                        CaptureVariable cv = new CaptureVariable(ident.Name, varref.Type, InsertArgRegister(), varref);
                        captureVars.Add(cv);
                        return new LocalVariableRef(ident, cv);
                    });
                }
            }
        }

        public Language.IType BestReturnType { get => ParentEnv.BestReturnType; set => ParentEnv.BestReturnType = value; }

        public IOptional<Language.IType> ExpectedReturnType => ParentEnv.ExpectedReturnType;

        public ITypeCheckLookup<Language.IType, Language.ITypeArgument> Context => ParentEnv.Context;

        private ICodeTransformEnvironment ParentEnv { get; }


        public IVariableReference AddLocalVariable(Identifier ident, Language.IType type)
        {
            throw new InternalException("Shouldn't be possible in struct initializer");
        }

        public IOptional<ICallableReference> AsCallable(IExprTransformResult value, IEnumerable<Language.IType> argTypes)
        {
            return ParentEnv.AsCallable(value, argTypes);
        }

        public bool DescendedFrom(ICodeTransformEnvironment ancestor)
        {
            return ancestor == this || ParentEnv.DescendedFrom(ancestor);
        }

        public PhiNode GetBreakPHI(int levels)
        {
            throw new InternalException("Shouldn't be possible in struct initializer");
        }

        public IOptional<ICallableReference> GetConstructor(IParamRef<IClassSpec, Language.IType> cls, IEnumerable<Language.IType> argTypes)
        {
            return ParentEnv.GetConstructor(cls, argTypes);
        }

        public PhiNode GetContinuePHI(int levels)
        {
            throw new InternalException("Shouldn't be possible in struct initializer");
        }

        public ILocalVariable GetCurrentVersion(ILocalVariable var)
        {
            if (var.Name == "this") { return thisVar; }
            else if (var.Name == "dis") { return disVar; }
            else { return captureVars.Single(v => v.Name == var.Name); }
        }

        public IOptional<IVariableReference> GetFieldAccessVariable(IExprTransformResult value, Identifier ident)
        {
            return ParentEnv.GetFieldAccessVariable(value, ident);
        }

        public IOptional<ICallableReference> GetLocalCallable(IArgIdentifier<Identifier, Language.IType> callableName, IEnumerable<Language.IType> argTypes)
        {
            return ParentEnv.GetLocalCallable(callableName, argTypes);
        }

        public IOptional<ILocalVariable> GetLocalVariable(string name)
        {
            if (name == "this")
            {
                return thisVar.InjectOptional();
            }
            else if (name == "dis")
            {
                return disVar.InjectOptional();
            }
            else if (captureVars.Any(cv => cv.Name == name))
            {
                return captureVars.Single(cv => cv.Name == name).InjectOptional();
            }
            else
            {
                return ParentEnv.GetLocalVariable(name).Bind(lv =>
                {
                    CaptureVariable cv = new CaptureVariable(name, lv.Type, InsertArgRegister(), new LocalVariableRef(new Identifier(name), lv));
                    captureVars.Add(cv);
                    return cv;
                });
            }
        }

        public IOptional<ICallableReference> GetMethod(IExprTransformResult value, IArgIdentifier<Identifier, Language.IType> method, IEnumerable<Language.IType> argTypes)
        {
            return ParentEnv.GetMethod(value, method, argTypes);
        }

        public IOptional<IVariableReference> GetStaticFieldAccessVariable(IParamRef<INamespaceSpec, Language.IType> ns, Identifier ident)
        {
            return ParentEnv.GetStaticFieldAccessVariable(ns, ident);
        }

        public IOptional<ICallableReference> GetStaticMethod(IParamRef<INamespaceSpec, Language.IType> ns, IArgIdentifier<Identifier, Language.IType> method, IEnumerable<Language.IType> argTypes)
        {
            return ParentEnv.GetStaticMethod(ns, method, argTypes);
        }

        public ILocalVariable GetTopmostVersion(ILocalVariable var)
        {
            throw new InternalException("Shouldn't be possible in struct initializer");
        }

        public void MergeIn(ICodeTransformEnvironment env)
        {
            ParentEnv.MergeIn(env);
        }
    }

    public class StructMethodTransformEnvironment : ICodeTransformEnvironment
    {
        private ICodeTransformEnvironment ParentEnv { get; }

        private Language.IType bestReturnType = BotType.Instance;
        public Language.IType BestReturnType
        {
            get => ExpectedReturnType.Extract(ert => ert.IsSubtypeOf(bestReturnType, false) ? ert : bestReturnType, bestReturnType);
            set { bestReturnType = bestReturnType.Join(value); }
        }

        public IOptional<Language.IType> ExpectedReturnType { get; }

        public ITypeCheckLookup<Language.IType, Language.ITypeArgument> Context { get; }

        public IOptional<IVariableReference> this[Identifier ident]
        {
            get
            {
                if (localVariables.ContainsKey(ident.Name))
                {
                    return new LocalVariableRef(ident, localVariables[ident.Name]).InjectOptional();
                }
                return ParentEnv[ident];
            }
        }

        private Dictionary<string, LocalVariable> localVariables = new Dictionary<string, LocalVariable>();

        public StructMethodTransformEnvironment(ICodeTransformEnvironment parentEnv, StructMethodDef smd)
        {
            ParentEnv = parentEnv;

            ExpectedReturnType = smd.ReturnType.InjectOptional();
            LocalVariable thisVar = new LocalVariable("this", new Language.DynamicType(), CreateRegister(), true);
            localVariables.Add("this", thisVar);
            localVariables.Add("dis", thisVar);
            Context = parentEnv.Context.PushVariables(smd.TypeParameters);

            foreach (IParameterSpec ips in smd.Parameters.Entries)
            {
                AddLocalVariable(ips.Name, ips.Type, CreateRegister(), false);
            }
        }


        protected ILocalVariable AddLocalVariable(string name, Language.IType type, IRegister register, bool isReadOnly)
        {
            if (localVariables.ContainsKey(name))
            {
                throw new InternalException("Duplicate local variable name!");
            }
            LocalVariable lv = new LocalVariable(name, type, register, isReadOnly);
            lv.RegisterChange += RaiseVariableRegisterChanged;
            localVariables.Add(name, lv);
            return lv;
        }
        public IVariableReference AddLocalVariable(Identifier ident, Language.IType type)
        {
            if (ident.Name == "this" || ident.Name == "dis")
            {
                throw new TypeCheckException("$0 is a reserved name!", ident);
            }
            if (localVariables.ContainsKey(ident.Name))
            {
                throw new TypeCheckException("A variable named $0 is already defined in this scope!", ident);
            }
            LocalVariable lv = new LocalVariable(ident.Name, type);
            lv.RegisterChange += RaiseVariableRegisterChanged;
            localVariables.Add(lv.Name, lv);
            return new LocalVariableRef(ident, lv);
        }

        public IOptional<IVariableReference> GetFieldAccessVariable(IExprTransformResult value, Identifier ident)
        {
            return ParentEnv.GetFieldAccessVariable(value, ident);
        }

        public IOptional<IVariableReference> GetStaticFieldAccessVariable(IParamRef<INamespaceSpec, Language.IType> ns, Identifier ident)
        {
            return ParentEnv.GetStaticFieldAccessVariable(ns, ident);
        }

        public IOptional<ICallableReference> GetStaticMethod(IParamRef<INamespaceSpec, Language.IType> ns, IArgIdentifier<Identifier, Language.IType> method, IEnumerable<Language.IType> argTypes)
        {
            return ParentEnv.GetStaticMethod(ns, method, argTypes);
        }

        public IOptional<ICallableReference> GetMethod(IExprTransformResult value, IArgIdentifier<Identifier, Language.IType> method, IEnumerable<Language.IType> argTypes)
        {
            return ParentEnv.GetMethod(value, method, argTypes);
        }

        public IOptional<ICallableReference> AsCallable(IExprTransformResult value, IEnumerable<Language.IType> argTypes)
        {
            return ParentEnv.AsCallable(value, argTypes);
        }

        public IOptional<ICallableReference> GetLocalCallable(IArgIdentifier<Identifier, Language.IType> callableName, IEnumerable<Language.IType> argTypes)
        {
            if (callableName.Arguments.Count() == 0 && localVariables.ContainsKey(callableName.Name.Name))
            {
                return AsCallable(new LocalVariableRef(callableName.Name, localVariables[callableName.Name.Name]).GenerateReadAccess(this), argTypes);
            }
            return ParentEnv.GetLocalCallable(callableName, argTypes);
        }

        public IOptional<ICallableReference> GetConstructor(IParamRef<IClassSpec, Language.IType> cls, IEnumerable<Language.IType> argTypes)
        {
            return ParentEnv.GetConstructor(cls, argTypes);
        }

        private class Register : IRegister
        {
            public Register(int index)
            {
                Index = index;
            }

            public int Index { get; }
        }
        private List<Register> registers = new List<Register>();

        public event Action<ICodeTransformEnvironment, ILocalVariable, IRegister, IRegister> VariableRegisterChanged;
        public void RaiseVariableRegisterChanged(ICodeTransformEnvironment e, ILocalVariable v, IRegister cr, IRegister nr)
        {
            if (VariableRegisterChanged != null)
            {
                VariableRegisterChanged(e, v, cr, nr);
            }
        }
        public void RaiseVariableRegisterChanged(ILocalVariable v, IRegister cr, IRegister nr)
        {
            if (VariableRegisterChanged != null)
            {
                VariableRegisterChanged(this, v, cr, nr);
            }
        }


        public IRegister CreateRegister()
        {
            Register reg = new Register(registers.Count);
            registers.Add(reg);
            return reg;
        }

        public int RegisterCount
        {
            get
            {
                return registers.Count();
            }
        }

        public IOptional<ILocalVariable> GetLocalVariable(string name)
        {
            return localVariables.GetOptional(name);
        }

        public ILocalVariable GetCurrentVersion(ILocalVariable var)
        {
            if (!localVariables.ContainsKey(var.Name))
            {
                throw new InternalException("Local variable does not exist!");
            }
            return localVariables[var.Name];
        }

        public void MergeIn(ICodeTransformEnvironment env)
        {
            foreach (LocalVariable lv in localVariables.Values)
            {
                lv.CurrentRegister = env.GetCurrentVersion(lv).CurrentRegister;
            }
        }
        public PhiNode GetBreakPHI(int levels)
        {
            throw new TypeCheckException("No loop to break out of!");
        }

        public PhiNode GetContinuePHI(int levels)
        {
            throw new NotImplementedException("No loop to continue!");
        }

        public ILocalVariable GetTopmostVersion(ILocalVariable var)
        {
            return GetCurrentVersion(var);
        }

        public bool DescendedFrom(ICodeTransformEnvironment ancestor)
        {
            return ancestor == this || ParentEnv.DescendedFrom(ancestor);
        }
    }

    public class StructAssignmentTransformEnvironment : ICodeTransformEnvironment
    {
        public StructAssignmentTransformEnvironment(ICodeTransformEnvironment parent, IExprTransformResult structConstructor)
        {
            this.ParentEnv = parent;
            this.structConstructor = structConstructor;
            thisVar = new LocalThisVar("this", this);
            disVar = new LocalThisVar("dis", this);
        }

        LocalThisVar thisVar, disVar;

        private class LocalThisVar : ILocalVariable
        {
            private StructAssignmentTransformEnvironment parent;
            public LocalThisVar(string name, StructAssignmentTransformEnvironment parent)
            {
                Name = Name;
                this.parent = parent;
                Type = new Language.DynamicType();
            }
            public string Name { get; }

            public Language.IType Type { get; }

            public bool IsInitialized => true;

            public bool IsReadOnly => true;

            public IRegister CurrentRegister
            {
                get => parent.structConstructor.Register;
                set { throw new TypeCheckException(Name + " is read-only"); }
            }

            public event Action<ILocalVariable, IRegister, IRegister> RegisterChange;

            public bool Matches(ILocalVariable other)
            {
                return other == this;
            }
        }
        private IExprTransformResult structConstructor;
        private ICodeTransformEnvironment ParentEnv { get; }
        public IOptional<IVariableReference> this[Identifier ident]
        {
            get
            {
                if (ident.Name == "this")
                {
                    return new LocalVariableRef(ident, thisVar).InjectOptional();
                }
                else if (ident.Name == "dis")
                {
                    return new LocalVariableRef(ident, disVar).InjectOptional();
                }
                else
                {
                    return ParentEnv[ident];
                }
            }
        }

        public Language.IType BestReturnType
        {
            get => new Nom.Language.DynamicType();

            set { }
        }

        public IOptional<Language.IType> ExpectedReturnType => Optional<Language.IType>.Empty;

        public ITypeCheckLookup<Language.IType, ITypeArgument> Context => ParentEnv.Context;

        public event Action<ICodeTransformEnvironment, ILocalVariable, IRegister, IRegister> VariableRegisterChanged
        {
            add
            {
                ParentEnv.VariableRegisterChanged += value;
            }
            remove
            {
                ParentEnv.VariableRegisterChanged -= value;
            }
        }

        public IVariableReference AddLocalVariable(Identifier ident, Language.IType type)
        {
            throw new TypeCheckException("Cannot add local variable in struct assignment");
        }

        public IOptional<ICallableReference> AsCallable(IExprTransformResult value, IEnumerable<Language.IType> argTypes)
        {
            return ParentEnv.AsCallable(value, argTypes);
        }

        public IRegister CreateRegister()
        {
            return ParentEnv.CreateRegister();
        }

        public bool DescendedFrom(ICodeTransformEnvironment ancestor)
        {
            return ancestor == this || ParentEnv.DescendedFrom(ancestor);
        }

        public PhiNode GetBreakPHI(int levels)
        {
            throw new TypeCheckException("Cannot break within struct assignment");
        }

        public IOptional<ICallableReference> GetConstructor(IParamRef<IClassSpec, Language.IType> cls, IEnumerable<Language.IType> argTypes)
        {
            return ParentEnv.GetConstructor(cls, argTypes);
        }

        public PhiNode GetContinuePHI(int levels)
        {
            throw new TypeCheckException("Cannot continue within struct assignment");
        }

        public ILocalVariable GetCurrentVersion(ILocalVariable var)
        {
            if (var.Name == "this") { return thisVar; }
            else if (var.Name == "dis") { return disVar; }
            else { return ParentEnv.GetCurrentVersion(var); }
        }

        public IOptional<IVariableReference> GetFieldAccessVariable(IExprTransformResult value, Identifier ident)
        {
            return ParentEnv.GetFieldAccessVariable(value, ident);
        }

        public IOptional<ICallableReference> GetLocalCallable(IArgIdentifier<Identifier, Language.IType> callableName, IEnumerable<Language.IType> argTypes)
        {
            return ParentEnv.GetLocalCallable(callableName, argTypes);
        }

        public IOptional<ILocalVariable> GetLocalVariable(string name)
        {
            if (name == "this")
            {
                return thisVar.InjectOptional();
            }
            else if (name == "dis")
            {
                return disVar.InjectOptional();
            }
            else
            {
                return ParentEnv.GetLocalVariable(name);
            }
        }

        public IOptional<ICallableReference> GetMethod(IExprTransformResult value, IArgIdentifier<Identifier, Language.IType> method, IEnumerable<Language.IType> argTypes)
        {
            return ParentEnv.GetMethod(value, method, argTypes);
        }

        public IOptional<IVariableReference> GetStaticFieldAccessVariable(IParamRef<INamespaceSpec, Language.IType> ns, Identifier ident)
        {
            return ParentEnv.GetStaticFieldAccessVariable(ns, ident);
        }

        public IOptional<ICallableReference> GetStaticMethod(IParamRef<INamespaceSpec, Language.IType> ns, IArgIdentifier<Identifier, Language.IType> method, IEnumerable<Language.IType> argTypes)
        {
            return ParentEnv.GetStaticMethod(ns, method, argTypes);
        }

        public ILocalVariable GetTopmostVersion(ILocalVariable var)
        {
            throw new InternalException("Should not be possible in struct assignment");
        }

        public void MergeIn(ICodeTransformEnvironment env)
        {
            ParentEnv.MergeIn(env);
        }
    }
}
