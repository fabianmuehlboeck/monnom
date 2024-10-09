using Nom.Language;
using Nom.Language.SpecExtensions;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Nom.TypeChecker
{
    public class CFunctionRegistry
    {
        private CFunctionRegistry() { }
        private static CFunctionRegistry instance = new CFunctionRegistry();
        public static CFunctionRegistry Instance { get { return instance; } }

        private List<ITDCFunction> cfunctions = new List<ITDCFunction>();

        public ITDCFunction GetCFunction(string libraryName, string functionName, ITypeParametersSpec typeParameters, IParametersSpec argTypes, IType returnType)
        {
            foreach (ITDCFunction function in cfunctions)
            {
                if (function.LibraryName == libraryName && function.FunctionName == functionName && function.TypeParameters.Count() == typeParameters.Count() && function.Parameters.Entries.Count() == argTypes.Entries.Count())
                {
                    bool match = true;
                    TypeEnvironment<Language.IType> env = new TypeEnvironment<IType>(typeParameters, function.TypeParameters.Select(tps => new TypeVariable(tps)));
                    if (!function.ReturnType.IsEquivalent(((ISubstitutable<IType>)returnType).Substitute(env)))
                    {
                        match = false;
                        CompilerOutput.Warn("Incompatible return type definitions for matching C function signatures: %0 vs %1", function.ReturnType, returnType); 
                    }
                    for (int i = 0; i < typeParameters.Count(); i++)
                    {
                        if (!((ISubstitutable<IType>)(typeParameters.ElementAt(i).LowerBound)).Substitute(env).IsEquivalent(function.TypeParameters.ElementAt(i).LowerBound))
                        {
                            match = false;
                            CompilerOutput.Warn("Incompatible type argument lower bound for matching C function signatures: %0 vs %1", function.TypeParameters.ElementAt(i).LowerBound, typeParameters.ElementAt(i).LowerBound);
                        }
                        if (!((ISubstitutable<IType>)(typeParameters.ElementAt(i).UpperBound)).Substitute(env).IsEquivalent(function.TypeParameters.ElementAt(i).UpperBound))
                        {
                            match = false;
                            CompilerOutput.Warn("Incompatible type argument upper bound for matching C function signatures: %0 vs %1", function.TypeParameters.ElementAt(i).UpperBound, typeParameters.ElementAt(i).UpperBound);
                        }
                    }
                    for(int i=0; i< argTypes.Entries.Count(); i++)
                    {
                        if(!((ISubstitutable<IType>)argTypes.Entries.ElementAt(i).Type).Substitute(env).IsEquivalent(function.Parameters.Entries.ElementAt(i).Type))
                        {
                            match = false;
                            CompilerOutput.Warn("Incompatible argument type definitions for matching C function signatures: %0 vs %1", function.Parameters.Entries.ElementAt(i).Type, argTypes.Entries.ElementAt(i).Type);
                        }
                    }
                    if(match)
                    {
                        return function;
                    }
                }
            }
            TDCFunction tdcf = new TDCFunction(libraryName, functionName, typeParameters, argTypes, returnType);
            cfunctions.Add(tdcf);
            return tdcf;
        }

        private class TDCFunction : ITDCFunction
        {
            public TDCFunction(string libraryName, string functionName, ITypeParametersSpec typeParameters, IParametersSpec argTypes, IType returnType)
            {
                LibraryName = libraryName;
                FunctionName = functionName;
                TypeParameters = typeParameters;
                ReturnType = returnType;
                Parameters = argTypes;
            }
            public string LibraryName { get; }

            public string FunctionName { get; }

            public IParametersSpec Parameters { get; }

            public IType ReturnType { get; }

            public IOptional<IParameterizedSpec> ParameterizedParent => Optional<IParameterizedSpec>.Empty;

            public ITypeParametersSpec TypeParameters { get; }

            public int OverallTypeParameterCount => TypeParameters.Count();

            public ITypeParametersSpec AllTypeParameters => TypeParameters;

            public IParameterizedSpecRef<IParameterizedSpec> GetAsRef()
            {
                throw new InvalidOperationException();
            }

        }
    }
}
