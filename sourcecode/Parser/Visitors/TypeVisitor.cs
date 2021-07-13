using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Nom.Parser
{
    public class TypeVisitor<T> : ITypeVisitor<object, T>
    {
        public TypeVisitor()
        {
            Func<Object, T> nothing = x => default(T);
            ArrayAction = nothing;
            ClassAction = nothing;
            DynamicAction = nothing;
            MaybeAction = nothing;
            ProbablyAction = nothing;
        }
        public TypeVisitor(T def)
        {
            Func<Object,T> nothing = x => def;
            ArrayAction = nothing;
            ClassAction = nothing;
            DynamicAction = nothing;
            MaybeAction = nothing;
            ProbablyAction = nothing;
        }

        public Func<ArrayType,T> ArrayAction
        {
            get;
            set;
        }


        public Func<ClassType, T> ClassAction
        {
            get;
            set;
        }
        public Func<DynamicType, T> DynamicAction
        {
            get;
            set;
        }

        public Func<MaybeType, T> MaybeAction
        {
            get;
            set;
        }
        public Func<ProbablyType, T> ProbablyAction
        {
            get;
            set;
        }

        public Func<ClassType, object, T> VisitClassType => (t, o) => ClassAction(t);

        public Func<ArrayType, object, T> VisitArrayType => (t, o) => ArrayAction(t);

        public Func<DynamicType, object, T> VisitDynamicType => (t, o) => DynamicAction(t);
        public Func<MaybeType, object, T> VisitMaybeType => (t, o) => MaybeAction(t);
        public Func<ProbablyType, object, T> VisitProbablyType => (t, o) => ProbablyAction(t);
    }
}
