using System;
using System.Collections.Generic;
using System.Text;

namespace Nom.Language
{
    public class TypeVisitor<Arg, Ret> : ITypeVisitor<Arg, Ret>
    {
        public TypeVisitor()
        {
        }
        private Func<BotType, Arg, Ret> visitBotType;
        public Func<BotType, Arg, Ret> VisitBotType
        {
            get
            {
                return visitBotType?? DefaultAction;
            }
            set
            {
                visitBotType = value;
            }
        }

        private Func<ClassType, Arg, Ret> visitClassType;
        public Func<ClassType, Arg, Ret> VisitClassType
        {
            get
            {
                return visitClassType ?? NamedTypeAction;
            }
            set
            {
                visitClassType = value;
            }
        }

        private Func<InterfaceType, Arg, Ret> visitInterfaceType;
        public Func<InterfaceType, Arg, Ret> VisitInterfaceType
        {
            get
            {
                return visitInterfaceType ?? NamedTypeAction;
            }
            set
            {
                visitInterfaceType = value;
            }
        }

        private Func<TopType, Arg, Ret> visitTopType;
        public Func<TopType, Arg, Ret> VisitTopType
        {
            get
            {
                return visitTopType ?? DefaultAction;
            }
            set
            {
                visitTopType = value;
            }
        }


        private Func<TypeVariable, Arg, Ret> visitTypeVariable;
        public Func<TypeVariable, Arg, Ret> VisitTypeVariable
        {
            get
            {
                return visitTypeVariable ?? DefaultAction;
            }
            set
            {
                visitTypeVariable = value;
            }
        }

        private Func<INamedType, Arg, Ret> visitNamedType;
        public Func<INamedType, Arg, Ret> NamedTypeAction
        {
            get
            {
                return visitNamedType ?? DefaultAction;
            }
            set
            {
                visitNamedType = value;
            }
        }
        
        public Func<IType, Arg, Ret> DefaultAction {
            set; get;
        }

        private Func<DynamicType, Arg, Ret> visitDynamicType;
        public Func<DynamicType, Arg, Ret> VisitDynamicType
        {
            get
            {
                return visitDynamicType ?? DefaultAction;
            }
            set
            {
                visitDynamicType = value;
            }
        }

        private Func<MaybeType, Arg, Ret> visitMaybeType;
        public Func<MaybeType, Arg, Ret> VisitMaybeType
        {
            get
            {
                return visitMaybeType ?? DefaultAction;
            }
            set
            {
                visitMaybeType = value;
            }
        }
        private Func<ProbablyType, Arg, Ret> visitProbablyType;
        public Func<ProbablyType, Arg, Ret> VisitProbablyType
        {
            get
            {
                return visitProbablyType ?? DefaultAction;
            }
            set
            {
                visitProbablyType = value;
            }
        }
    }

    public class NoargTypeVisitor<Ret> : TypeVisitor<object, Ret>
    {
        public NoargTypeVisitor(Func<IType,Ret> defaultAction=null)
        {
            this.DefaultAction = defaultAction;
        }


        new public Func<BotType, Ret> VisitBotType
        {
            set
            {
                base.VisitBotType = (t, o) => value(t);
            }
        }


        new public Func<ClassType, Ret> VisitClassType
        {
            set
            {
                base.VisitClassType = (t, o) => value(t);
            }
        }

        new public Func<InterfaceType, Ret> VisitInterfaceType
        {
            set
            {
                base.VisitInterfaceType = (t, o) => value(t);
            }
        }

        new public Func<TopType, Ret> VisitTopType
        {
            set
            {
                base.VisitTopType = (t, o) => value(t);
            }
        }
        new public Func<TypeVariable, Ret> VisitTypeVariable
        {
            set
            {
                base.VisitTypeVariable = (t, o) => value(t);
            }
        }

        new public Func<INamedType, Ret> NamedTypeAction
        {
            set
            {
                base.NamedTypeAction = (t, o) => value(t);
            }
        }
        new public Func<DynamicType, Ret> VisitDynamicType
        {
            set
            {
                base.VisitDynamicType = (t, o) => value(t);
            }
        }

        new public Func<IType, Ret> DefaultAction
        {
            set
            {
                base.DefaultAction = (t, o) => value(t);
            }
        }
        new public Func<MaybeType, Ret> VisitMaybeType
        {
            set
            {
                base.VisitMaybeType = (t, o) => value(t);
            }
        }
        new public Func<ProbablyType, Ret> VisitProbablyType
        {
            set
            {
                base.VisitProbablyType = (t, o) => value(t);
            }
        }
    }
}
