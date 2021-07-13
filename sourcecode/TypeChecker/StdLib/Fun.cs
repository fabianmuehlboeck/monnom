using System;
using System.Collections.Generic;
using System.Text;
using System.Linq;
using Nom.Language;

namespace Nom.TypeChecker.StdLib
{
    public class Fun : AStdInterface
    {
        internal readonly MethodDeclDef InvokeMethod;
        private static Dictionary<int, Fun> instances = new Dictionary<int, Fun>();

        public static IEnumerable<Fun> GetInstances()
        {
            return instances.Values;
        }

        public static Fun GetInstance(int argcount)
        {
            if (!instances.ContainsKey(argcount))
            {
                instances.Add(argcount, new Fun(argcount));
            }
            return instances[argcount];
        }
        private Fun(int argcount) : base("Fun")
        {
            List<TDTypeArgDecl> typeArgs = new List<TDTypeArgDecl>();
            List<ParameterSpec> parameters = new List<ParameterSpec>();
            for (int i = 0; i < argcount; i++)
            {
                typeArgs.Add(new TDTypeArgDecl("Arg" + i.ToString(), i) { Parent = this });
                parameters.Add(new ParameterSpec("arg" + i.ToString(), new TypeVariable(typeArgs[i])));
            }
            typeArgs.Add(new TDTypeArgDecl("Ret",  argcount) { Parent = this });
            TypeParameters = new TypeParametersSpec(typeArgs);
            InvokeMethod = new MethodDeclDef(new Parser.Identifier(""), new TypeParametersSpec(new List<ITypeParameterSpec>()), new ParametersSpec(parameters), new TypeVariable(typeArgs.Last()), Visibility.Public, this);
        }

        public override bool IsShape => false;

        public override IEnumerable<IMethodSpec> Methods
        {
            get
            {
                yield return InvokeMethod;
                yield break;
            }
        }

        public override ITypeParametersSpec TypeParameters { get; }
    }
}
