using System;
using System.Collections.Generic;
using System.Text;

namespace Nom.Language
{
    public interface IClassSpec : IInterfaceSpec
    {
        IOptional<IParamRef<IClassSpec, IType>> SuperClass
        {
            get;
        }
        bool IsFinal
        {
            get;
        }
        bool IsPartial
        {
            get;
        }
        IEnumerable<IStaticMethodSpec> StaticMethods
        {
            get;
        }

        /// <summary>
        /// All fields of this class, including non-private inherited ones from superclasses
        /// </summary>
        IEnumerable<IFieldSpec> Fields
        {
            get;
        }

        /// <summary>
        /// Static fields of this class (no inheritance)
        /// </summary>
        IEnumerable<IStaticFieldSpec> StaticFields
        {
            get;
        }

        IEnumerable<IConstructorSpec> Constructors
        {
            get;
        }

        IEnumerable<IInstanceSpec> Instances
        {
            get;
        }
        IParamRef<IClassSpec, IType> MakeClassRef(ITypeEnvironment<IType> env);
    }
}
