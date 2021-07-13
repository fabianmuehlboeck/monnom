using System;
using System.Collections.Generic;
using System.Text;
using Nom.Language;

namespace Nom.TypeChecker
{
    public interface IClass : IClassSpec
    {
        new IEnumerable<IConstructorDef> Constructors { get;}
        new IEnumerable<IStaticMethodDef> StaticMethods { get; }
        new IEnumerable<IFieldDecl> Fields { get; }
        new IEnumerable<IMethodDef> Methods { get; }

        IEnumerable<ITDLambda> Lambdas { get; }
        IEnumerable<ITDStruct> Structs { get; }

        new IEnumerable<IStaticFieldDecl> StaticFields { get; }

        new IEnumerable<IInterface> Interfaces { get; }
        new IEnumerable<IClass> Classes { get; }

    }
}
