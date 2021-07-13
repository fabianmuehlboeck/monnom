using Nom.Parser;
using System;
using System.Collections.Generic;
using System.Text;
using System.Linq;

namespace Nom.BenchmarkGenerator
{
    public class CheckingVisitor<S> : IDefaultExprVisitor<S, object>, IExprVisitor<S, object>, ITypeVisitor<S, object>, IStmtVisitor<S, object>
    {
        public virtual Func<DefaultBoolExpr, S, object> VisitDefaultBoolExpr => (e, s) => { return e; };
        public virtual Func<BoolExpr, S, object> VisitBoolExpr => (e, s) => { return e; };

        public virtual Func<BinOpExpr, S, object> VisitBinOpExpr => (e, s) =>
        {
            e.Left.Visit(this, s);
            e.Right.Visit(this, s);
            return e;
        };

        public virtual Func<DefaultIdentExpr, S, object> VisitDefaultIdentExpr => (e, s) => { return e; };
        public virtual Func<IdentExpr, S, object> VisitIdentExpr => (e, s) => { return e; };

        public virtual Func<RefIdentExpr, S, object> VisitRefIdentExpr => (e, s) =>
        {
            e.Accessor.Visit(this, s);
            VisitRefIdentifier(e.Identifier, s);
            return e;
        };

        public virtual Func<RefIdentRootExpr, S, object> VisitRefIdentRootExpr => (e, s) =>
        {
            return e;
        };

        public virtual Func<CallExpr, S, object> VisitCallExpr => (e, s) =>
        {
            e.Receiver?.Visit(this, s);
            VisitList(e.Args, s, (arg, s2) => arg.Visit(this, s2));
            return e;
        };

        public virtual Func<NewExpr, S, object> VisitNewExpr => (e, s) =>
        {
            VisitRefQName(e.NewCall.Type, s);
            VisitList(e.Args, s, (arg, s2) => arg.Visit(this, s2));
            return e;
        };

        public virtual Func<DefaultFloatExpr, S, object> VisitDefaultFloatExpr => (e, s) => { return e; };
        public virtual Func<FloatExpr, S, object> VisitFloatExpr => (e, s) => { return e; };

        public virtual Func<UnaryOpExpr, S, object> VisitUnaryOpExpr => (e, s) =>
        {
            e.Expr.Visit(this, s);
            return e;
        };

        public virtual Func<InstanceExpr, S, object> VisitInstanceExpr => (e, s) =>
        {
            VisitList(e.Arguments, s, (arg, s2) => arg.Visit(this, s2));
            return e;
        };

        public virtual Func<DefaultNullExpr, S, object> VisitDefaultNullExpr => (e, s) => { return e; };
        public virtual Func<NullExpr, S, object> VisitNullExpr => (e, s) => { return e; };

        public virtual Func<StructExpr, S, object> VisitStructExpr => (e, s) =>
        {
            VisitList(e.Fields, s, VisitStructFieldDecl);
            VisitList(e.Methods, s, VisitMethodDef);
            VisitList(e.StructAssignments.Select(sa => sa.Expression), s, (e, ss) => e.Visit(this, ss));
            return e;
        };


        public virtual Func<ListExpr, S, object> VisitListExpr => (e, s) =>
        {
            VisitList(e.Elements, s, (elem, s2) => elem.Visit(this, s2));
            return e;
        };

        public virtual Func<DefaultListExpr, S, object> VisitDefaultListExpr => (e, s) =>
        {

            VisitList(e.Elements, s, (elem, s2) => elem.Visit(this as IDefaultExprVisitor<S, IDefaultExpr>, s2));
            return e;
        };
        Func<DefaultListExpr, S, object> IDefaultExprVisitor<S, object>.VisitDefaultListExpr => VisitDefaultListExpr;

        public virtual Func<LambdaExpr, S, object> VisitLambdaExpr => (e, s) =>
        {
            e.ReturnType.Visit(this, s);
            e.Code.Visit(this, s);
            VisitList(e.Arguments, s, VisitArgDecl);
            return e;
        };

        public virtual Func<DefaultStringExpr, S, object> VisitDefaultStringExpr => (e, s) => { return e; };
        public virtual Func<StringExpr, S, object> VisitStringExpr => (e, s) => { return e; };

        public virtual Func<RangeExpr, S, object> VisitRangeExpr => (e, s) =>
        {
            e.RangeStart?.Visit(this, s);
            e.RangeEnd?.Visit(this, s);
            e.RangeStep?.Visit(this, s);
            return e;
        };

        public virtual Func<DefaultIntExpr, S, object> VisitDefaultIntExpr => (e, s) => { return e; };
        public virtual Func<IntExpr, S, object> VisitIntExpr => (e, s) => { return e; };

        public virtual Func<CastExpr, S, object> VisitCastExpr => (e, s) =>
        {
            e.Type.Visit(this, s);
            e.Expr.Visit(this, s);
            return e;
        };

        public virtual Func<StructFieldDecl, S, object> VisitStructFieldDecl => (sfd, s) =>
        {
            sfd.InitExpr?.Visit(this, s);
            sfd.Type?.Visit(this, s);
            return sfd;
        };
        public virtual Func<MethodDef, S, object> VisitMethodDef => (md, s) =>
        {
            md.Code.Visit(this, s);
            md.Returns.Visit(this, s);
            VisitDeclIdentifier(md.Name, s);
            VisitList(md.ArgDefs, s, VisitVarDecl);
            return md;
        };

        public virtual Func<MethodDecl, S, object> VisitMethodDecl => (md, s) =>
        {
            md.Returns.Visit(this, s);
            VisitDeclIdentifier(md.Name, s);
            VisitList(md.Args, s, VisitArgDecl);
            return md;
        };
        public virtual Func<Constructor, S, object> VisitConstructorDef => (cd, s) =>
        {
            new List<ArgDecl>();
            VisitList(cd.Args, s, VisitVarDecl);
            VisitList(cd.SuperCallArgs, s, (e, s) => e.Visit(this, s));
            cd.PreSuperStatements.Visit(this, s);
            cd.PastSuperStatements.Visit(this, s);
            return cd;
        };

        public virtual Func<InheritanceDecl, S, object> VisitInheritanceDecl => (ihd, s) =>
        {
            VisitRefQName(ihd.Parent, s);
            return ihd;
        };
        public virtual Func<FieldDecl, S, object> VisitFieldDecl => (fd, s) =>
        {
            fd.InitExpr?.Visit(this, s);
            fd.Type?.Visit(this, s);
            return fd;
        };

        public virtual Func<StaticFieldDecl, S, object> VisitStaticFieldDecl => (sfd, s) =>
        {
            sfd.InitExpr?.Visit(this as IDefaultExprVisitor<S, IDefaultExpr>, s);
            sfd.Type?.Visit(this, s);
            return sfd;
        };
        public virtual Func<StaticMethodDef, S, object> VisitStaticMethodDef => (smd, s) =>
        {
            smd.Returns.Visit(this, s);
            smd.Code.Visit(this, s);
            VisitDeclIdentifier(smd.Name, s);
            VisitList(smd.ArgDefs, s, VisitVarDecl);
            return smd;
        };



        public virtual Func<InstanceFieldAssignment, S, object> VisitInstanceFieldAssignment => (ifa, s) =>
        {
            ifa.Expr.Visit(this as IDefaultExprVisitor<S, IDefaultExpr>, s);
            return ifa;
        };
        public virtual Func<InstanceDef, S, object> VisitInstanceDef => (idef, s) =>
        {
            VisitList(idef.Assignments, s, VisitInstanceFieldAssignment);
            VisitList(idef.SuperArgs, s, (e, s) => e.Visit(this as IDefaultExprVisitor<S, IDefaultExpr>, s));
            VisitList(idef.Arguments, s, VisitArgDecl);
            return idef;
        };

        protected IEnumerable<object> VisitList<T, R>(IEnumerable<T> l, S s, Func<T, S, R> visitor)
        {
            foreach (T t in l)
            {
                visitor(t, s);
            }
            return null;
        }
        public virtual Func<InterfaceDef, S, object> VisitInterfaceDef => (idef, s) =>
        {
            VisitDeclIdentifier(idef.Name, s);
            VisitList(idef.Implements, s, VisitInheritanceDecl);
            VisitList(idef.Methods, s, VisitMethodDecl);
            VisitList(idef.Interfaces, s, VisitInterfaceDef);
            return idef;
        };

        public virtual Func<ClassDef, S, object> VisitClassDef => (cdef, s) =>
        {
            cdef.SuperClass.Bind(sc => VisitInheritanceDecl(sc, s));
            VisitDeclIdentifier(cdef.Name, s);
            VisitList(cdef.Implements, s, VisitInheritanceDecl);
            VisitList(cdef.ImplementedMethods, s, VisitMethodDef);
            VisitList(cdef.Fields, s, VisitFieldDecl);
            VisitList(cdef.Constructors, s, VisitConstructorDef);
            VisitList(cdef.StaticFields, s, VisitStaticFieldDecl);
            VisitList(cdef.StaticMethods, s, VisitStaticMethodDef);
            VisitList(cdef.Instances, s, VisitInstanceDef);
            VisitList(cdef.Interfaces, s, VisitInterfaceDef);
            VisitList(cdef.Classes, s, VisitClassDef);
            return cdef;
        };

        public virtual Func<Namespace, S, object> VisitNamespaceDef => (nsd, s) =>
        {
            VisitDeclQName(nsd.Name, s);
            VisitList(nsd.Namespaces, s, VisitNamespaceDef);
            VisitList(nsd.Interfaces, s, VisitInterfaceDef);
            VisitList(nsd.Classes, s, VisitClassDef);
            return nsd;
        };
        public virtual Func<CodeFile, S, object> VisitCodeFile => (nsd, s) =>
        {
            VisitList(nsd.Namespaces, s, VisitNamespaceDef);
            VisitList(nsd.Interfaces, s, VisitInterfaceDef);
            VisitList(nsd.Classes, s, VisitClassDef);
            return nsd;
        };


        public virtual Func<ArgDecl, S, object> VisitArgDecl => (arg, s) =>
        {
            arg.Type.Visit(this, s);
            return arg;
        };
        public virtual Func<VarDecl, S, object> VisitVarDecl => (arg, s) =>
        {
            arg.Type.Visit(this, s);
            return arg;
        };

        public virtual Func<DynamicType, S, object> VisitDynamicType => (t, s) => { return t; };

        public virtual Func<ArrayType, S, object> VisitArrayType => (t, s) =>
        {
            t.Type.Visit(this, s);
            return t;
        };

        public virtual Func<ClassType, S, object> VisitClassType => (t, s) =>
        {
            return t;
        };

        public virtual Func<ExprStmt, S, object> VisitExprStmt => (stmt, s) =>
        {
            stmt.Expression.Visit(this, s);
            return stmt;
        };

        public virtual Func<DeclStmt, S, object> VisitDeclStmt => (stmt, s) =>
        {
            stmt.Expr?.Visit(this, s);
            stmt.Type?.Visit(this, s);
            return stmt;
        };

        public virtual Func<ErrorStmt, S, object> VisitErrorStmt => (stmt, s) =>
        {
            stmt.Expr.Visit(this, s);
            return stmt;
        };

        public virtual Func<ContinueStmt, S, object> VisitContinueStmt => (stmt, s) => { return stmt; };

        public virtual Func<ForeachStmt, S, object> VisitForeachStmt => (stmt, s) =>
        {
            VisitVarDecl(stmt.Var, s);
            stmt.Range.Visit(this, s);
            stmt.Block.Visit(this, s);
            return stmt;
        };

        public virtual Func<DebugStmt, S, object> VisitDebugStmt => (stmt, s) => { return stmt; };

        public virtual Func<ReturnStmt, S, object> VisitReturnStmt => (stmt, s) =>
        {
            stmt.Expr.Visit(this, s);
            return stmt;
        };

        public virtual Func<IfTypeStmt, S, object> VisitIfTypeStmt => (stmt, s) =>
        {
            stmt.Type.Visit(this, s);
            stmt.Thenb.Visit(this, s);
            stmt.Elseb?.Visit(this, s);
            return stmt;
        };

        public virtual Func<IfStmt, S, object> VisitIfStmt => (stmt, s) =>
        {
            stmt.GuardExpression.Visit(this, s);
            stmt.ThenBlock.Visit(this, s);
            stmt.ElseBlock.Bind(b => b.Visit(this, s));
            foreach (var eif in stmt.ElseIfs)
            {
                eif.Item1.Visit(this, s);
                eif.Item2.Visit(this, s);
            }
            return stmt;
        };

        public virtual Func<AssignStmt, S, object> VisitAssignStmt => (stmt, s) =>
        {
            stmt.Accessor.Bind(a => a.Visit(this, s));
            stmt.Expression.Visit(this, s);
            return stmt;
        };

        public virtual Func<Block, S, object> VisitBlock => (stmt, s) =>
        {
            VisitList(stmt, s, (stmt2, s2) => stmt2.Visit(this, s2));
            return stmt;
        };

        public virtual Func<BreakStmt, S, object> VisitBreakStmt => (stmt, s) => { return stmt; };

        public virtual Func<WhileStmt, S, object> VisitWhileStmt => (stmt, s) =>
        {
            stmt.Guard.Visit(this, s);
            stmt.Block.Visit(this, s);
            return stmt;
        };

        public virtual Func<RTCmdStmt, S, object> VisitRTCmdStmt => (stmt, s) => { return stmt; };

        public virtual Func<SpecialStmt, S, object> VisitSpecialStmt => (stmt, s) => { return stmt; };

        public virtual Func<DefaultInstanceExpr, S, object> VisitDefaultInstanceExpr => (e, s) => { return e; };

        Func<DefaultBoolExpr, S, object> IDefaultExprVisitor<S, object>.VisitDefaultBoolExpr => VisitBoolExpr;

        Func<DefaultIdentExpr, S, object> IDefaultExprVisitor<S, object>.VisitDefaultIdentExpr => VisitIdentExpr;

        Func<DefaultFloatExpr, S, object> IDefaultExprVisitor<S, object>.VisitDefaultFloatExpr => VisitFloatExpr;

        Func<DefaultInstanceExpr, S, object> IDefaultExprVisitor<S, object>.VisitDefaultInstanceExpr => VisitDefaultInstanceExpr;

        Func<DefaultNullExpr, S, object> IDefaultExprVisitor<S, object>.VisitDefaultNullExpr => VisitDefaultNullExpr;

        Func<DefaultStringExpr, S, object> IDefaultExprVisitor<S, object>.VisitDefaultStringExpr => VisitStringExpr;

        Func<DefaultIntExpr, S, object> IDefaultExprVisitor<S, object>.VisitDefaultIntExpr => VisitIntExpr;

        public virtual Func<Parser.Program, S, object> VisitProgram => (p, s) =>
        {
            VisitList(p.CodeFiles, s, VisitCodeFile);
            return p;
        };

        public virtual Func<TypeArgDecl, S, object> VisitTypeArgDecl => (tad, s) =>
        {
            tad.LowerBound?.Visit(this, s);
            tad.UpperBound?.Visit(this, s);
            return tad;
        };

        public virtual Func<IArgIdentifier<Identifier, TypeArgDecl>, S, object> VisitDeclIdentifier => (id, s) =>
        {
            VisitList(id.Arguments, s, VisitTypeArgDecl);
            return id;
        };

        public virtual Func<DeclQName, S, object> VisitDeclQName => (qn, s) =>
        {
            VisitList(qn, s, VisitDeclIdentifier);
            return qn;
        };

        public virtual Func<IArgIdentifier<Identifier, IType>, S, object> VisitRefIdentifier => (id, s) =>
        {
            VisitList(id.Arguments, s, (t, s2) => t.Visit(this, s2));
            return id;
        };

        public virtual Func<RefQName, S, object> VisitRefQName => (qn, s) =>
        {
            VisitList(qn, s, VisitRefIdentifier);
            return qn;
        };

        public virtual Func<IArgIdentifier<Identifier, IType>, S, object> VisitTypeIdentifier => (id, s) =>
        {
            VisitList(id.Arguments, s, (t, s2) => t.Visit(this, s2));
            return id;
        };

        public virtual Func<TypeQName, S, object> VisitTypeQName => (qn, s) =>
        {
            VisitList(qn, s, VisitTypeIdentifier);
            return qn;
        };

        public Func<MaybeType, S, object> VisitMaybeType => (mb, s) =>
        {
            mb.Type.Visit(this, s);
            return mb;
        };

        public Func<ProbablyType, S, object> VisitProbablyType => (pb, s) =>
        {
            pb.Type.Visit(this, s);
            return pb;
        };

        public Func<IfObjStmt, S, object> VisitIfObjStmt => (stmt, s) =>
        {
            stmt.ThenBlock.Visit(this, s);
            stmt.ElseBlock.ActionBind(b => b.Visit(this, s));
            return stmt;
        };

        public Func<IfNullStmt, S, object> VisitIfNullStmt => (stmt, s) =>
        {
            stmt.ThenBlock.Visit(this, s);
            stmt.ElseBlock.ActionBind(b => b.Visit(this, s));
            return stmt;
        };

        public Func<LetExpr, S, object> VisitLetExpr => (expr, s) =>
        {
            expr.Declaration.Type.Visit(this, s);
            expr.Declaration.InitExpr.Visit(this, s);
            expr.Body.Visit(this, s);
            return expr;
        };

        public Func<LetVarExpr, S, object> VisitLetVarExpr => (expr, s) =>
        {
            expr.BindExpr.Visit(this, s);
            expr.Body.Visit(this, s);
            return expr;
        };
    }
}