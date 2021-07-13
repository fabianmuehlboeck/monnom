using System;
using System.Collections.Generic;
using System.Text;
using Nom.Language;
using System.IO;
using System.Linq;
using Nom.TypeChecker;
using Nom.Language.SpecExtensions;

namespace Nom.Bytecode
{
    public class BytecodeUnit : IConstantSource
    {
        public static readonly uint BYTECODE_VERSION = 1;
        public AssemblyUnit AssemblyUnit { get; }
        private List<ClassRep> classes = new List<ClassRep>();
        private List<InterfaceRep> interfaces = new List<InterfaceRep>();
        private List<IConstant> constants = new List<IConstant>();
        private T RegisterConstant<T>(T constant) where T : IConstant
        {
            constants.Add(constant);
            return constant;
        }

        public IEnumerable<ClassRep> Classes => classes;
        public IEnumerable<InterfaceRep> Interfaces => interfaces;
        public string Name { get; }
        private ulong constantCounter = 0;
        public BytecodeUnit(string name, AssemblyUnit au)
        {
            AssemblyUnit = au;
            Name = name;
            typeVarConstants = new ConstantDict<int, TypeVariableConstant>(index => RegisterConstant(new TypeVariableConstant(++constantCounter, index)));
            stringConstants = new ConstantDict<string, StringConstant>(str => str.Length>0?RegisterConstant(new StringConstant(str, ++constantCounter)):StringConstant.EmptyStringConstant);
            superClassConstants = new ConstantDict<IParamRef<IClassSpec, IType>, SuperClassConstant>(sc => RegisterConstant(new SuperClassConstant(GetClassConstant(sc.Element), GetTypeListConstant(sc.Substitutions.OrderBy(kvp => kvp.Key.Index).Select(kvp => kvp.Value)), ++constantCounter)));//TODO: get right argument list
            classConstants = new ConstantDict<IClassSpec, ClassConstant>(sc => RegisterConstant(new ClassConstant(++constantCounter, GetStringConstant(sc.Library.Name), GetStringConstant(sc.FullQualifiedName))));
            interfaceConstants = new ConstantDict<IInterfaceSpec, InterfaceConstant>(sc => RegisterConstant(new InterfaceConstant(++constantCounter, GetStringConstant(sc.Library.Name), GetStringConstant(sc.FullQualifiedName))));
            classTypeConstants = new ConstantDict<IParamRef<IInterfaceSpec, ITypeArgument>, ClassTypeConstant>(sc => RegisterConstant(new ClassTypeConstant(++constantCounter, sc.Visit(new ParamRefVisitor<BytecodeUnit, IConstantRef<INamedConstant>, Language.ITypeArgument>((NamespaceSpecVisitor, bcu)=> { throw new InternalException("Can't use a namespace as class type"); }, (t, env) => env.GetInterfaceConstant(t.Element), (t, bcu) => bcu.GetClassConstant(t.Element)), this), GetTypeListConstant(sc.Substitutions.OrderBy(kvp=>kvp.Key.Index).Select(kvp=>kvp.Value)))));
            maybeTypeConstants = new ConstantDict<IType, MaybeTypeConstant>(tp => RegisterConstant(new MaybeTypeConstant(++constantCounter, GetTypeConstant(tp))));
            staticMethodConstants = new ConstantDict<IParameterizedSpecRef<IStaticMethodSpec>, StaticMethodConstant>(sm => RegisterConstant(new StaticMethodConstant(++constantCounter, (IConstantRef<ClassConstant>)GetNamespaceConstant(sm.Element.Container), GetStringConstant(sm.Element.Name), GetTypeListConstant(sm.Arguments), GetTypeListConstant(sm.Element.Parameters.Entries.Select(ps => ps.Type)))));
            methodConstants = new ConstantDict<IParameterizedSpecRef<IMethodSpec>, MethodConstant>(sm =>
            {
                Func<INamespaceSpec, object, IParamRef<IInterfaceSpec, ITypeArgument>> nsh = (ns, o) => { throw new InternalException("Namespace can't contain method!"); };
                Func<IInterfaceSpec, object, IParamRef<IInterfaceSpec, ITypeArgument>> ifaceh = (iface, o) => new InterfaceRef<ITypeArgument>(iface, sm.Substitutions.Restrict(sm.Element.Container));
                Func<IClassSpec, object, IParamRef<IInterfaceSpec, ITypeArgument>> clsh = (cls, o) => new ClassRef<ITypeArgument>(cls, sm.Substitutions.Restrict(sm.Element.Container));
                var nsv = new NamespaceSpecVisitor<object, IParamRef<IInterfaceSpec, ITypeArgument>>(
                        nsh,
                        ifaceh,
                        clsh);
                return RegisterConstant(new MethodConstant(
                    ++constantCounter,
                    /*(IConstantRef<INamedConstant>)GetNamespaceConstant(sm.Element.Container)*/
                    GetClassTypeConstant(sm.Element.Container.Visit(nsv)),
                    //sm.ParameterizedParent.Extract(pp => pp.Substitutions)),
                    GetStringConstant(sm.Element.Name),
                    GetTypeListConstant(sm.Arguments),
                    GetTypeListConstant(sm.Element.Parameters.Entries.Select(ps => ((ISubstitutable<Language.IType>)ps.Type).Substitute(sm.Substitutions.Transform<Language.IType>(t => t.AsType))))));
            });
            lambdaConstants = new ConstantDict<ITDLambda, LambdaConstant>(index => RegisterConstant(new LambdaConstant(++constantCounter)));
            structConstants = new ConstantDict<ITDStruct, StructConstant>(index => RegisterConstant(new StructConstant(++constantCounter)));
        }

        public void Emit(Func<String, Stream> openStream)
        {
            //FileInfo fi = new FileInfo(dir.FullName + "/" + Name + ".gnil");
            //if (fi.Exists)
            //{
            //    fi.Delete();
            //}
            using (Stream fs = openStream(Name))
            {
                fs.WriteValue(BYTECODE_VERSION);
                foreach (IConstant constant in constants)
                {
                    constant.Emit(fs);
                }
                foreach (ClassRep cr in classes)
                {
                    cr.Emit(fs);
                }
                foreach (InterfaceRep ir in interfaces)
                {
                    ir.Emit(fs);
                }
            }
        }

        private class ConstantRef<T> : IConstantRef<T> where T : IConstant
        {
            public ConstantRef(T constant)
            {
                Constant = constant;
                ConstantID = constant.ConstantID;
            }
            public T Constant { get; }
            public ulong ConstantID { get; }
        }

        private class ConstantDict<V, C> where C : IConstant
        {
            private readonly Func<V, C> converter;
            private Dictionary<V, C> entries = new Dictionary<V, C>();
            public ConstantDict(Func<V, C> converter)
            {
                this.converter = converter;
            }
            public IConstantRef<C> GetConstant(V value)
            {
                if (!entries.ContainsKey(value))
                {
                    entries.Add(value, converter(value));
                }
                return new ConstantRef<C>(entries[value]);
            }
        }

        public void AddClass(ClassRep cls)
        {
            classes.Add(cls);
        }
        public void AddInterface(InterfaceRep ifc)
        {
            interfaces.Add(ifc);
        }

        public IConstantRef<IConstant> GetNamespaceConstant(INamespaceSpec ns)
        {
            return ns.Visit(new NamespaceSpecVisitor<object, IConstantRef<IConstant>>(
                (nsx, arg) => throw new NotImplementedException(),
                (iface, arg) => GetInterfaceConstant(iface),
                (cls, arg) => GetClassConstant(cls)));
        }


        private ConstantDict<IType, MaybeTypeConstant> maybeTypeConstants;
        public IConstantRef<MaybeTypeConstant> GetMaybeTypeConstant(MaybeType tp)
        {
            return maybeTypeConstants.GetConstant(tp.PotentialType);
        }
        private ConstantDict<IClassSpec, ClassConstant> classConstants;
        public IConstantRef<ClassConstant> GetClassConstant(IClassSpec cls)
        {
            return classConstants.GetConstant(cls);
        }
        public IConstantRef<IClassConstant> GetEmptyClassConstant() => new ConstantRef<IClassConstant>(EmptyClassConstant.Instance);

        private ConstantDict<IParamRef<IInterfaceSpec, Language.ITypeArgument>, ClassTypeConstant> classTypeConstants;
        public IConstantRef<ClassTypeConstant> GetClassTypeConstant(IParamRef<IInterfaceSpec, Language.ITypeArgument> ct)
        {
            return classTypeConstants.GetConstant(ct);
        }

        private ConstantDict<ITDStruct, StructConstant> structConstants;
        public IConstantRef<StructConstant> GetStructConstant(ITDStruct structure)
        {
            return structConstants.GetConstant(structure);
        }

        private ConstantDict<ITDLambda, LambdaConstant> lambdaConstants;
        public IConstantRef<LambdaConstant> GetLambdaConstant(ITDLambda lambda)
        {
            return lambdaConstants.GetConstant(lambda);
        }

        private ConstantDict<String, StringConstant> stringConstants;
        public IConstantRef<StringConstant> GetStringConstant(string str)
        {
            return stringConstants.GetConstant(str);
        }

        private ConstantDict<IParamRef<IClassSpec, IType>, SuperClassConstant> superClassConstants;
        public IConstantRef<SuperClassConstant> GetSuperClassConstant(IParamRef<IClassSpec, IType> superclass)
        {
            return superClassConstants.GetConstant(superclass);
        }

        public IConstantRef<SuperInterfacesConstant> GetSuperInterfacesConstant(IEnumerable<IParamRef<IInterfaceSpec, IType>> superInterfaces)
        {
            return new ConstantRef<SuperInterfacesConstant>(RegisterConstant(new SuperInterfacesConstant(++constantCounter, superInterfaces.Select(si => (GetInterfaceConstant(si.Element), GetTypeListConstant(si.PArguments.OrderBy(kvp=>kvp.Key.Index).Select(kvp=>kvp.Value)))))));
        }

        public IConstantRef<ITypeConstant> GetTypeConstant(IType type, bool defaultBottom = false)
        {
            return type.Visit(typeVisitor, (defaultBottom, this));
        }

        private static TypeListConstant emptyTLC = new TypeListConstant(0, new List<IConstantRef<ITypeConstant>>());
        private List<TypeListConstant> typeListConstants = new List<TypeListConstant>();
        public IConstantRef<TypeListConstant> GetTypeListConstant(IEnumerable<IType> types)
        {
            if (types.Count() == 0)
            {
                return new ConstantRef<TypeListConstant>(emptyTLC);
            }
            List<IConstantRef<ITypeConstant>> typeConstants = types.Select(t => GetTypeConstant(t)).ToList();
            IOptional<TypeListConstant> ret = typeListConstants.FirstOrDefault(tlc => tlc.TypeConstants.Count() == typeConstants.Count && tlc.TypeConstants.Zip(typeConstants, (x, y) => x.Equals(y)).All(x => x)).InjectOptional();
            if (!ret.HasElem)
            {
                ret = RegisterConstant(new TypeListConstant(++constantCounter, typeConstants)).InjectOptional();
                typeListConstants.Add(ret.Elem);
            }
            return new ConstantRef<TypeListConstant>(ret.Elem);
        }
        public IConstantRef<TypeListConstant> GetTypeListConstant(IEnumerable<ITypeArgument> types)
        {
            //return GetTypeListConstant(types.SelectMany(t => t.ContravariantPart.Singleton().Snoc(t.CovariantPart)));
            return GetTypeListConstant(types.Select(t => t.AsType));
        }

        private static TypeParametersConstant emptyTPC = new TypeParametersConstant(0, new List<TypeParameterEntry>());
        private List<TypeParametersConstant> typeParametersConstants = new List<TypeParametersConstant>();
        public IConstantRef<TypeParametersConstant> GetTypeParametersConstant(ITypeParametersSpec tps)
        {
            if (tps.Count() == 0)
            {
                return new ConstantRef<TypeParametersConstant>(emptyTPC);
            }
            var entries = tps.Select(tpsx => new TypeParameterEntry(/*tpsx.Variance,*/ GetTypeConstant(tpsx.LowerBound), GetTypeConstant(tpsx.UpperBound)));
            IOptional<TypeParametersConstant> ret = typeParametersConstants.FirstOrDefault(tpc => tpc.Entries.Count() == tps.Count() && tpc.Entries.Zip(entries, (x, y) => x.Equals(y)).All(x => x)).InjectOptional();
            if(!ret.HasElem)
            {
                ret = RegisterConstant(new TypeParametersConstant(++constantCounter, entries)).InjectOptional();
                typeParametersConstants.Add(ret.Elem);
            }
            return new ConstantRef<TypeParametersConstant>(ret.Elem);
        }

        public IConstantRef<IStringConstant> ReferenceStringConstant(ulong id)
        {
            throw new NotImplementedException();
        }

        public IConstantRef<ITypeConstant> ReferenceTypeConstant(ulong id)
        {
            throw new NotImplementedException();
        }

        public IConstantRef<TypeListConstant> ReferenceTypeListConstant(ulong id)
        {
            throw new NotImplementedException();
        }

        ConstantDict<IInterfaceSpec, InterfaceConstant> interfaceConstants;
        public IConstantRef<IInterfaceConstant> GetInterfaceConstant(IInterfaceSpec iface)
        {
            return interfaceConstants.GetConstant(iface);
        }

        ConstantDict<IParameterizedSpecRef<IStaticMethodSpec>, StaticMethodConstant> staticMethodConstants;
        public IConstantRef<StaticMethodConstant> GetStaticMethodConstant(IParameterizedSpecRef<IStaticMethodSpec> staticMethod)
        {
            return staticMethodConstants.GetConstant(staticMethod); // new ParameterizedSpecRef<IStaticMethodSpec>(staticMethod, new TypeEnvironment<ITypeArgument>(staticMethod.TypeParameters, typeArgs)));
        }

        ConstantDict<IParameterizedSpecRef<IMethodSpec>, MethodConstant> methodConstants;
        public IConstantRef<MethodConstant> GetMethodConstant(IParameterizedSpecRef<IMethodSpec> method)
        {
            return methodConstants.GetConstant(method); // new ParameterizedSpecRef<IMethodSpec>(method, new TypeEnvironment<ITypeArgument>(method.TypeParameters, typeArgs)));
        }

        private BottomTypeConstant bottomTypeConstant = null;
        public IConstantRef<ITypeConstant> GetBottomTypeConstant()
        {
            if(bottomTypeConstant==null)
            {
                bottomTypeConstant = RegisterConstant(new BottomTypeConstant(++constantCounter));
            }
            return new ConstantRef<BottomTypeConstant>(bottomTypeConstant);
        }

        private Dictionary<IEnumerable<IType>, IIntersectionConstant> intersectionConstants=new Dictionary<IEnumerable<IType>, IIntersectionConstant>();
        public IConstantRef<IIntersectionConstant> GetIntersectionTypeConstant(IEnumerable<IType> entries)
        {
            var matchKey = intersectionConstants.Keys.FirstOrDefault(k => k.SetEqual(entries));
            if (matchKey != null)
            {
                return new ConstantRef<IIntersectionConstant>(intersectionConstants[matchKey]);
            }
            var constnt = RegisterConstant(new IntersectionTypeConstant(++constantCounter, GetTypeListConstant(entries)));
            intersectionConstants.Add(entries, constnt);
            return new ConstantRef<IIntersectionConstant>(constnt);
        }

        private ConstantDict<int, TypeVariableConstant> typeVarConstants;
        public IConstantRef<TypeVariableConstant> GetTypeVariableConstant(int index)
        {
            return typeVarConstants.GetConstant(index);
        }

        private DynamicTypeConstant dynamicTypeConstant = null; 
        public IConstantRef<ITypeConstant> GetDynamicTypeConstant()
        {
            if(dynamicTypeConstant==null)
            {
                dynamicTypeConstant = this.RegisterConstant(new DynamicTypeConstant(++constantCounter));
            }
            return new ConstantRef<ITypeConstant>(dynamicTypeConstant);
        }

        static ITypeConstant zeroTypeConstant = new EmptyTypeConstant();
        static IConstantRef<ITypeConstant> GetEmptyTypeConstant() => new ConstantRef<ITypeConstant>(zeroTypeConstant);

        private readonly static TypeVisitor<(bool, BytecodeUnit), IConstantRef<ITypeConstant>> typeVisitor = new TypeVisitor<(bool, BytecodeUnit), IConstantRef<ITypeConstant>>()
        {
            VisitClassType = (ct, pair) => pair.Item2.GetClassTypeConstant(ct),
            VisitInterfaceType = (ct, pair) => pair.Item2.GetClassTypeConstant(ct),
            VisitTopType = (tt, pair) => pair.Item1 ? pair.Item2.GetIntersectionTypeConstant(new List<IType>()) : GetEmptyTypeConstant(),
            VisitBotType = (tt, pair) => pair.Item1 ? GetEmptyTypeConstant() : pair.Item2.GetBottomTypeConstant(),
            //VisitIntersectionType = (tt, pair) => pair.Item2.GetIntersectionTypeConstant(tt.Components),
            VisitTypeVariable = (tv, pair) => pair.Item2.GetTypeVariableConstant(tv.ParameterSpec.Index),
            //VisitTypeRange = (tr, pair) => tr.PessimisticType.Visit(typeVisitor, pair),
            VisitDynamicType = (dt, pair) => pair.Item2.GetDynamicTypeConstant(),
            VisitMaybeType = (mb, pair) => pair.Item2.GetMaybeTypeConstant(mb),
            VisitProbablyType = (mb, pair) => pair.Item2.GetMaybeTypeConstant(mb) //for runtime purposes, probably types are maybe types
        };
    }
}
