using System;
using System.Collections.Generic;
using System.Text;
using Nom.Language;
using Nom.Parser;
using System.Linq;
using Nom.TypeChecker;
using System.IO;

namespace Nom.BenchmarkGenerator
{
    public static class ExtensionMethods
    {
        public static Parser.IType ToParserType(this Language.IType type, ISourceSpan locs)
        {
            var visitor = new Language.TypeVisitor<object, Parser.IType>();
            visitor.DefaultAction = (tt, o) => { throw new InternalException("No other types implemented in parser"); };
            visitor.VisitTypeVariable = (tv, o) => Parser.ClassType.GetInstance(tv.ParameterSpec.Name, locs);
            visitor.VisitDynamicType = (dt, o) => new Parser.DynamicType(locs);
            visitor.VisitTopType = (tt, o) => Parser.ClassType.GetInstance("Anything", locs);
            visitor.VisitBotType = (tt, o) => Parser.ClassType.GetInstance("Nothing", locs);
            visitor.VisitMaybeType = (tt, o) => new Parser.MaybeType(tt.PotentialType.ToParserType(locs));
            visitor.VisitProbablyType = (tt, o) => new Parser.ProbablyType(tt.PotentialType.ToParserType(locs));
            visitor.NamedTypeAction = (nt, o) =>
            {
                IParameterizedSpecRef<IParameterizedSpec> nsr = nt;
                var nsre = nsr.Element as INamespaceSpec;
                List<IArgIdentifier<Identifier, Parser.IType>> ids = new List<IArgIdentifier<Identifier, Parser.IType>>();
                ids.Insert(0, new Parser.TypeIdentifier(new Identifier(nsre.Name, locs), nsr.Element.TypeParameters.Select(tps=>nsr.Substitutions[tps].AsType.Visit(visitor)), locs));
                while (nsr.ParameterizedParent.HasElem)
                {
                    nsr = nsr.ParameterizedParent.Elem as IParameterizedSpecRef<IParameterizedSpec>;
                    nsre = nsr.Element as INamespaceSpec;
                    if (nsre.Name.Length > 0)
                    {
                        ids.Insert(0, new Parser.TypeIdentifier(new Identifier(nsre.Name, locs), nsr.Element.TypeParameters.Select(tps => nsr.Substitutions[tps].AsType.Visit(visitor)), locs));
                    }
                }
                return Parser.ClassType.GetInstance(new TypeQName(false, ids, locs));
            };
            var ret = type.Visit(visitor);
            ret.Annotation = type;
            return ret;
        }

        public static IEnumerable<IClassSpec> AllClasses(this INamespaceSpec ns)
        {
            foreach (IClassSpec cls in ns.Classes)
            {
                yield return cls;
                foreach (IClassSpec cls2 in cls.AllClasses())
                {
                    yield return cls2;
                }
            }
            foreach (INamespaceSpec ns2 in ns.Namespaces)
            {
                foreach (IClassSpec cls2 in ns2.AllClasses())
                {
                    yield return cls2;
                }
            }
        }
        public static IEnumerable<ITDStruct> AllStructs(this TypeChecker.INamespace ns)
        {
            foreach (TypeChecker.IClass cls in ns.Classes)
            {
                foreach (ITDStruct strct in cls.AllStructs())
                {
                    yield return strct;
                }
            }
            foreach (TypeChecker.INamespace ns2 in ns.Namespaces)
            {
                foreach (ITDStruct strct in ns2.AllStructs())
                {
                    yield return strct;
                }
            }
        }

        public static IEnumerable<ITDStruct> AllStructs(this TypeChecker.IClass cls)
        {
            foreach (ITDStruct strct in cls.Structs)
            {
                yield return strct;
            }
            foreach (TypeChecker.IClass cls2 in cls.Classes)
            {
                foreach (ITDStruct strct in cls2.AllStructs())
                {
                    yield return strct;
                }
            }
        }
        public static IEnumerable<ITDLambda> AllLambdas(this TypeChecker.INamespace ns)
        {
            foreach (TypeChecker.IClass cls in ns.Classes)
            {
                foreach (ITDLambda lam in cls.AllLambdas())
                {
                    yield return lam;
                }
            }
            foreach (TypeChecker.INamespace ns2 in ns.Namespaces)
            {
                foreach (ITDLambda lam in ns2.AllLambdas())
                {
                    yield return lam;
                }
            }
        }

        public static IEnumerable<ITDLambda> AllLambdas(this TypeChecker.IClass cls)
        {
            foreach (ITDLambda lam in cls.Lambdas)
            {
                yield return lam;
            }
            foreach (TypeChecker.IClass cls2 in cls.Classes)
            {
                foreach (ITDLambda lam in cls2.AllLambdas())
                {
                    yield return lam;
                }
            }
        }


        public static IEnumerable<IInterfaceSpec> AllInterfaces(this INamespaceSpec ns)
        {
            foreach (IClassSpec cls in ns.Classes)
            {
                foreach (IInterfaceSpec ifc2 in cls.AllInterfaces())
                {
                    yield return ifc2;
                }
            }
            foreach (INamespaceSpec ns2 in ns.Namespaces)
            {
                foreach (IInterfaceSpec ifc2 in ns2.AllInterfaces())
                {
                    yield return ifc2;
                }
            }
            foreach (IInterfaceSpec ifc in ns.Interfaces)
            {
                yield return ifc;
                foreach (IInterfaceSpec ifc2 in ifc.AllInterfaces())
                {
                    yield return ifc2;
                }
            }
        }

        public static IEnumerable<IEnumerable<T>> PowersetWithoutEmpty<T>(this IEnumerable<T> e)
        {
            var lst = e.ToList();
            if(lst.Any())
            {
                yield return new List<T>() { lst[0] };
                foreach(var rest in lst.Skip(1).PowersetWithoutEmpty())
                {
                    yield return rest;
                    yield return rest.Cons(lst[0]);
                }
            }
            yield break;
        }


        public static void Output(this Parser.Program program, DirectoryInfo subdir)
        {
            foreach (Parser.CodeFile cf in program.CodeFiles)
            {
                FileInfo fi = new FileInfo(subdir.FullName + "/" + cf.FileName);
                if (fi.Exists)
                {
                    fi.Delete();
                }
                using (var pp = new PrettyPrinter(fi))
                {
                    cf.PrettyPrint(pp);
                }
            }
        }

    }
}
