using System;
using System.Collections.Generic;
using System.Text;
using Nom.Language;
using Nom.Parser;
using System.Linq;
using Nom.Language.SpecExtensions;

namespace Nom.TypeChecker
{
    public interface IVariableReference
    {
        Parser.Identifier Identifier { get; }
        bool IsReadOnly { get; }
        bool IsInitialized { get; }
        Language.IType Type { get; }
        IExprTransformResult GenerateReadAccess(ICodeTransformEnvironment env);
        IStmtTransformResult GenerateWriteAccess(IExprTransformResult value, ICodeTransformEnvironment env);

    }

    public interface IFieldReference : IVariableReference
    {

    }

    public interface IInstanceFieldReference : IFieldReference
    {
        IInstanceFieldReference OptimisticVersion { get; }
    }

    internal class CheckedFieldReference : IInstanceFieldReference
    {
        public CheckedFieldReference(Identifier identifier, IFieldSpec fieldSpec, IExprTransformResult receiver)
        {
            Identifier = identifier;
            FieldSpec = fieldSpec;
            Receiver = receiver;
        }
        public IFieldSpec FieldSpec { get; }
        public IExprTransformResult Receiver { get; }

        public Identifier Identifier { get; }

        public virtual bool IsReadOnly => FieldSpec.IsReadonly;

        public virtual bool IsInitialized => true;

        public Language.IType Type => FieldSpec.Type;

        public IInstanceFieldReference OptimisticVersion => throw new NotImplementedException();


        public virtual IExprTransformResult GenerateReadAccess(ICodeTransformEnvironment env)
        {
            ReadInstanceFieldInstruction ri = new ReadInstanceFieldInstruction(FieldSpec, Receiver.Register, env.CreateRegister());
            return new ExprTransformResult(Type, ri.Register, Receiver.Snoc(ri));
        }

        public virtual IStmtTransformResult GenerateWriteAccess(IExprTransformResult value, ICodeTransformEnvironment env)
        {
            value = value.EnsureType(Type, env);
            IRegister writeReg = value.Register;
            List<IInstruction> instructions = new List<IInstruction>(Receiver.Concat(value));
            WriteInstanceFieldInstruction wi = new WriteInstanceFieldInstruction(FieldSpec, Receiver.Register, writeReg);
            instructions.Add(wi);
            return new StmtTransformResult(instructions, env, false, false, false);
        }

    }
    public interface ICallableReference
    {
        ICallReceiverTransformResult GenerateCall(IEnumerable<IExprTransformResult> arguments, ICodeTransformEnvironment env);
    }

    public interface ICallableInstanceReference
    {
        ICallReceiverTransformResult GenerateCall(IExprTransformResult receiver, IEnumerable<IExprTransformResult> arguments, ICodeTransformEnvironment env);
    }

    public class InstantiatedCallableReference : ICallableReference
    {
        public readonly ICallableInstanceReference InstanceReference;
        public readonly IExprTransformResult Receiver;

        public InstantiatedCallableReference(ICallableInstanceReference instanceReference, IExprTransformResult receiver)
        {
            InstanceReference = instanceReference;
            Receiver = receiver;
        }

        public ICallReceiverTransformResult GenerateCall(IEnumerable<IExprTransformResult> arguments, ICodeTransformEnvironment env)
        {
            return InstanceReference.GenerateCall(Receiver, arguments, env);
        }
    }

    public interface IInstanceMethodReference : ICallableInstanceReference
    {
    }

    internal class UniqueStaticMethodReference : ICallableReference
    {
        IStaticMethodSpec StaticMethod { get; }
        ITypeEnvironment<Language.IType> Environment { get; }
        public UniqueStaticMethodReference(IStaticMethodSpec staticMethod, ITypeEnvironment<Language.IType> environment)
        {
            StaticMethod = staticMethod;
            Environment = environment;
        }
        public ICallReceiverTransformResult GenerateCall(IEnumerable<IExprTransformResult> arguments, ICodeTransformEnvironment env)
        {
            List<IInstruction> instructions = new List<IInstruction>();
            List<IRegister> argRegs = new List<IRegister>();
            foreach ((IExprTransformResult, Language.IType) argpair in arguments.Zip(StaticMethod.Parameters.Entries.Select(t => ((ISubstitutable<Language.IType>)t.Type).Substitute(Environment)), (etr, t) => (etr, t)))
            {
                IExprTransformResult actualArg = argpair.Item1.EnsureType(argpair.Item2, env);
                instructions.AddRange(actualArg);
                argRegs.Add(actualArg.Register);
            }
            var call = new CallStaticMethodCheckedInstruction(StaticMethod.GetAsTypedRef().Substitute(Environment.Transform<ITypeArgument>(t=>t)), argRegs, env.CreateRegister());
            instructions.Add(call);
            return new CallReceiverTransformResult(((ISubstitutable<Language.IType>)StaticMethod.ReturnType).Substitute<Language.IType>(Environment), call.Register, instructions);
        }
    }
    internal class UniqueConstructorCall : ICallableReference
    {
        public UniqueConstructorCall(IParameterizedSpecRef<IConstructorSpec> constructor)
        {
            Constructor = constructor;
        }
        public IParameterizedSpecRef<IConstructorSpec> Constructor { get; }
        public ICallReceiverTransformResult GenerateCall(IEnumerable<IExprTransformResult> arguments, ICodeTransformEnvironment env)
        {
            List<IInstruction> instructions = new List<IInstruction>(); 
            if (arguments.Count() != Constructor.Element.Parameters.Entries.Count())
            {
                throw new InternalException("Generating call with wrong number of arguments");
            }
            List<IRegister> argregs = new List<IRegister>();
            foreach (var pairing in arguments.Zip(Constructor.Element.Parameters.Entries, (arg, entry) => (arg, entry).ToTuple()))
            {
                Language.IType targetType = ((ISubstitutable<Language.IType>)pairing.Item2.Type).Substitute(Constructor.Substitutions);
                IExprTransformResult actualArgument = pairing.Item1.EnsureType(targetType, env);
                instructions.AddRange(actualArgument);
                argregs.Add(actualArgument.Register);
            }
            var instr = new CallConstructorCheckedInstruction(Constructor, argregs, env.CreateRegister());
            instructions.Add(instr);
            return new CallReceiverTransformResult(new Language.ClassType(Constructor.Element.Container, Constructor.Substitutions), instr.Register, instructions);
        }
    }

    public interface ICodeTransformEnvironment
    {
        /// <summary>
        /// Setting this means joining the current value of this field with the newly "assigned" value
        /// </summary>
        Language.IType BestReturnType { get; set; }
        IOptional<Language.IType> ExpectedReturnType { get; }
        IVariableReference AddLocalVariable(Parser.Identifier ident, Language.IType type);
        IOptional<IVariableReference> this[Parser.Identifier ident] { get; }
        ITypeCheckLookup<Language.IType, Language.ITypeArgument> Context { get; }
        IOptional<IVariableReference> GetFieldAccessVariable(IExprTransformResult value, Parser.Identifier ident);
        IOptional<IVariableReference> GetStaticFieldAccessVariable(IParamRef<INamespaceSpec, Language.IType> ns, Parser.Identifier ident);
        IOptional<ICallableReference> GetStaticMethod(IParamRef<INamespaceSpec, Language.IType> ns, IArgIdentifier<Parser.Identifier, Language.IType> method, IEnumerable<Language.IType> argTypes);
        IOptional<ICallableReference> GetMethod(IExprTransformResult value, IArgIdentifier<Parser.Identifier, Language.IType> method, IEnumerable<Language.IType> argTypes);
        IOptional<ICallableReference> AsCallable(IExprTransformResult value, IEnumerable<Language.IType> argTypes);
        IOptional<ICallableReference> GetLocalCallable(IArgIdentifier<Parser.Identifier, Language.IType> callableName, IEnumerable<Language.IType> argTypes);
        IOptional<ICallableReference> GetConstructor(IParamRef<IClassSpec, Language.IType> cls, IEnumerable<Language.IType> argTypes);
        IRegister CreateRegister();

        IOptional<ILocalVariable> GetLocalVariable(string name);
        ILocalVariable GetCurrentVersion(ILocalVariable var);
        ILocalVariable GetTopmostVersion(ILocalVariable var);
        bool DescendedFrom(ICodeTransformEnvironment ancestor);
        void MergeIn(ICodeTransformEnvironment env);

        PhiNode GetBreakPHI(int levels);
        PhiNode GetContinuePHI(int levels);

        event Action<ICodeTransformEnvironment, ILocalVariable, IRegister, IRegister> VariableRegisterChanged;
    }


    public interface ILocalVariable
    {
        String Name { get; }
        Language.IType Type { get; }
        bool IsInitialized { get; }
        bool IsReadOnly { get; }
        IRegister CurrentRegister { get; set; }
        bool Matches(ILocalVariable other);
        event Action<ILocalVariable, IRegister, IRegister> RegisterChange;
    }

    internal class LocalVariable : ILocalVariable
    {
        private IRegister currentRegister;

        public LocalVariable(string name, Language.IType type, IRegister currentRegister = null, bool isReadOnly = false)
        {
            Name = name;
            Type = type;
            CurrentRegister = currentRegister;
            IsReadOnly = isReadOnly;
        }

        public string Name { get; }

        public Language.IType Type { get; }

        public bool IsInitialized => CurrentRegister != null;

        public bool IsReadOnly { get; }

        public IRegister CurrentRegister
        {
            get => currentRegister; set
            {
                if (RegisterChange != null)
                {
                    RegisterChange(this, currentRegister, value);
                }
                currentRegister = value;
            }
        }

        public event Action<ILocalVariable, IRegister, IRegister> RegisterChange;

        public bool Matches(ILocalVariable other)
        {
            return this == other;
        }
    }

    internal class LocalVariableRef : IVariableReference
    {
        public ILocalVariable Variable { get; }
        public LocalVariableRef(Identifier ident, ILocalVariable var)
        {
            Identifier = ident;
            Variable = var;
        }
        public Identifier Identifier { get; }

        public bool IsReadOnly => Variable.IsReadOnly;

        public bool IsInitialized => Variable.IsInitialized;

        public Language.IType Type => Variable.Type;

        public IExprTransformResult GenerateReadAccess(ICodeTransformEnvironment env)
        {
            if (!Variable.IsInitialized)
            {
                throw new TypeCheckException("Variable $0 is not guaranteed to have been initialized!", Identifier);
            }
            return new ExprTransformResult(Variable.Type, Variable.CurrentRegister, new List<IInstruction>());
        }

        public IStmtTransformResult GenerateWriteAccess(IExprTransformResult value, ICodeTransformEnvironment env)
        {
            if (Variable.IsReadOnly)
            {
                throw new TypeCheckException("Cannot write to read-only variable $0!", Identifier);
            }
            if (!value.Type.IsSubtypeOf(Variable.Type, true))
            {
                throw new TypeCheckException("Result of assignment expression has wrong type for variable $0 even optimistically!", Identifier);
            }
            IExprTransformResult actualValue = value.EnsureType(Variable.Type, env);
            List<IInstruction> instructions = actualValue.ToList();
            Variable.CurrentRegister = actualValue.Register;
            return new StmtTransformResult(instructions, env, false, false, false);
        }

    }

    /// <summary>
    /// Abstract definition of local variable scope
    /// </summary>
    public abstract class ACodeTransformEnvironment : ICodeTransformEnvironment
    {
        private Dictionary<string, ILocalVariable> localVariables = new Dictionary<string, ILocalVariable>();

        protected ILocalVariable AddLocalVariable(string name, Language.IType type, IRegister register, bool isReadOnly)
        {
            if (localVariables.ContainsKey(name))
            {
                throw new InternalException("Duplicate local variable name!");
            }
            ILocalVariable lv = new LocalVariable(name, type, register, isReadOnly);
            lv.RegisterChange += RaiseVariableRegisterChanged;
            localVariables.Add(name, lv);
            return lv;
        }

        public virtual IOptional<IVariableReference> this[Identifier ident]
        {
            get
            {
                return GetLocalVariable(ident.Name).Bind(lv => new LocalVariableRef(ident, lv));
            }
        }

        public virtual IVariableReference AddLocalVariable(Identifier ident, Language.IType type)
        {
            if (ident.Name == "this" || ident.Name == "dis")
            {
                throw new TypeCheckException("$0 is a reserved variable name", ident);
            }
            if (localVariables.ContainsKey(ident.Name))
            {
                throw new TypeCheckException("A variable named $0 has already been declared in the same scope!", ident);
            }
            var lv = new LocalVariable(ident.Name, type);
            lv.RegisterChange += RaiseVariableRegisterChanged;
            localVariables.Add(ident.Name, lv);
            return this[ident].Elem;
        }

        protected void RaiseVariableRegisterChanged(ILocalVariable v, IRegister cr, IRegister nr)
        {
            if (this.VariableRegisterChanged != null)
            {
                this.VariableRegisterChanged(this, v, cr, nr);
            }
        }
        protected void RaiseVariableRegisterChanged(ICodeTransformEnvironment env, ILocalVariable v, IRegister cr, IRegister nr)
        {
            if (this.VariableRegisterChanged != null)
            {
                this.VariableRegisterChanged(env, v, cr, nr);
            }
        }

        public virtual void MergeIn(ICodeTransformEnvironment env)
        {
            foreach (ILocalVariable lvar in this.localVariables.Values)
            {
                var cv = env.GetCurrentVersion(lvar);
                //Assumes that Initialization cannot be undone, i.e. if any of the branches in uninitialized, so is the parent, so we don't need to change anything about that
                lvar.CurrentRegister = cv.CurrentRegister;
            }
        }

        public virtual ILocalVariable GetCurrentVersion(ILocalVariable var)
        {
            var res = TryGetCurrentVersion(var);
            if (res.HasElem)
            {
                return res.Elem;
            }
            throw new InternalException("Did not find current version of variable");
        }
        public virtual ILocalVariable GetTopmostVersion(ILocalVariable var)
        {
            var res = TryGetTopmostVersion(var);
            if (res.HasElem)
            {
                return res.Elem;
            }
            throw new InternalException("Did not find topmost version of variable");
        }
        protected virtual IOptional<ILocalVariable> TryGetCurrentVersion(ILocalVariable var)
        {
            if (localVariables.Values.Any(v => v.Matches(var)))
            {
                return localVariables.Values.Single(v => v.Matches(var)).InjectOptional();
            }
            return Optional<ILocalVariable>.Empty;
        }
        protected virtual IOptional<ILocalVariable> TryGetTopmostVersion(ILocalVariable var)
        {
            if (localVariables.Values.Any(v => v.Matches(var) || var.Matches(v)))
            {
                return localVariables.Values.Single(v => v.Matches(var) || var.Matches(v)).InjectOptional();
            }
            return Optional<ILocalVariable>.Empty;
        }

        public virtual IOptional<ILocalVariable> GetLocalVariable(string name)
        {
            return GetScopedLocalVariable(name).Coalesce(() => GetOuterLocalVariable(name));
        }

        protected virtual IOptional<ILocalVariable> GetScopedLocalVariable(string name)
        {
            if (localVariables.ContainsKey(name))
            {
                return localVariables[name].InjectOptional();
            }
            return Optional<ILocalVariable>.Empty;
        }

        protected virtual IOptional<ILocalVariable> GetOuterLocalVariable(string name)
        {
            return Optional<ILocalVariable>.Empty;
        }
        public abstract IOptional<Language.IType> ExpectedReturnType { get; }
        public abstract ITypeCheckLookup<Language.IType, Language.ITypeArgument> Context { get; }


        private Language.IType bestReturnType = Language.BotType.Instance;

        public event Action<ICodeTransformEnvironment, ILocalVariable, IRegister, IRegister> VariableRegisterChanged;

        public virtual Language.IType BestReturnType
        {
            get => bestReturnType;
            set
            {
                bestReturnType = bestReturnType.Join(value);
            }
        }

        public abstract IOptional<ICallableReference> AsCallable(IExprTransformResult value, IEnumerable<Language.IType> argTypes);
        public abstract IRegister CreateRegister();
        public abstract IOptional<ICallableReference> GetConstructor(IParamRef<IClassSpec, Language.IType> cls, IEnumerable<Language.IType> argTypes);
        public abstract IOptional<IVariableReference> GetFieldAccessVariable(IExprTransformResult value, Identifier ident);
        public abstract IOptional<ICallableReference> GetLocalCallable(IArgIdentifier<Identifier, Language.IType> callableName, IEnumerable<Language.IType> argTypes);
        public abstract IOptional<ICallableReference> GetMethod(IExprTransformResult value, IArgIdentifier<Identifier, Language.IType> method, IEnumerable<Language.IType> argTypes);
        public abstract IOptional<IVariableReference> GetStaticFieldAccessVariable(IParamRef<INamespaceSpec, Language.IType> ns, Identifier ident);
        public abstract IOptional<ICallableReference> GetStaticMethod(IParamRef<INamespaceSpec, Language.IType> ns, IArgIdentifier<Identifier, Language.IType> method, IEnumerable<Language.IType> argTypes);
        public abstract PhiNode GetBreakPHI(int levels);
        public abstract PhiNode GetContinuePHI(int levels);
        public abstract bool DescendedFrom(ICodeTransformEnvironment ancestor);
    }

    /// <summary>
    /// Abstract top-level code transform environment
    /// </summary>
    internal abstract class ATopLevelCodeTransformEnvironment : ACodeTransformEnvironment
    {

        private List<IRegister> registers = new List<IRegister>() { };

        public int RegisterCount => registers.Count;

        public override IOptional<Language.IType> ExpectedReturnType { get; }
        public override ITypeCheckLookup<Language.IType, Language.ITypeArgument> Context { get; }


        public ATopLevelCodeTransformEnvironment(ITypeCheckLookup<Language.IType, Language.ITypeArgument> context, IOptional<Language.IType> expectedReturnType)
        {
            Context = context;
            ExpectedReturnType = expectedReturnType;
        }

        /// <summary>
        /// Needs to be called at end of constructor to be able to prepend "this" in register 0
        /// </summary>
        /// <param name="parameters"></param>
        protected void InitializeParameters(IParametersSpec parameters)
        {
            foreach (IParameterSpec ps in parameters.Entries)
            {
                AddLocalVariable(ps.Name, ps.Type, CreateRegister(), false);
            }
        }




        public override IOptional<ICallableReference> AsCallable(IExprTransformResult value, IEnumerable<Language.IType> argTypes)
        {
            var meth = GetMethod(value, new AArgIdentifier<Identifier, Language.IType>(new Identifier("", value.Locs), new List<Language.IType>()), argTypes);
            if (!meth.HasElem)
            {
                throw new InternalException("(Optimistic) callable should have fitting invoke method");
            }
            return meth;
        }

        public override IOptional<ICallableReference> GetConstructor(IParamRef<IClassSpec, Language.IType> cls, IEnumerable<Language.IType> argTypes)
        {
            List<IConstructorSpec> constructors = cls.Element.Constructors.Where(c => c.Visibility >= this.Context.MembersVisibleAt(cls.Element) && c.Parameters.WouldAccept(cls.PArguments, argTypes)).ToList();
            if (constructors.Count == 0)
            {
                return Optional<ICallableReference>.Empty;
            }
            if (constructors.Count == 1)
            {
                return new UniqueConstructorCall(new ParameterizedSpecRef<IConstructorSpec>(constructors.Single(), cls.PArguments.Transform<Language.ITypeArgument>(t=>t))).InjectOptional();
            }
            return new OverloadedConstructorCall(constructors, cls.PArguments).InjectOptional();
        }
        public override bool DescendedFrom(ICodeTransformEnvironment ancestor)
        {
            return this == ancestor;
        }

        protected class OverloadedConstructorCall : ICallableReference
        {
            public OverloadedConstructorCall(IEnumerable<IConstructorSpec> constructors, ITypeEnvironment<Language.IType> env)
            {
                Constructors = constructors;
                Environment = env;
            }
            public IEnumerable<IConstructorSpec> Constructors { get; }
            public ITypeEnvironment<Language.IType> Environment { get; }
            public ICallReceiverTransformResult GenerateCall(IEnumerable<IExprTransformResult> arguments, ICodeTransformEnvironment env)
            {
                throw new NotImplementedException();
            }
        }

        public override IOptional<IVariableReference> GetFieldAccessVariable(IExprTransformResult value, Identifier ident)
        {
            return value.Type.Visit(FieldLookupVisitor.Instance, (ident, value, this));
        }

        /// <summary>
        /// Here's why expando-access is fine in constructors: there's no access to this or dis, and therefore there is no
        /// way to expando-access things that are actually fields in the current class.
        /// </summary>
        internal class ExpandoFieldReference : IInstanceFieldReference
        {
            public ExpandoFieldReference(Identifier identifier, IExprTransformResult receiver)
            {
                Identifier = identifier;
                Receiver = receiver;
            }
            public IExprTransformResult Receiver { get; }
            public Identifier Identifier { get; }

            public bool IsReadOnly => false;

            public bool IsInitialized => true;

            public Language.IType Type { get; } = new Language.DynamicType();

            public IInstanceFieldReference OptimisticVersion => this;

            public IExprTransformResult GenerateReadAccess(ICodeTransformEnvironment env)
            {
                ReadExpandoFieldInstruction refi = new ReadExpandoFieldInstruction(Identifier.Name, Receiver.Register, env.CreateRegister());
                return new ExprTransformResult(Type, refi.Register, Receiver.Snoc(refi), Identifier.Locs);
            }

            public IStmtTransformResult GenerateWriteAccess(IExprTransformResult value, ICodeTransformEnvironment env)
            {
                WriteExpandoFieldInstruction wefi = new WriteExpandoFieldInstruction(Identifier.Name, Receiver.Register, value.Register);
                return new StmtTransformResult(Receiver.Concat(value.Snoc(wefi)), env);
            }
        }

        internal class CheckedThisFieldReference : IInstanceFieldReference
        {
            public CheckedThisFieldReference(Identifier identifier, IFieldSpec fieldSpec)
            {
                Identifier = identifier;
                FieldSpec = fieldSpec;
            }

            public IFieldSpec FieldSpec { get; }
            public IInstanceFieldReference OptimisticVersion => throw new NotImplementedException();

            public Identifier Identifier { get; }

            public bool IsReadOnly => FieldSpec.IsReadonly;

            public bool IsInitialized => true;

            public Language.IType Type => FieldSpec.Type;

            public IExprTransformResult GenerateReadAccess(ICodeTransformEnvironment env)
            {
                IExprTransformResult thisexpr = env[new Identifier("this", Identifier.Locs)].Elem.GenerateReadAccess(env);
                ReadInstanceFieldInstruction ri = new ReadInstanceFieldInstruction(FieldSpec, thisexpr.Register, env.CreateRegister());
                return new ExprTransformResult(Type, ri.Register, thisexpr.Snoc(ri));
            }

            public IStmtTransformResult GenerateWriteAccess(IExprTransformResult value, ICodeTransformEnvironment env)
            {
                IExprTransformResult thisexpr = env[new Identifier("this", Identifier.Locs)].Elem.GenerateReadAccess(env);
                IExprTransformResult actualValue = value.EnsureType(Type, env);
                IRegister writeReg = actualValue.Register;
                List<IInstruction> instructions = new List<IInstruction>(thisexpr.Concat(actualValue));
                WriteInstanceFieldInstruction wi = new WriteInstanceFieldInstruction(FieldSpec, thisexpr.Register, writeReg);
                instructions.Add(wi);
                return new StmtTransformResult(instructions, env, false, false, false);
            }
        }


        internal class StaticFieldReference : IVariableReference
        {
            public readonly IStaticFieldSpec Field;
            public StaticFieldReference(Identifier ident, IStaticFieldSpec sfs)
            {
                Field = sfs;
                Identifier = ident;
            }
            public Identifier Identifier { get; }

            public bool IsReadOnly => Field.IsReadonly;

            public bool IsInitialized => true;

            public Language.IType Type => Field.Type;


            public IExprTransformResult GenerateReadAccess(ICodeTransformEnvironment env)
            {
                throw new NotImplementedException();
            }

            public IStmtTransformResult GenerateWriteAccess(IExprTransformResult value, ICodeTransformEnvironment env)
            {
                throw new NotImplementedException();
            }
        }

        private class FieldLookupVisitor : Language.ITypeVisitor<(Identifier, IExprTransformResult, ICodeTransformEnvironment), IOptional<IInstanceFieldReference>>
        {

            public static FieldLookupVisitor Instance = new FieldLookupVisitor();
            private FieldLookupVisitor() { }
            public Func<BotType, (Identifier, IExprTransformResult, ICodeTransformEnvironment), IOptional<IInstanceFieldReference>> VisitBotType => (type, args) =>
            {
                return new ExpandoFieldReference(args.Item1, args.Item2).InjectOptional();
            };

            public Func<Language.ClassType, (Identifier, IExprTransformResult, ICodeTransformEnvironment), IOptional<IInstanceFieldReference>> VisitClassType => (type, args) =>
            {
                IOptional<IFieldSpec> field = type.Class.Fields.FirstOrDefault(fs => fs.Visibility >= args.Item3.Context.MembersVisibleAt(type.Class) && fs.Name == args.Item1.Name).InjectOptional();
                if (!field.HasElem)
                {
                    if (type.Class.IsExpando)
                    {
                        return new ExpandoFieldReference(args.Item1, args.Item2).InjectOptional();
                    }
                    return Optional<IInstanceFieldReference>.Empty;
                }
                return new CheckedFieldReference(args.Item1, field.Elem, args.Item2).InjectOptional();
            };

            public Func<InterfaceType, (Identifier, IExprTransformResult, ICodeTransformEnvironment), IOptional<IInstanceFieldReference>> VisitInterfaceType => (type, args) => Optional<IInstanceFieldReference>.Empty;

            public Func<TopType, (Identifier, IExprTransformResult, ICodeTransformEnvironment), IOptional<IInstanceFieldReference>> VisitTopType => (type, args) => Optional<IInstanceFieldReference>.Empty;


            public Func<TypeVariable, (Identifier, IExprTransformResult, ICodeTransformEnvironment), IOptional<IInstanceFieldReference>> VisitTypeVariable => (type, args) => type.ParameterSpec.UpperBound.Visit(this, args);

            public Func<Language.DynamicType, (Identifier, IExprTransformResult, ICodeTransformEnvironment), IOptional<IInstanceFieldReference>> VisitDynamicType => (type, args) =>
            {
                return new ExpandoFieldReference(args.Item1, args.Item2).InjectOptional();
            };

            public Func<Language.MaybeType, (Identifier, IExprTransformResult, ICodeTransformEnvironment), IOptional<IInstanceFieldReference>> VisitMaybeType => (type,args) =>
                Optional<IInstanceFieldReference>.Empty;

            public Func<Language.ProbablyType, (Identifier, IExprTransformResult, ICodeTransformEnvironment), IOptional<IInstanceFieldReference>> VisitProbablyType => (type, args) =>
            {
                args.Item2= args.Item2.EnsureType(type.PotentialType, args.Item3);
                return type.PotentialType.Visit(this, args);
            };
        }

        

        public override IOptional<ICallableReference> GetMethod(IExprTransformResult value, IArgIdentifier<Identifier, Language.IType> method, IEnumerable<Language.IType> argTypes)
        {
            return value.Type.GetInstanceMethodReference(method.Name.Name, method.Arguments, argTypes, Context).Bind(imr=>new InstantiatedCallableReference(imr, value));
        }
        internal class MethodLookupContext
        {
            public MethodLookupContext(IExprTransformResult receiver, IArgIdentifier<Identifier, Language.IType> methodName, IEnumerable<Language.IType> argtypes, ITypeCheckLookup<Language.IType, Language.ITypeArgument> context)
            {
                Receiver = receiver;
                MethodName = methodName;
                ArgumentTypes = argtypes;
                Context = context;
            }

            public IExprTransformResult Receiver { get; }
            public ITypeCheckLookup<Language.IType, Language.ITypeArgument> Context { get; }
            public IArgIdentifier<Identifier, Language.IType> MethodName { get; }
            public IEnumerable<Language.IType> ArgumentTypes { get; }
        }
        internal class BottomMethodReference : IInstanceMethodReference
        {
            public BottomMethodReference(string methodName, IEnumerable<Language.IType> typeArguments)
            {
                MethodName = methodName;
                TypeArguments = typeArguments.ToList();
            }

            public string MethodName { get; }

            public IEnumerable<Language.IType> TypeArguments { get; }
            
            public IInstanceMethodReference OptimisticVersion => this;

            public ICallReceiverTransformResult GenerateCall(IExprTransformResult receiver, IEnumerable<IExprTransformResult> arguments, ICodeTransformEnvironment env)
            {
                List<IInstruction> instructions = arguments.Flatten().ToList();
                var cemi = new CallExpandoMethodInstruction(receiver.Register, MethodName, TypeArguments, arguments.Select(arg => arg.Register), env.CreateRegister());
                return new CallReceiverTransformResult(new Language.DynamicType(), cemi.Register, instructions.Snoc(cemi));
            }

        }
        internal class CheckedMethodReference : IInstanceMethodReference
        {
            public CheckedMethodReference(IMethodSpec method, ITypeEnvironment<Language.ITypeArgument> substitutions, ITypeEnvironment<Language.IType> environment)
            {
                Method = method;
                Environment = environment;
                Substitutions = substitutions;
            }
            public IMethodSpec Method { get; }
            public ITypeEnvironment<Language.IType> Environment { get; }
            public ITypeEnvironment<Language.ITypeArgument> Substitutions { get; }
            public IInstanceMethodReference OptimisticVersion => throw new NotImplementedException();

            public ICallReceiverTransformResult GenerateCall(IExprTransformResult receiver, IEnumerable<IExprTransformResult> arguments, ICodeTransformEnvironment env)
            {
                List<IInstruction> instructions = new List<IInstruction>(); 
                List<IRegister> argRegs = new List<IRegister>();
                foreach ((IExprTransformResult, Language.IType) argpair in arguments.Zip(Method.Parameters.Entries.Select(ps => ((ISubstitutable<Language.IType>)ps.Type).Substitute(Substitutions)), (x, y) => (x, y)))
                {
                    IExprTransformResult actualArgument = argpair.Item1.EnsureType(argpair.Item2, env);
                    instructions.AddRange(actualArgument);
                    argRegs.Add(actualArgument.Register);

                }
                CallInstanceMethodCheckedInstruction cmi = new CallInstanceMethodCheckedInstruction(Method.GetAsTypedRef(), receiver.Register, argRegs, env.CreateRegister());
                return new CallReceiverTransformResult(((ISubstitutable<Language.IType>)Method.ReturnType).Substitute(Substitutions), cmi.Register, instructions.Snoc(cmi));
            }

        }
        internal class MethodLookupVisitor : Language.ITypeVisitor<MethodLookupContext, IOptional<IInstanceMethodReference>>
        {
            public static MethodLookupVisitor Instance = new MethodLookupVisitor();
            private MethodLookupVisitor() { }
            public Func<BotType, MethodLookupContext, IOptional<IInstanceMethodReference>> VisitBotType => (type, context) =>
            {
                throw new NotImplementedException();
            };

            public Func<Language.ClassType, MethodLookupContext, IOptional<IInstanceMethodReference>> VisitClassType => (type, context) =>
            {
                throw new NotImplementedException();
            };

            public Func<InterfaceType, MethodLookupContext, IOptional<IInstanceMethodReference>> VisitInterfaceType => (type, context) =>
            {
                throw new NotImplementedException();
            };

            public Func<TopType, MethodLookupContext, IOptional<IInstanceMethodReference>> VisitTopType => (type, context) => Optional<IInstanceMethodReference>.Empty;



            public Func<TypeVariable, MethodLookupContext, IOptional<IInstanceMethodReference>> VisitTypeVariable => (type, context) => type.ParameterSpec.UpperBound.Visit(this, context);

            public Func<Language.DynamicType, MethodLookupContext, IOptional<IInstanceMethodReference>> VisitDynamicType => (type, context) => throw new NotImplementedException(); 

            public Func<Language.MaybeType, MethodLookupContext, IOptional<IInstanceMethodReference>> VisitMaybeType => throw new NotImplementedException();

            public Func<Language.ProbablyType, MethodLookupContext, IOptional<IInstanceMethodReference>> VisitProbablyType => throw new NotImplementedException();
        }

        public override IOptional<IVariableReference> GetStaticFieldAccessVariable(IParamRef<INamespaceSpec, Language.IType> ns, Identifier ident)
        {
            throw new NotImplementedException();
        }

        public override IOptional<ICallableReference> GetStaticMethod(IParamRef<INamespaceSpec, Language.IType> ns, IArgIdentifier<Identifier, Language.IType> method, IEnumerable<Language.IType> argTypes)
        {
            IParamRef<IClassSpec, Language.IType> cls = ns.Visit(new ParamRefVisitor<object, IParamRef<IClassSpec, Language.IType>, Language.IType>((x, arg) => throw new NotImplementedException(), (x, arg) => throw new NotImplementedException(), (x, arg) => x));
            List<IStaticMethodSpec> methods = cls.Element.StaticMethods.Where(m => m.Visibility >= this.Context.MembersVisibleAt(ns.Element) && m.Name == method.Name.Name && m.TypeParameters.Count() == method.Arguments.Count() && m.Parameters.Entries.Count() == argTypes.Count() && m.Parameters.WouldAccept(cls.PArguments.Push(m.TypeParameters, method.Arguments), argTypes, true)).ToList();
            if (methods.Count == 0)
            {
                return Optional<ICallableReference>.Empty;
            }
            if (methods.Count == 1)
            {
                return new UniqueStaticMethodReference(methods.Single(), ns.PArguments.Push(methods.Single().TypeParameters, method.Arguments)).InjectOptional();
            }
            else
            {
                throw new NotImplementedException();
            }
        }

        public override IRegister CreateRegister()
        {
            Register reg = new Register(this.registers.Count);
            registers.Add(reg);
            return reg;
        }

        private class Register : IRegister
        {
            public Register(int index)
            {
                Index = index;
            }
            public int Index { get; }
        }
    }

    internal abstract class AMemberCodeTransformEnvirnment : ATopLevelCodeTransformEnvironment
    {

        protected TDClass Class { get; }
        public Language.ClassType LocalClassType { get; }
        public AMemberCodeTransformEnvirnment(TDClass cls, ITypeParametersSpec typeParameters, IParametersSpec parameters, ITypeCheckLookup<Language.IType, Language.ITypeArgument> context, IOptional<Language.IType> expectedReturnType) : base(context, expectedReturnType)
        {
            Class = cls;
            LocalClassType = new Language.ClassType(cls, context.TypeEnvironment.Transform<Language.ITypeArgument>(x => x));
        }

        public override IOptional<IVariableReference> this[Identifier ident] => base[ident].Coalesce(() => FindField(ident));
        protected abstract IOptional<IVariableReference> FindField(Identifier ident);
        protected virtual IOptional<IVariableReference> FindStaticField(Identifier ident)
        {
            return Class.StaticFields.SingleOrDefault(fd => fd.Name == ident.Name).InjectOptional().Bind(fd => new StaticFieldReference(ident, fd));
        }

        public override IOptional<ICallableReference> GetLocalCallable(IArgIdentifier<Identifier, Language.IType> callableName, IEnumerable<Language.IType> argTypes)
        {
            if (callableName.Arguments.Count() == 0)
            {
                var lc = this[callableName.Name].Join(lvr => AsCallable(lvr.GenerateReadAccess(this), argTypes));
                if (lc.HasElem)
                {
                    return lc;
                }
            }
            return FindMethod(callableName, argTypes);
        }

        protected virtual IOptional<ICallableReference> FindMethod(IArgIdentifier<Identifier, Language.IType> callableName, IEnumerable<Language.IType> argTypes)
        {
            return GetStaticMethod(new ClassRef<Language.IType>(LocalClassType.Class, Context.TypeEnvironment), callableName, argTypes);
        }
    }

    internal abstract class AInstanceMethodCodeTransformEnvironment : AMemberCodeTransformEnvirnment
    {
        public readonly IRegister ThisRegister;
        bool thisEnabled = false;
        public AInstanceMethodCodeTransformEnvironment(TDClass cls, ITypeParametersSpec typeParameters, IParametersSpec parameters, ITypeCheckLookup<Language.IType, Language.ITypeArgument> context, IOptional<Language.IType> expectedReturnType) : base(cls, typeParameters, parameters, context, expectedReturnType)
        {
            ThisRegister = CreateRegister();
            InitializeParameters(parameters);
        }

        protected override IOptional<IVariableReference> FindField(Identifier ident)
        {
            return FindLocalCheckedField(ident).Coalesce(() => FindStaticField(ident).Coalesce(() => FindLocalExpandoField(ident)));
        }

        protected virtual IOptional<IVariableReference> FindLocalCheckedField(Identifier ident)
        {
            if (thisEnabled)
            {
                return Class.Fields.SingleOrDefault(fd => fd.Name == ident.Name).InjectOptional().Bind(fd => new CheckedFieldReference(ident, fd, new ExprTransformResult(LocalClassType, ThisRegister, new List<IInstruction>())));
            }
            return Optional<IVariableReference>.Empty;
        }

        protected virtual IOptional<IVariableReference> FindLocalExpandoField(Identifier ident)
        {
            if (Class.IsExpando)
            {
                return new ExpandoFieldReference(ident, new ExprTransformResult(LocalClassType, ThisRegister, new List<IInstruction>())).InjectOptional();
            }
            return Optional<IVariableReference>.Empty;
        }
        protected override IOptional<ICallableReference> FindMethod(IArgIdentifier<Identifier, Language.IType> callableName, IEnumerable<Language.IType> argTypes)
        {
            return this[new Identifier("this")].Join(v => GetMethod(v.GenerateReadAccess(this), callableName, argTypes)).Coalesce(() => base.FindMethod(callableName, argTypes));
        }

        public virtual void EnableThis()
        {
            AddLocalVariable("this", LocalClassType, ThisRegister, true);
            AddLocalVariable("dis", new Language.DynamicType(), ThisRegister, true);
            thisEnabled = true;
        }
    }

    internal abstract class AUnscopedChildCodeTransformEnvironment : ICodeTransformEnvironment
    {
        public ICodeTransformEnvironment ParentEnv { get; }
        public IOptional<Language.IType> ExpectedReturnType => ParentEnv.ExpectedReturnType;
        public AUnscopedChildCodeTransformEnvironment(ICodeTransformEnvironment parent)
        {
            ParentEnv = parent;
        }

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

        virtual public IOptional<IVariableReference> this[Parser.Identifier ident] => ParentEnv[ident];

        public ITypeCheckLookup<Language.IType, Language.ITypeArgument> Context => ParentEnv.Context;

        public Language.IType BestReturnType { get => ParentEnv.BestReturnType; set => ParentEnv.BestReturnType = value; }

        virtual public IVariableReference AddLocalVariable(Identifier ident, Language.IType type)
        {
            return ParentEnv.AddLocalVariable(ident, type);
        }

        virtual public IOptional<ICallableReference> AsCallable(IExprTransformResult value, IEnumerable<Language.IType> argTypes)
        {
            return ParentEnv.AsCallable(value, argTypes);
        }

        virtual public IOptional<IVariableReference> GetFieldAccessVariable(IExprTransformResult value, Identifier ident)
        {
            return ParentEnv.GetFieldAccessVariable(value, ident);
        }

        virtual public IOptional<ICallableReference> GetMethod(IExprTransformResult value, IArgIdentifier<Identifier, Language.IType> method, IEnumerable<Language.IType> argTypes)
        {
            return ParentEnv.GetMethod(value, method, argTypes);
        }

        virtual public IOptional<IVariableReference> GetStaticFieldAccessVariable(Language.IParamRef<Language.INamespaceSpec, Language.IType> ns, Identifier ident)
        {
            return ParentEnv.GetStaticFieldAccessVariable(ns, ident);
        }

        virtual public IOptional<ICallableReference> GetStaticMethod(Language.IParamRef<Language.INamespaceSpec, Language.IType> ns, IArgIdentifier<Identifier, Language.IType> method, IEnumerable<Language.IType> argTypes)
        {
            return ParentEnv.GetStaticMethod(ns, method, argTypes);
        }

        virtual public IOptional<ICallableReference> GetLocalCallable(IArgIdentifier<Identifier, Language.IType> callableName, IEnumerable<Language.IType> argTypes)
        {
            return ParentEnv.GetLocalCallable(callableName, argTypes);
        }

        virtual public IOptional<ICallableReference> GetConstructor(IParamRef<IClassSpec, Language.IType> cls, IEnumerable<Language.IType> argTypes)
        {
            return ParentEnv.GetConstructor(cls, argTypes);
        }

        virtual public IRegister CreateRegister()
        {
            return ParentEnv.CreateRegister();
        }

        virtual public ILocalVariable GetCurrentVersion(ILocalVariable var)
        {
            return ParentEnv.GetCurrentVersion(var);
        }

        virtual public IOptional<ILocalVariable> GetLocalVariable(string name)
        {
            return ParentEnv.GetLocalVariable(name);
        }

        public void MergeIn(ICodeTransformEnvironment env)
        {
            ParentEnv.MergeIn(env);
        }

        public PhiNode GetBreakPHI(int levels)
        {
            return ParentEnv.GetBreakPHI(levels);
        }

        public PhiNode GetContinuePHI(int levels)
        {
            return ParentEnv.GetContinuePHI(levels);
        }

        public ILocalVariable GetTopmostVersion(ILocalVariable var)
        {
            return ParentEnv.GetTopmostVersion(var);
        }

        public bool DescendedFrom(ICodeTransformEnvironment ancestor)
        {
            return ancestor == this || ParentEnv.DescendedFrom(ancestor);
        }
    }
    public abstract class AScopedChildTransformEnvironment : ACodeTransformEnvironment
    {
        private ICodeTransformEnvironment parent;

        protected abstract class ALocalVariableCopy : ILocalVariable
        {
            public readonly ILocalVariable Parent;
            private IRegister currentRegister;

            public ALocalVariableCopy(ILocalVariable parent)
            {
                Parent = parent;
            }
            public string Name => Parent.Name;
            public Language.IType Type => Parent.Type;
            public bool IsReadOnly => Parent.IsReadOnly;

            public virtual bool IsInitialized => CurrentRegister != null;

            public IRegister CurrentRegister
            {
                get => currentRegister;
                set
                {
                    if (RegisterChange != null)
                    {
                        RegisterChange(this, currentRegister, value);
                    }
                    currentRegister = value;
                }
            }

            public event Action<ILocalVariable, IRegister, IRegister> RegisterChange;

            public virtual bool Matches(ILocalVariable other)
            {
                return other == this || Parent.Matches(other);
            }

            public abstract void UpdateFrom(ILocalVariable other);
        }
        public ICodeTransformEnvironment Parent
        {
            get => parent;
            protected set
            {
                if (parent != null)
                {
                    parent.VariableRegisterChanged -= Parent_VariableRegisterChanged;
                }
                parent = value;
                if (parent != null)
                {
                    parent.VariableRegisterChanged += Parent_VariableRegisterChanged;
                }
            }
        }

        protected abstract ALocalVariableCopy CreateLocalVariableCopy(ILocalVariable lv);
        protected abstract IEnumerable<ALocalVariableCopy> LocalVariableCopies { get; }
        protected abstract IOptional<ALocalVariableCopy> GetLocalVariableCopy(string name);

        public AScopedChildTransformEnvironment(ICodeTransformEnvironment parent)
        {
            Parent = parent;
        }

        private void Parent_VariableRegisterChanged(ICodeTransformEnvironment arg1, ILocalVariable arg2, IRegister arg3, IRegister arg4)
        {
            if (!LocalVariableCopies.Any(var => var.Matches(arg2)))
            {
                RaiseVariableRegisterChanged(arg1, arg2, arg3, arg4);
            }
        }


        //By the time we get to asking the parent, all local variables should have been handled, so only non-local variable variable references can come from the parent
        public override IOptional<IVariableReference> this[Identifier ident] => base[ident].Coalesce(() => Parent[ident]);

        protected override IOptional<ILocalVariable> GetOuterLocalVariable(string name)
        {
            return GetLocalVariableCopy(name).Coalesce(() => Parent.GetLocalVariable(name).Bind(lv => CreateLocalVariableCopy(lv)));
        }

        public override IOptional<Language.IType> ExpectedReturnType => Parent.ExpectedReturnType;

        public override ITypeCheckLookup<Language.IType, Language.ITypeArgument> Context => Parent.Context;



        public override IOptional<ICallableReference> AsCallable(IExprTransformResult value, IEnumerable<Language.IType> argTypes)
        {
            return Parent.AsCallable(value, argTypes);
        }

        public override IRegister CreateRegister()
        {
            return Parent.CreateRegister();
        }

        public override IOptional<ICallableReference> GetConstructor(IParamRef<IClassSpec, Language.IType> cls, IEnumerable<Language.IType> argTypes)
        {
            return Parent.GetConstructor(cls, argTypes);
        }

        public override ILocalVariable GetCurrentVersion(ILocalVariable var)
        {
            var res = TryGetCurrentVersion(var);
            if (res.HasElem)
            {
                return res.Elem;
            }
            return Parent.GetCurrentVersion(var);
        }
        public override ILocalVariable GetTopmostVersion(ILocalVariable var)
        {
            var res = TryGetTopmostVersion(var);
            if (res.HasElem)
            {
                return res.Elem;
            }
            return Parent.GetTopmostVersion(var);
        }
        protected override IOptional<ILocalVariable> TryGetCurrentVersion(ILocalVariable var)
        {
            //NOTE: hopefully this doesn't break anything
            var btcv = base.TryGetCurrentVersion(var);
            if (btcv.HasElem)
            {
                return btcv;
            }
            //END NOTE
            if (LocalVariableCopies.Any(lvc => lvc.Matches(var)))
            {
                return LocalVariableCopies.Single(lvc => lvc.Matches(var)).InjectOptional();
            }
            return base.TryGetCurrentVersion(var);
        }
        protected override IOptional<ILocalVariable> TryGetTopmostVersion(ILocalVariable var)
        {
            //NOTE: hopefully this doesn't break anything
            var btmv = base.TryGetTopmostVersion(var);
            if(btmv.HasElem)
            {
                return btmv;
            }
            //END NOTE
            if (LocalVariableCopies.Any(lvc => lvc.Matches(var) || var.Matches(lvc)))
            {
                return LocalVariableCopies.Single(lvc => lvc.Matches(var) || var.Matches(lvc)).InjectOptional();
            }
            return base.TryGetCurrentVersion(var);
        }

        public override IOptional<IVariableReference> GetFieldAccessVariable(IExprTransformResult value, Identifier ident)
        {
            return Parent.GetFieldAccessVariable(value, ident);
        }

        public override IOptional<ICallableReference> GetLocalCallable(IArgIdentifier<Identifier, Language.IType> callableName, IEnumerable<Language.IType> argTypes)
        {
            return Parent.GetLocalCallable(callableName, argTypes);
        }

        public override IOptional<ICallableReference> GetMethod(IExprTransformResult value, IArgIdentifier<Identifier, Language.IType> method, IEnumerable<Language.IType> argTypes)
        {
            return Parent.GetMethod(value, method, argTypes);
        }

        public override IOptional<IVariableReference> GetStaticFieldAccessVariable(IParamRef<INamespaceSpec, Language.IType> ns, Identifier ident)
        {
            return Parent.GetStaticFieldAccessVariable(ns, ident);
        }

        public override IOptional<ICallableReference> GetStaticMethod(IParamRef<INamespaceSpec, Language.IType> ns, IArgIdentifier<Identifier, Language.IType> method, IEnumerable<Language.IType> argTypes)
        {
            return Parent.GetStaticMethod(ns, method, argTypes);
        }

        public override void MergeIn(ICodeTransformEnvironment env)
        {
            base.MergeIn(env);
            foreach (ALocalVariableCopy lv in LocalVariableCopies)
            {
                lv.UpdateFrom(env.GetCurrentVersion(lv));
            }
        }
        public override bool DescendedFrom(ICodeTransformEnvironment ancestor)
        {
            return this == ancestor || Parent.DescendedFrom(ancestor);
        }
    }

    public class ScopeTransformEnvironment : AScopedChildTransformEnvironment
    {
        protected class LocalVariableCopy : ALocalVariableCopy
        {
            public LocalVariableCopy(ILocalVariable parent) : base(parent)
            {
                CurrentRegister = parent.CurrentRegister;
            }


            public override void UpdateFrom(ILocalVariable other)
            {
                CurrentRegister = other.CurrentRegister;
            }
        }
        public ScopeTransformEnvironment(ICodeTransformEnvironment parent) : base(parent)
        {
        }

        private Dictionary<string, LocalVariableCopy> localVariableCopies = new Dictionary<string, LocalVariableCopy>();
        protected override IEnumerable<ALocalVariableCopy> LocalVariableCopies => localVariableCopies.Values;

        protected override ALocalVariableCopy CreateLocalVariableCopy(ILocalVariable lv)
        {
            var lvc = new LocalVariableCopy(lv);
            localVariableCopies.Add(lv.Name, lvc);
            lvc.RegisterChange += RaiseVariableRegisterChanged;
            return lvc;
        }

        protected override IOptional<ALocalVariableCopy> GetLocalVariableCopy(string name)
        {
            if (localVariableCopies.ContainsKey(name))
            {
                return localVariableCopies[name].InjectOptional();
            }
            return Optional<ALocalVariableCopy>.Empty;
        }

        public override PhiNode GetBreakPHI(int levels)
        {
            return Parent.GetBreakPHI(levels);
        }

        public override PhiNode GetContinuePHI(int levels)
        {
            return Parent.GetContinuePHI(levels);
        }
    }

    internal class InstanceMethodTransformEnvironment : AInstanceMethodCodeTransformEnvironment
    {
        public InstanceMethodTransformEnvironment(TDClass cls, ITypeParametersSpec typeParameters, IParametersSpec parameters, Language.IType expectedReturnType, ITypeCheckLookup<Language.IType, Language.ITypeArgument> context) : base(cls, typeParameters, parameters, context, expectedReturnType.InjectOptional())
        {
            EnableThis();
        }
        public override PhiNode GetBreakPHI(int levels)
        {
            throw new TypeCheckException("No loop to break out of!");
        }

        public override PhiNode GetContinuePHI(int levels)
        {
            throw new NotImplementedException("No loop to continue!");
        }
    }

    internal class ConstructorTransformEnvironment : AInstanceMethodCodeTransformEnvironment
    {
        private Dictionary<IFieldSpec, InstanceFieldVariable> fieldVariables = new Dictionary<IFieldSpec, InstanceFieldVariable>();
        private class InstanceFieldVariable : ILocalVariable
        {
            public readonly IFieldSpec Field;
            private IRegister currentRegister;

            public InstanceFieldVariable(IFieldSpec fieldSpec)
            {
                Field = fieldSpec;
            }

            public string Name => Field.Name;

            public Language.IType Type => Field.Type;

            public bool IsInitialized => CurrentRegister != null;

            public bool IsReadOnly => false;

            public IRegister CurrentRegister
            {
                get => currentRegister;
                set
                {
                    if (RegisterChange != null)
                    {
                        RegisterChange(this, currentRegister, value);
                    }
                    currentRegister = value;
                }
            }

            public event Action<ILocalVariable, IRegister, IRegister> RegisterChange;

            public bool Matches(ILocalVariable other)
            {
                return other == this;
            }
        }
        public ConstructorTransformEnvironment(TDClass cls, ITypeParametersSpec typeParameters, IParametersSpec parameters, ITypeCheckLookup<Language.IType, Language.ITypeArgument> context) : base(cls, typeParameters, parameters, context, Optional<Language.IType>.Empty)
        {
            foreach (var field in Class.LocalFields)
            {
                fieldVariables.Add(field, new InstanceFieldVariable(field));
            }
        }

        protected override IOptional<IVariableReference> FindField(Identifier ident)
        {
            return fieldVariables.Values.FirstOrDefault(fd => fd.Name == ident.Name).InjectOptional().Bind(fv => new LocalVariableRef(ident, fv));
        }

        public IVariableReference GetFieldVariable(FieldSpec field)
        {
            if (!fieldVariables.ContainsKey(field))
            {
                throw new InternalException("Asked for missing field");
            }
            return new LocalVariableRef(field.Identifier, fieldVariables[field]);
        }

        public IOptional<ICallableReference> GetSuperConstructor(IEnumerable<Language.IType> argTypes)
        {
            var scelem = Class.SuperClass.Elem.Element;
            var constructors = scelem.Constructors;
            var cspecs = constructors.Where(
                cd => cd.Visibility >= Visibility.Protected && cd.Parameters.WouldAccept(Class.SuperClass.Elem.PArguments, argTypes, true));
            if (cspecs.Count() > 1)
            {
                throw new InternalException("Ambiguous constructor reached constructor selection!");
            }
            if (cspecs.Count() == 1)
            {
                return new UniqueConstructorCall(new ParameterizedSpecRef<IConstructorSpec>(cspecs.Single(), Class.SuperClass.Elem.PArguments.Transform<Language.ITypeArgument>(t=>t))).InjectOptional();
            }
            cspecs = constructors.Where(cd => cd.Visibility >= Visibility.Protected && cd.Parameters.WouldAccept(Class.SuperClass.Elem.PArguments, argTypes, true));
            throw new NotImplementedException();
        }

        public override PhiNode GetBreakPHI(int levels)
        {
            throw new TypeCheckException("No loop to break out of!");
        }

        public override PhiNode GetContinuePHI(int levels)
        {
            throw new NotImplementedException("No loop to continue!");
        }
    }


    internal class StaticMethodTransformEnvironment : AMemberCodeTransformEnvirnment
    {

        public StaticMethodTransformEnvironment(TDClass cls, ITypeParametersSpec typeParameters, IParametersSpec parameters, Language.IType expectedReturnType, ITypeCheckLookup<Language.IType, Language.ITypeArgument> context) : base(cls, typeParameters, parameters, context, expectedReturnType.InjectOptional())
        {
            InitializeParameters(parameters);
        }

        protected override IOptional<IVariableReference> FindField(Identifier ident)
        {
            return FindStaticField(ident);
        }
        public override PhiNode GetBreakPHI(int levels)
        {
            throw new TypeCheckException("No loop to break out of!");
        }

        public override PhiNode GetContinuePHI(int levels)
        {
            throw new NotImplementedException("No loop to continue!");
        }
    }
}
