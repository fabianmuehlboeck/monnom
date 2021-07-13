using System;
using System.Collections.Generic;
using System.Text;
using System.Linq;
using Nom.Language;

namespace Nom.TypeChecker
{
    internal class TDInterface : ATDNamed<TDInterfaceDef>, ITDInterface
    {
        public bool IsInheritanceInitialized
        {
            get
            {
                return this.Definitions.All(d => d.IsInheritanceInitialized);
            }
        }

        public TDInterface(Program program, string name, IEnumerable<TDTypeArgDecl> arguments, IOptional<ITDChild> parentNamespace):base(program, name, arguments, parentNamespace)
        {
        }



        public override Ret Visit<Arg, Ret>(INamespaceSpecVisitor<Arg, Ret> visitor, Arg arg = default(Arg))
        {
            return visitor.VisitInterfaceSpec(this, arg);
        }


        public override Ret Visit<Arg, Ret>(ITDChildVisitor<Arg, Ret> visitor, Arg arg = default(Arg))
        {
            return visitor.VisitInterface(this, arg);
        }

        public override TDLambda CreateLambda(ITypeParametersSpec typeParameters, IParametersSpec parameters, IType returnType)
        {
            throw new InternalException("It should be impossible to create a lambda within an interface");
        }
        public override TDStruct CreateStruct()
        {
            throw new InternalException("It should be impossible to create a struct within an interface");
        }
        public override INamedType Instantiate(ITypeEnvironment<ITypeArgument> args)
        {
            return new InterfaceType(this, args);
        }
    }
}
