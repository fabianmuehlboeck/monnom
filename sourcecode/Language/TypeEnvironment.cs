using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Nom.Language
{
    public interface ITypeEnvironment<T> : IEnumerable<KeyValuePair<ITypeParameterSpec, T>> where T : ITypeArgument, ISubstitutable<T>
    {
        ITypeEnvironment<T> Push<TX>(IEnumerable<ITypeParameterSpec> specs, IEnumerable<TX> values) where TX : T;

        ITypeEnvironment<P> Transform<P>(Func<T, P> transformer) where P : ITypeArgument, ISubstitutable<P>;
        T this[ITypeParameterSpec key] { get; }

        IType GetType(ITypeParameterSpec spec);

        bool Contains(ITypeParameterSpec spec);
        ITypeEnvironment<T> Flip();
        ITypeEnvironment<T> Meet(ITypeEnvironment<T> other);
        ITypeEnvironment<T> Substitute(ITypeParameterSpec spec, IType type);
        ITypeEnvironment<T> Substitute<TX>(ITypeEnvironment<TX> env) where TX : ITypeArgument, ISubstitutable<TX>;

        ITypeEnvironment<T> Restrict(IParameterizedSpec spec);
    }

    public class TypeEnvironment<T> : ITypeEnvironment<T> where T : ITypeArgument, ISubstitutable<T>
    {
        private ITypeEnvironment<T> parent;
        private IDictionary<ITypeParameterSpec, T> dict;

        public T this[ITypeParameterSpec key]
        {
            get
            {
                if(dict.ContainsKey(key))
                {
                    return dict[key];
                }
                if(parent!=null)
                {
                    return parent[key];
                }
                throw new InternalException("No entry for type variable");
            }
        }

        public TypeEnvironment(IEnumerable<ITypeParameterSpec> specs, IEnumerable<T> values, ITypeEnvironment<T> parent = null)
        {
            this.parent = parent;
            dict = new Dictionary<ITypeParameterSpec, T>(specs.Zip(values, (k, v) => new KeyValuePair<ITypeParameterSpec, T>(k, v)));
        }
        public TypeEnvironment(IDictionary<ITypeParameterSpec, T> dict =null, ITypeEnvironment<T> parent = null)
        {
            this.parent = parent;
            this.dict = dict??new Dictionary<ITypeParameterSpec, T>();
        }

        public ITypeEnvironment<T> Push<TX>(IEnumerable<ITypeParameterSpec> specs, IEnumerable<TX> values) where TX : T
        {
            return new TypeEnvironment<T>(specs, values.Cast<T>(), this);
        }

        public IEnumerator<KeyValuePair<ITypeParameterSpec, T>> GetEnumerator()
        {
            if (parent == null)
            {
                return dict.GetEnumerator();
            }
            else
            {
                return dict.Concat(parent).GetEnumerator();
            }
        }
        
        IEnumerator IEnumerable.GetEnumerator()
        {
            return GetEnumerator();
        }


        public ITypeEnvironment<P> Transform<P>(Func<T, P> transformer) where P : ITypeArgument, ISubstitutable<P>
        {
            return new TypeEnvironment<P>(new Dictionary<ITypeParameterSpec, P>(dict.Select(kvp => new KeyValuePair<ITypeParameterSpec, P>(kvp.Key, transformer(kvp.Value)))), parent?.Transform(transformer));
        }

        public IType GetType(ITypeParameterSpec key)
        {
            return this[key].AsType;
        }
        public bool Contains(ITypeParameterSpec spec)
        {
            if(dict.ContainsKey(spec))
            {
                return true;
            }
            if(parent!=null)
            {
                return parent.Contains(spec);
            }
            return false;
        }

        private ITypeEnvironment<T> flippedMe = null;
        public ITypeEnvironment<T> Flip()
        {
            if(flippedMe==null)
            {
                flippedMe = new FlippedTypeEnvironment(this);
            }
            return flippedMe;
        }

        public ITypeEnvironment<T> Meet(ITypeEnvironment<T> other)
        {
            throw new NotImplementedException();
        }

        public ITypeEnvironment<T> Substitute(ITypeParameterSpec spec, IType type)
        {
            return new TypeEnvironment<T>(new Dictionary<ITypeParameterSpec, T>(dict.Select(kvp => new KeyValuePair<ITypeParameterSpec, T>(kvp.Key, ((ISubstitutable<T>)kvp.Value).Substitute(spec, type)))), parent?.Substitute(spec, type));
        }

        public ITypeEnvironment<T> Substitute<TX>(ITypeEnvironment<TX> env) where TX : ITypeArgument, ISubstitutable<TX>
        {
            return new TypeEnvironment<T>(new Dictionary<ITypeParameterSpec, T>(dict.Select(kvp => new KeyValuePair<ITypeParameterSpec, T>(kvp.Key, ((ISubstitutable<T>)kvp.Value).Substitute(env)))), parent?.Substitute(env));
        }

        public ITypeEnvironment<T> Restrict(IParameterizedSpec spec)
        {
            return new TypeEnvironment<T>(new Dictionary<ITypeParameterSpec, T>(dict.Where(kvp => spec.AllTypeParameters.Contains(kvp.Key))), parent?.Restrict(spec));
        }

        private class FlippedTypeEnvironment : ITypeEnvironment<T>
        {
            private ITypeEnvironment<T> parent;
            public FlippedTypeEnvironment(ITypeEnvironment<T> parent)
            {
                this.parent = parent;
            }

            public T this[ITypeParameterSpec key] => parent[key];

            public bool Contains(ITypeParameterSpec spec)
            {
                return parent.Contains(spec);
            }
            public ITypeEnvironment<T> Flip()
            {
                return parent;
            }

            public IType GetType(ITypeParameterSpec key)
            {
                return parent.GetType(key);
            }

            public IEnumerator<KeyValuePair<ITypeParameterSpec, T>> GetEnumerator()
            {
                return parent.GetEnumerator();
            }

            public ITypeEnvironment<T> Meet(ITypeEnvironment<T> other)
            {
                return parent.Meet(other);
            }

            public ITypeEnvironment<T> Push<TX>(IEnumerable<ITypeParameterSpec> specs, IEnumerable<TX> values) where TX : T
            {
                return parent.Push(specs, values);
            }

            public ITypeEnvironment<T> Substitute(ITypeParameterSpec spec, IType type)
            {
                return parent.Substitute(spec, type);
            }

            public ITypeEnvironment<T> Substitute<TX>(ITypeEnvironment<TX> env) where TX : ITypeArgument, ISubstitutable<TX>
            {
                return parent.Substitute(env);
            }

            public ITypeEnvironment<P> Transform<P>(Func<T, P> transformer) where P : ITypeArgument, ISubstitutable<P>
            {
                return parent.Transform(transformer);
            }

            IEnumerator IEnumerable.GetEnumerator()
            {
                return parent.GetEnumerator();
            }


            public ITypeEnvironment<T> Restrict(IParameterizedSpec spec)
            {
                return new FlippedTypeEnvironment(parent.Restrict(spec));
            }
        }
    }

}
