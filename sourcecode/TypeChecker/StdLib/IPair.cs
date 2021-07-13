using System;
using System.Collections.Generic;
using System.Text;
using Nom.Language;

namespace Nom.TypeChecker.StdLib
{
    class IPair : AStdInterface
    {
        private MethodDeclDef FstMethod;
        private MethodDeclDef SndMethod;
        private static Dictionary<int, Fun> instances = new Dictionary<int, Fun>();
        public static IPair Instance { get; } = new IPair();
        private IPair() : base("IPair")
        {
            var fstt = new TDTypeArgDecl("Fst", 0) { Parent = this };
            var sndt = new TDTypeArgDecl("Snd", 1) { Parent = this };
            TypeParameters = new TypeParametersSpec(new List<TDTypeArgDecl>() { fstt, sndt });

            FstMethod = new MethodDeclDef(new Parser.Identifier("Fst"), new TypeParametersSpec(new List<ITypeParameterSpec>()), new ParametersSpec(new List<ParameterSpec>() { }), new TypeVariable(fstt), Visibility.Public, this);
            SndMethod = new MethodDeclDef(new Parser.Identifier("Snd"), new TypeParametersSpec(new List<ITypeParameterSpec>()), new ParametersSpec(new List<ParameterSpec>() { }), new TypeVariable(sndt), Visibility.Public, this);
        }

        public override IEnumerable<IMethodSpec> Methods
        {
            get
            {
                yield return FstMethod;
                yield return SndMethod;
                yield break;
            }
        }

        public override ITypeParametersSpec TypeParameters { get; }
        public override bool IsShape => false;
    }
}
