using System;
using System.Collections.Generic;
using System.Text;
using System.Linq;

using Nom.Language;
using Nom.Language.SpecExtensions;

namespace Nom.TypeChecker
{
    public static class TypeExtensions
    {

        #region Method Lookup
        public static IOptional<IInstanceMethodReference> GetInstanceMethodReference(this IType type, string methodName, IEnumerable<Language.IType> typeArguments, IEnumerable<Language.IType> argumentTypes, ITypeCheckLookup<Language.IType, Language.ITypeArgument> context)
        {
            return type.Visit(new MethodReferenceVisitor(methodName, Visibility.Public, typeArguments, argumentTypes), context.TypeEnvironment.Transform<Language.ITypeArgument>(t => t));
        }

        private class MethodReferenceVisitor : ITypeVisitor<ITypeEnvironment<ITypeArgument>, IOptional<IInstanceMethodReference>>
        {
            public MethodReferenceVisitor(string methodName, Visibility visibility, IEnumerable<Language.IType> typeArguments, IEnumerable<Language.IType> argumentTypes)
            {
                this.methodName = methodName;
                this.visibility = visibility;
                this.typeArguments = typeArguments;
                this.argumentTypes = argumentTypes;
            }

            private readonly String methodName;
            private readonly Visibility visibility;
            IEnumerable<Language.IType> typeArguments;
            IEnumerable<Language.IType> argumentTypes;
            public Func<DynamicType, ITypeEnvironment<ITypeArgument>, IOptional<IInstanceMethodReference>> VisitDynamicType => (dyn, env) =>
            {
                return new DynamicInstanceMethodReference(methodName, typeArguments).InjectOptional();
            };

            public Func<BotType, ITypeEnvironment<ITypeArgument>, IOptional<IInstanceMethodReference>> VisitBotType => (bot, env) =>
            {
                return new BottomInstanceMethodReference().InjectOptional();
            };

            public Func<ClassType, ITypeEnvironment<ITypeArgument>, IOptional<IInstanceMethodReference>> VisitClassType => (cls, env) =>
            {
                return FilterMethodList(cls.Class.Methods, env.Push(cls.Substitutions));
            };

            public Func<InterfaceType, ITypeEnvironment<ITypeArgument>, IOptional<IInstanceMethodReference>> VisitInterfaceType => (iface, env) =>
            {
                return FilterMethodList(iface.Element.Methods, env.Push(iface.Substitutions));
            };

            private IOptional<IInstanceMethodReference> FilterMethodList(IEnumerable<IMethodSpec> methods, ITypeEnvironment<ITypeArgument> env)
            {
                List<IInstanceMethodReference> references = new List<IInstanceMethodReference>();
                foreach (var method in methods)
                {
                    if (method.Name != methodName)
                    {
                        continue;
                    }
                    if (method.Visibility < visibility)
                    {
                        continue;
                    }
                    if (method.TypeParameters.Count() != typeArguments.Count())
                    {
                        continue;
                    }
                    if (method.Parameters.Entries.Count() != argumentTypes.Count())
                    {
                        continue;
                    }
                    bool validTypeArgs = true;
                    foreach (var (tps, targ) in method.TypeParameters.Zip(typeArguments, (x, y) => (x, y)))
                    {
                        if (!(targ.IsSubtypeOf(tps.UpperBound) && targ.IsSupertypeOf(tps.LowerBound)))
                        {
                            validTypeArgs = false;
                            break;
                        }
                    }
                    if (!validTypeArgs)
                    {
                        continue;
                    }
                    bool validArgs = true;
                    bool pessimisticArgs = true;
                    List<Language.IType> formalTypes = new List<IType>();
                    foreach (var (paramType, argType) in method.Parameters.Entries.Zip(argumentTypes, (x, y) => (x, y)))
                    {
                        var formalType = ((ISubstitutable<Language.IType>)paramType.Type).Substitute(env);
                        formalTypes.Add(formalType);
                        if (!argType.IsSubtypeOf(formalType, false))
                        {
                            pessimisticArgs = false;
                        }
                        if (!argType.IsSubtypeOf(formalType, true))
                        {
                            validArgs = false;
                            break;
                        }
                    }
                    if (!validArgs)
                    {
                        continue;
                    }
                    if (pessimisticArgs)
                    {
                        references.Add(new CheckedInstanceMethodReference(method, typeArguments, formalTypes, env));
                    }
                    else
                    {
                        references.Add(new OptimisticInstanceMethodReference(method, typeArguments, formalTypes, env));
                    }
                }
                if (references.Count == 1)
                {
                    return references[0].InjectOptional();
                }
                if (references.Count == 0)
                {
                    return Optional<IInstanceMethodReference>.Empty;
                }
                throw new NotImplementedException();
            }

            public Func<TopType, ITypeEnvironment<ITypeArgument>, IOptional<IInstanceMethodReference>> VisitTopType => (top, env) =>
            {
                return Optional<IInstanceMethodReference>.Empty;
            };

            public Func<TypeVariable, ITypeEnvironment<ITypeArgument>, IOptional<IInstanceMethodReference>> VisitTypeVariable => (tv, env) => tv.ParameterSpec.UpperBound.Visit(this, env);

            public Func<MaybeType, ITypeEnvironment<ITypeArgument>, IOptional<IInstanceMethodReference>> VisitMaybeType => (mb, env) => Optional<IInstanceMethodReference>.Empty;

            public Func<ProbablyType, ITypeEnvironment<ITypeArgument>, IOptional<IInstanceMethodReference>> VisitProbablyType => (pb, env) =>
                pb.PotentialType.Visit(this, env).Bind(mr => new ProbablyInstanceMethodReference(mr, pb.PotentialType));
        }

        private class ProbablyInstanceMethodReference : IInstanceMethodReference
        {
            public readonly IInstanceMethodReference Wrapped;
            public readonly IType TargetType;
            public ProbablyInstanceMethodReference(IInstanceMethodReference wrapped, IType targetType)
            {
                Wrapped = wrapped;
                TargetType = targetType;
            }
            public ICallReceiverTransformResult GenerateCall(IExprTransformResult receiver, IEnumerable<IExprTransformResult> arguments, ICodeTransformEnvironment env)
            {
                return Wrapped.GenerateCall(receiver.EnsureType(TargetType, env), arguments, env);
            }
        }

        private class DynamicInstanceMethodReference : IInstanceMethodReference
        {
            public readonly String MethodName;
            public readonly IEnumerable<Language.ITypeArgument> TypeArguments;
            public DynamicInstanceMethodReference(string methodName, IEnumerable<Language.ITypeArgument> typeArguments)
            {
                MethodName = methodName;
                TypeArguments = typeArguments;
            }
            public ICallReceiverTransformResult GenerateCall(IExprTransformResult receiver, IEnumerable<IExprTransformResult> arguments, ICodeTransformEnvironment env)
            {
                List<IInstruction> instructions = receiver.Snoc(new EnsureDynamicMethodInstruction(receiver.Register,MethodName)).Concat(arguments.Flatten()).ToList();
                CallExpandoMethodInstruction cemi = new CallExpandoMethodInstruction(receiver.Register, MethodName, TypeArguments.Select(targ => targ.AsType), arguments.Select(arg => arg.Register), env.CreateRegister());
                instructions.Add(cemi);
                return new CallReceiverTransformResult(new DynamicType(), cemi.Register, instructions);
            }
        }

        private class CheckedInstanceMethodReference : IInstanceMethodReference
        {
            public readonly IEnumerable<Language.ITypeArgument> TypeArguments;
            public readonly IMethodSpec Method;
            public readonly ITypeEnvironment<Language.ITypeArgument> Substitutions;
            public CheckedInstanceMethodReference(IMethodSpec method, IEnumerable<Language.ITypeArgument> typeArguments, IEnumerable<Language.IType> paramTypes, ITypeEnvironment<Language.ITypeArgument> substitutions)
            {
                TypeArguments = typeArguments;
                Substitutions = substitutions;
                Method = method;
            }
            public IInstanceMethodReference OptimisticVersion => throw new NotImplementedException();

            public ICallReceiverTransformResult GenerateCall(IExprTransformResult receiver, IEnumerable<IExprTransformResult> arguments, ICodeTransformEnvironment env)
            {
                List<IInstruction> instructions = receiver.Snoc(new EnsureCheckedMethodInstruction(receiver.Register, Method.Name)).ToList(); 
                List<IRegister> argRegs = new List<IRegister>();
                foreach ((IExprTransformResult, Language.IType) argpair in arguments.Zip(Method.Parameters.Entries.Select(ps => ((ISubstitutable<Language.IType>)ps.Type).Substitute(Substitutions)), (x, y) => (x, y)))
                {
                    IExprTransformResult actualArgument = argpair.Item1.EnsureType(argpair.Item2, env);
                    instructions.AddRange(actualArgument);
                    argRegs.Add(actualArgument.Register);

                }
                CallInstanceMethodCheckedInstruction cmi = new CallInstanceMethodCheckedInstruction(Method.GetAsTypedRef().Substitute(Substitutions), receiver.Register, argRegs, env.CreateRegister());
                return new CallReceiverTransformResult(((ISubstitutable<Language.IType>)Method.ReturnType).Substitute(Substitutions), cmi.Register, instructions.Snoc(cmi));
            }
        }
        private class OptimisticInstanceMethodReference : IInstanceMethodReference
        {
            public readonly IEnumerable<Language.ITypeArgument> TypeArguments;
            public readonly IMethodSpec Method;
            public readonly ITypeEnvironment<Language.ITypeArgument> Substitutions;
            public readonly IEnumerable<Language.IType> ParamTypes;
            public OptimisticInstanceMethodReference(IMethodSpec method, IEnumerable<Language.ITypeArgument> typeArguments, IEnumerable<Language.IType> paramTypes, ITypeEnvironment<Language.ITypeArgument> substitutions)
            {
                TypeArguments = typeArguments;
                Substitutions = substitutions;
                Method = method;
                ParamTypes = paramTypes;
            }
            public IInstanceMethodReference OptimisticVersion => throw new NotImplementedException();

            public ICallReceiverTransformResult GenerateCall(IExprTransformResult receiver, IEnumerable<IExprTransformResult> arguments, ICodeTransformEnvironment env)
            {
                List<IInstruction> instructions = receiver.Snoc(new EnsureCheckedMethodInstruction(receiver.Register, Method.Name)).ToList();
                List<IRegister> argRegs = new List<IRegister>();
                foreach ((IExprTransformResult, Language.IType) argpair in arguments.Zip(Method.Parameters.Entries.Select(ps => ((ISubstitutable<Language.IType>)ps.Type).Substitute(Substitutions)), (x, y) => (x, y)))
                {
                    IExprTransformResult actualArgument = argpair.Item1.EnsureType(argpair.Item2, env);
                    instructions.AddRange(actualArgument);
                    argRegs.Add(actualArgument.Register);

                }
                CallInstanceMethodCheckedInstruction cmi = new CallInstanceMethodCheckedInstruction(Method.GetAsTypedRef().Substitute(Substitutions), receiver.Register, argRegs, env.CreateRegister());
                return new CallReceiverTransformResult(((ISubstitutable<Language.IType>)Method.ReturnType).Substitute(Substitutions), cmi.Register, instructions.Snoc(cmi));
            }
        }
        private class BottomInstanceMethodReference : IInstanceMethodReference
        {
            public IInstanceMethodReference OptimisticVersion => throw new NotImplementedException();

            public ICallReceiverTransformResult GenerateCall(IExprTransformResult receiver, IEnumerable<IExprTransformResult> arguments, ICodeTransformEnvironment env)
            {
                return new CallReceiverTransformResult(BotType.Instance, env.CreateRegister(), new DebugInstruction("This code should never be reached!").Singleton());
            }
        }

        #endregion
    }
}
