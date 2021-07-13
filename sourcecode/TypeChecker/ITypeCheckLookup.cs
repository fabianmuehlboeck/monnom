using Nom.Language;
using System.Collections.Generic;

namespace Nom.TypeChecker
{
    public interface ITypeCheckLookup<P, PX>
        where P : ISubstitutable<P>, PX
        where PX : ITypeArgument, ISubstitutable<PX>, INamedReference
    {
        IParamRef<INamespaceSpec, P> ContextRoot { get; }
        ITypeEnvironment<IType> TypeEnvironment { get; }
        internal ITDChild Container { get; }

        void AddUsing(ILookupContext<P, PX> usecontext);
        IParamRef<INamespaceSpec, P> GetChild(IArgIdentifier<string, P> name);
        IParamRef<INamespaceSpec, PX> GetChild(IArgIdentifier<string, PX> name);
        IParamRef<INamespaceSpec, P> GetChild(IQName<IArgIdentifier<string, P>> name);
        IParamRef<INamespaceSpec, PX> GetChild(IQName<IArgIdentifier<string, PX>> name);
        IOptional<ITypeParameterSpec> GetTypeVariable(string name);
        bool HasChild(IQName<IArgIdentifier<string, P>> name);
        Visibility MembersVisibleAt(INamespaceSpec ns);
        ITypeCheckLookup<P, PX> PushVariables(IEnumerable<ITypeParameterSpec> typeParams);

        internal ITypeCheckLookup<P, PX> Push(ITDChild container, IQName<IArgIdentifier<string, P>> name, IEnumerable<ILookupContext<P, PX>> protecteds);
        internal ITypeCheckLookup<P, PX> Push(ITDChild container, IArgIdentifier<string, P> name, IEnumerable<ILookupContext<P, PX>> protecteds);
    }
}