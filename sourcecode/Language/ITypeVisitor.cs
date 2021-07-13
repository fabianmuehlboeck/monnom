using System;
using System.Collections.Generic;
using System.Text;

namespace Nom.Language
{
    public partial interface ITypeVisitor<in Arg, out Ret>
    {
        Func<BotType, Arg, Ret> VisitBotType
        {
            get;
        }

        Func<ClassType, Arg, Ret> VisitClassType
        {
            get;
        }

        Func<InterfaceType, Arg, Ret> VisitInterfaceType
        {
            get;
        }

        Func<TopType, Arg, Ret> VisitTopType
        {
            get;
        }
        Func<MaybeType, Arg, Ret> VisitMaybeType
        {
            get;
        }
        Func<ProbablyType, Arg, Ret> VisitProbablyType
        {
            get;
        }

        Func<TypeVariable, Arg, Ret> VisitTypeVariable
        {
            get;
        }
    }
}
