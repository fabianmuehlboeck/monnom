using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Nom
{

    public interface IOptional<out T>
    {
        bool HasElem
        {
            get;
        }

        T Elem
        {
            get;
        }

        /// <summary>
        /// If this value is non-empty, the result of applying <paramref name="fun"/> to it
        /// is wrapped in an optional container and returned, else an emtpy optional container.
        /// </summary>
        IOptional<S> Bind<S>(Func<T, S> fun);

        /// <summary>
        /// If this value is non-empty, the result of applying <paramref name="fun"/> to it
        /// is wrapped in an optional container and returned, else the default value is
        /// wrapped in an optional container an returned.
        /// </summary>
        IOptional<S> BindCoalesce<S>(Func<T, S> fun, S defaultValue);
        
        /// <summary>
        /// If this value is non-empty, returns the result of applying <paramref name="fun"/> to it.
        /// Else, returns an empty optional container of the expected type.
        /// </summary>
        /// <typeparam name="S"></typeparam>
        /// <param name="fun"></param>
        /// <returns></returns>
        IOptional<S> Join<S>(Func<T, IOptional<S>> fun);

        /// <summary>
        /// If this value is non-empty, the value is unwrapped and returned, else the default value for
        /// the contained type.
        /// </summary>
        S Extract<S>(Func<T, S> fun, S defaultValue=default(S));

        /// <summary>
        /// If this value is non-empty, <paramref name="action"/> is exectued with the value as argument.
        /// Else, nothing happens.
        /// </summary>
        void ActionBind(Action<T> action);

        /// <summary>
        /// Returns an enumerable with either 0 elements (if empty), or 1 element (the element)
        /// </summary>
        /// <returns></returns>
        IEnumerable<T> AsEnumerable();
    }

    public abstract class Optional<T> : IOptional<T>, IEquatable<Optional<T>>
    {
        public abstract T Elem
        {
            get;
            protected set;
        }

        public abstract bool HasElem
        {
            get;
        }

        public static Optional<T> Empty
        {
            get;
        } = new EmptyOptional();

        public static Optional<T> Inject(T elem)
        {
            return new FilledOptional(elem);
        }

        public IOptional<S> Bind<S>(Func<T, S> fun)
        {
            if (HasElem)
            {
                return Optional<S>.Inject(fun(Elem));
            }
            return Optional<S>.Empty;
        }

        public IOptional<S> Join<S>(Func<T, IOptional<S>> fun)
        {
            if(HasElem)
            {
                return fun(Elem);
            }
            return Optional<S>.Empty;
        }

        public IOptional<S> BindCoalesce<S>(Func<T, S> fun, S defaultValue)
        {
            if (HasElem)
            {
                return fun(Elem).InjectOptional();
            }
            return defaultValue.InjectOptional();
        }

        public void ActionBind(Action<T> action)
        {
            if(HasElem)
            {
                action(Elem);
            }
        }

        public S Extract<S>(Func<T, S> fun, S defaultValue = default(S))
        {
            if(HasElem)
            {
                return fun(Elem);
            }
            return defaultValue;
        }

        public override bool Equals(object obj)
        {
            return Equals(obj as Optional<T>);
        }

        public bool Equals(Optional<T> other)
        {
            return other != null &&
                   EqualityComparer<T>.Default.Equals(Elem, other.Elem);
        }

        public override int GetHashCode()
        {
            return -90071542 + EqualityComparer<T>.Default.GetHashCode(Elem);
        }

        public IEnumerable<T> AsEnumerable()
        {
            if(HasElem)
            {
                yield return Elem;
            }
            yield break;
        }

        private class EmptyOptional : Optional<T>
        {
            public override T Elem
            {
                get
                {
                    throw new IndexOutOfRangeException("Tried to access empty optional value");
                }

                protected set
                {
                }
            }
            public override bool HasElem
            {
                get
                {
                    return false;
                }
            }
        }

        private class FilledOptional:Optional<T>
        {
            public FilledOptional(T elem)
            {
                Elem = elem;
            }

            public override T Elem
            {
                get;
                protected set;
            }

            public override bool HasElem
            {
                get
                {
                    return true;
                }
            }
        }
    }
}
