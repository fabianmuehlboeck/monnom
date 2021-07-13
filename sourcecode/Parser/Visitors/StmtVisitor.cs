using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Nom.Parser
{
    public class StmtVisitor<Arg, Ret> : IStmtVisitor<Arg, Ret>
    {
        private Func<IStmt, Arg, Ret> defaultVisitor;

        public virtual Func<IStmt, Arg, Ret> DefaultVisitor
        {
            get
            {
                return defaultVisitor;
            }

            set
            {
                defaultVisitor = value;
            }
        }

        private Func<AssignStmt, Arg, Ret> visitAssignStmt;

        private Func<Block, Arg, Ret> visitBlock;

        private Func<BreakStmt, Arg, Ret> visitBreakStmt;

        private Func<ContinueStmt, Arg, Ret> visitContinueStmt;

        private Func<DebugStmt, Arg, Ret> visitDebugStmt;

        private Func<DeclStmt, Arg, Ret> visitDeclStmt;

        private Func<ErrorStmt, Arg, Ret> visitErrorStmt;

        private Func<ExprStmt, Arg, Ret> visitExprStmt;

        private Func<ForeachStmt, Arg, Ret> visitForeachStmt;

        private Func<IfStmt, Arg, Ret> visitIfStmt;

        private Func<IfTypeStmt, Arg, Ret> visitIfTypeStmt;

        private Func<ReturnStmt, Arg, Ret> visitReturnStmt;

        private Func<SpecialStmt, Arg, Ret> visitSpecialStmt;


        private Func<WhileStmt, Arg, Ret> visitWhileStmt;
        private Func<RTCmdStmt, Arg, Ret> visitRTCmdStmt;
        private Func<IfNullStmt, Arg, Ret> visitIfNullStmt;
        private Func<IfObjStmt, Arg, Ret> visitIfObjStmt;

        public virtual Func<AssignStmt, Arg, Ret> VisitAssignStmt
        {
            get
            {
                return visitAssignStmt??DefaultVisitor;
            }

            set
            {
                visitAssignStmt = value;
            }
        }
        
        public virtual Func<Block, Arg, Ret> VisitBlock
        {
            get
            {
                return visitBlock ?? DefaultVisitor;
            }

            set
            {
                visitBlock = value;
            }
        }

        public virtual Func<BreakStmt, Arg, Ret> VisitBreakStmt
        {
            get
            {
                return visitBreakStmt ?? DefaultVisitor;
            }

            set
            {
                visitBreakStmt = value;
            }
        }

        public virtual Func<ContinueStmt, Arg, Ret> VisitContinueStmt
        {
            get
            {
                return visitContinueStmt ?? DefaultVisitor;
            }

            set
            {
                visitContinueStmt = value;
            }
        }

        public virtual Func<DebugStmt, Arg, Ret> VisitDebugStmt
        {
            get
            {
                return visitDebugStmt ?? DefaultVisitor;
            }

            set
            {
                visitDebugStmt = value;
            }
        }

        public virtual Func<DeclStmt, Arg, Ret> VisitDeclStmt
        {
            get
            {
                return visitDeclStmt ?? DefaultVisitor;
            }

            set
            {
                visitDeclStmt = value;
            }
        }

        public virtual Func<ErrorStmt, Arg, Ret> VisitErrorStmt
        {
            get
            {
                return visitErrorStmt ?? DefaultVisitor;
            }

            set
            {
                visitErrorStmt = value;
            }
        }

        public virtual Func<ExprStmt, Arg, Ret> VisitExprStmt
        {
            get
            {
                return visitExprStmt ?? DefaultVisitor;
            }

            set
            {
                visitExprStmt = value;
            }
        }

        public virtual Func<ForeachStmt, Arg, Ret> VisitForeachStmt
        {
            get
            {
                return visitForeachStmt ?? DefaultVisitor;
            }

            set
            {
                visitForeachStmt = value;
            }
        }

        public virtual Func<IfStmt, Arg, Ret> VisitIfStmt
        {
            get
            {
                return visitIfStmt ?? DefaultVisitor;
            }

            set
            {
                visitIfStmt = value;
            }
        }

        public virtual Func<IfTypeStmt, Arg, Ret> VisitIfTypeStmt
        {
            get
            {
                return visitIfTypeStmt ?? DefaultVisitor;
            }

            set
            {
                visitIfTypeStmt = value;
            }
        }

        public virtual Func<ReturnStmt, Arg, Ret> VisitReturnStmt
        {
            get
            {
                return visitReturnStmt ?? DefaultVisitor;
            }

            set
            {
                visitReturnStmt = value;
            }
        }

        public virtual Func<SpecialStmt, Arg, Ret> VisitSpecialStmt
        {
            get
            {
                return visitSpecialStmt ?? DefaultVisitor;
            }

            set
            {
                visitSpecialStmt = value;
            }
        }

        public virtual Func<WhileStmt, Arg, Ret> VisitWhileStmt
        {
            get
            {
                return visitWhileStmt ?? DefaultVisitor;
            }

            set
            {
                visitWhileStmt = value;
            }
        }

        public Func<RTCmdStmt, Arg, Ret> VisitRTCmdStmt
        {
            get
            {
                return visitRTCmdStmt ?? DefaultVisitor;
            }

            set
            {
                visitRTCmdStmt = value;
            }
        }

        public Func<IfObjStmt, Arg, Ret> VisitIfObjStmt
        {
            get
            {
                return visitIfObjStmt ?? DefaultVisitor;
            }

            set
            {
                visitIfObjStmt = value;
            }
        }

        public Func<IfNullStmt, Arg, Ret> VisitIfNullStmt
        {
            get
            {
                return visitIfNullStmt ?? DefaultVisitor;
            }

            set
            {
                visitIfNullStmt = value;
            }
        }
    }
}
