using System;
using System.Collections.Generic;
using System.Text;
using System.Linq;

namespace Nom.Language.SpecExtensions
{
    public static class SpecExtensions
    {
        /// <summary>
        /// includes superclasses for class refs, otherwise just superinterfaces
        /// </summary>
        public static IEnumerable<IParamRef<IInterfaceSpec,IType>> AllSuperNameds(this IParamRef<IInterfaceSpec, IType> ifs)
        {
            return ifs.Visit(new ParamRefVisitor<object, IEnumerable<IParamRef<IInterfaceSpec, IType>>, IType>((ns, o) => throw new InternalException("Super-nameds only exist for interfaces and classes!"), (ifc, o) => ifc.AllImplementedInterfaces(), (cls, o) => cls.AllImplementedInterfacesClasses()));
        }

        /// <summary>
        /// Just super-interfaces
        /// </summary>
        /// <param name="ifs"></param>
        /// <returns></returns>
        public static IEnumerable<IParamRef<IInterfaceSpec, IType>> AllImplementedInterfaces(this IParamRef<IInterfaceSpec, IType> ifs)
        {
            foreach (var ift in ifs.Element.Implements.Select(super=>super.TransformArgs(t=>ifs.PArguments.Aggregate(t,(tacc,kvp)=>((ISubstitutable<IType>)tacc).Substitute(kvp.Key, kvp.Value)))))
            {
                foreach (IParamRef<IInterfaceSpec, IType> iift in ift.AllImplementedInterfaces())
                {
                    yield return iift;
                }
                yield return ift;
            }
        }

        /// <summary>
        /// includes super classes, but param-ref needs to be know to be for class
        /// </summary>
        /// <param name="cls"></param>
        /// <returns></returns>
        public static IEnumerable<IParamRef<IInterfaceSpec, IType>> AllImplementedInterfacesClasses(this IParamRef<IClassSpec, IType> cls)
        {
            IOptional<IParamRef<IClassSpec, IType>> sci = cls.InstantiateSuperClass();
            if(sci.HasElem)
            {
                yield return sci.Elem;
                foreach(var scinh in sci.Elem.AllImplementedInterfacesClasses())
                {
                    yield return scinh;
                }
            }
            IParamRef<IInterfaceSpec, IType> ifcls = cls;
            foreach(var inh in ifcls.AllImplementedInterfaces())
            {
                yield return inh;
            }
        }

        /// <summary>
        /// searches, but does not include super classes, but param-ref needs to be know to be for class
        /// </summary>
        /// <param name="cls"></param>
        /// <returns></returns>
        public static IEnumerable<IParamRef<IInterfaceSpec, IType>> AllImplementedInterfaces(this IParamRef<IClassSpec, IType> cls)
        {
            IOptional<IParamRef<IClassSpec, IType>> sci = cls.InstantiateSuperClass();
            if (sci.HasElem)
            {
                foreach (var scinh in sci.Elem.AllImplementedInterfaces())
                {
                    yield return scinh;
                }
            }
            IParamRef<IInterfaceSpec, IType> ifcls = cls;
            foreach (var inh in ifcls.AllImplementedInterfaces())
            {
                yield return inh;
            }
        }

        public static IParamRef<T, ArgT> GetParamRef<T, ArgT>(this T elem) where T : INamespaceSpec where ArgT : ITypeArgument, ISubstitutable<ArgT>
        {
            //TODO: fix really dirty casting hack for paramRefs
            return elem.Visit(new NamespaceSpecVisitor<object, IParamRef<T, ArgT>>(
                (ns,o)=> (IParamRef<T, ArgT>)new NamespaceRef<ArgT>(ns, new TypeEnvironment<ArgT>()),
                (iface, o) => (IParamRef<T, ArgT>)new InterfaceRef<ArgT>(iface, new TypeEnvironment<ArgT>(iface.TypeParameters, iface.TypeParameters.Select(tp=>(ArgT)(object)new TypeVariable(tp)))),
                (cls, o) => (IParamRef<T, ArgT>)new ClassRef<ArgT>(cls, new TypeEnvironment<ArgT>(cls.TypeParameters, cls.TypeParameters.Select(tp=> (ArgT)(object)new TypeVariable(tp))))
                ));
        }

        public static IOptional<IParamRef<IClassSpec, IType>> InstantiateSuperClass(this IParamRef<IClassSpec, IType> cls)
        {
            return cls.Element.SuperClass.Bind(sc=>sc.TransformArgs(t => cls.PArguments.Aggregate(t, (tacc, kvp) => ((ISubstitutable<IType>)tacc).Substitute(kvp.Key, kvp.Value))));
        }

        public static ITypeEnvironment<ITypeArgument> ToTypeEnvironment(this IEnumerable<ITypeParameterSpec> tpss)
        {
            return new TypeEnvironment<ITypeArgument>(tpss, tpss.Select(tps => new TypeVariable(tps)));
        }

        public static bool Satisfies(this ITypeEnvironment<ITypeArgument> env, ITypeParametersSpec tpss)
        {
            return tpss.All(tps=>env.Any(kvp=>kvp.Key.Equals(tps)));
        }

        /// <summary>
        /// Hack to enable covariant spec refs generally. Guarded against assertion failures.
        /// </summary>
        /// <typeparam name="T"></typeparam>
        /// <param name="t"></param>
        /// <returns></returns>
        public static IParameterizedSpecRef<T> GetAsTypedRef<T>(this T t) where T : IParameterizedSpec
        {
            var psr = t.GetAsRef();
            if(psr.Element!=(object)t)
            {
                throw new InternalException("Typed parameterized spec on badly specialized object");
            }
            return new ParameterizedSpecRef<T>(t, psr.Substitutions);
        }

    }
}
