using System;
using System.Collections.Generic;
using System.Text;
using System.Linq;
using Nom.Language;

namespace Nom.TypeChecker.StdLib
{
    public class StdLib : ILibrary
    {
        public static StdLib Instance = new StdLib();

        public static ClassType ObjectType = new ClassType(Object.Instance, new TypeEnvironment<ITypeArgument>());
        public static ClassType VoidType = new ClassType(Void.Instance, new TypeEnvironment<ITypeArgument>());
        public static ClassType NullType = new ClassType(Null.Instance, new TypeEnvironment<ITypeArgument>());
        public static ClassType BoolType = new ClassType(Bool.Instance, new TypeEnvironment<ITypeArgument>());
        public static ClassType IntType = new ClassType(Int.Instance, new TypeEnvironment<ITypeArgument>());
        public static ClassType FloatType = new ClassType(Float.Instance, new TypeEnvironment<ITypeArgument>());
        public static ClassType TimerType = new ClassType(Timer.Instance, new TypeEnvironment<ITypeArgument>());

        public static InterfaceType GetFunType(IEnumerable<IType> argtypes, IType returnType)
        {
            Fun f = Fun.GetInstance(argtypes.Count());
            return new InterfaceType(f, new TypeEnvironment<ITypeArgument>(f.TypeParameters, argtypes.Snoc(returnType)));
        }

        private StdLib()
        {

        }
        public string Name => "stdlib";

        public Version Version { get; } = new Version();

        public INamespaceSpec GlobalNamespace => StdLibGlobalNamespace.Instance;

        public IEnumerable<IParamRef<INamespaceSpec, P>> FindVarargsChildren<P>(IArgIdentifier<string, P> name) where P : ITypeArgument, ISubstitutable<P>
        {
            if(name.Name=="Fun"&&!Fun.GetInstances().Any(f=>f.TypeParameters.Count()==name.Arguments.Count()))
            {
                var fun = Fun.GetInstance(name.Arguments.Count()-1);
                yield return new InterfaceRef<P>(fun, new TypeEnvironment<P>(fun.TypeParameters, name.Arguments));
            }
            yield break; 
        }
    }
}
