using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Nom.Language.SpecExtensions;

namespace Nom.Language
{
    public interface IParameterizedSpecRef<out T> : ISpecRef<T> where T : IParameterizedSpec
    {
        IEnumerable<ITypeArgument> Arguments
        {
            get;
        }

        ITypeEnvironment<ITypeArgument> Substitutions
        {
            get;
        }

        IParameterizedSpecRef<T> Substitute(ITypeEnvironment<ITypeArgument> substitutions);

        IOptional<IParameterizedSpecRef<IParameterizedSpec>> ParameterizedParent
        {
            get;
        }
    }

    public class ParameterizedSpecRef<T> : IParameterizedSpecRef<T> where T : IParameterizedSpec
    {
        public ParameterizedSpecRef(T element, ITypeEnvironment<ITypeArgument> substitutions)
        {
            Element = element;
            Substitutions = substitutions;
            if (!Substitutions.Satisfies(Element.AllTypeParameters))
            {
                throw new InternalException("Bad instantiation!");
            }
        }
        public T Element { get; }
        public ITypeEnvironment<ITypeArgument> Substitutions
        {
            get;
        }

        public IParameterizedSpecRef<T> Substitute(ITypeEnvironment<ITypeArgument> substitutions)
        {
            return new ParameterizedSpecRef<T>(Element, Substitutions.Substitute(substitutions));
        }

        public IEnumerable<ITypeArgument> Arguments => Element.TypeParameters.Select(tps => Substitutions[tps]).ToList();

        public IOptional<IParameterizedSpecRef<IParameterizedSpec>> ParameterizedParent => Element.ParameterizedParent.Bind(pp => new ParameterizedSpecRef<IParameterizedSpec>(pp, Substitutions));
    }
}
